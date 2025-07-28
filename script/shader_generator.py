import sys
import re
import subprocess
import os
import json
import argparse
from typing import List, TypedDict, Optional, Tuple


def get_shader_content(content: str) -> Tuple[str, str]:
    pattern = r"#ifdef VERTEX(.*?)#endif"
    matches = re.findall(pattern, content, re.DOTALL)
    if (len(matches) == 0):
        raise ValueError("未找到顶点着色器内容")
    m1 = matches[0]

    pattern = r"#ifdef FRAGMENT(.*?)#endif"
    matches = re.findall(pattern, content, re.DOTALL)
    if (len(matches) == 0):
        raise ValueError("未找到片段着色器内容")
    m2 = matches[0]
    return m1, m2


g_glsl_c_type_map = {
    "float": "float",
    "vec2": "glm::vec2",
    "vec3": "glm::vec3",
    "vec4": "glm::vec4",
    "mat2": "glm::mat2",
    "mat3": "glm::mat3",
    "mat4": "glm::mat4",
    "int": "int",
}

g_glsl_c_typesize_map = {
    "float": 4,
    "vec2": 8,
    "vec3": 16,
    "vec4": 16,
    "mat2": 16,
    "mat3": 48,
    "mat4": 64,
    "int": 4,
}

g_glsl_c_output_format_map = {
    "srgba32f": "VK_FORMAT_R32G32B32A32_SFLOAT",
    "srgb32f": "VK_FORMAT_R32G32B32_SFLOAT",
}

g_glsl_c_desc_type_map = {
    "ubo": "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER",
    "sampler2D": "VK_DESCRIPTOR_TYPE_SAMPLER",

}


class BlockVariable(TypedDict):
    name: str
    type: str


class UnifromStructVariable(TypedDict):
    binding: int
    name: str
    member: List[BlockVariable]

    alia: str


class UniformVarible(TypedDict):
    name: str
    type: str
    binding: int


class OutputVariable(TypedDict):
    location: int
    name: str
    type: str
    format: str  # srgba32, etc.


def parse_shader_structs_uniforms(content: str):
    values: List[UnifromStructVariable] = []
    binding_pattern = re.compile(
        r'layout\s*\(\s*binding\s*=\s*(\d+).*\)\s+uniform\s+(\w+)\s*{([^}]*)}\s*(\w+)\s*;'
    )
    for match in binding_pattern.finditer(content):
        binding = int(match.group(1))
        struct_name = match.group(2)
        struct_body = match.group(3)
        struct_alias = match.group(4)

        members: List[BlockVariable] = []
        member_pattern = re.compile(r'(\w+)\s+(\w+)\s*;')
        for m in member_pattern.finditer(struct_body):
            var_type = m.group(1)
            var_name = m.group(2)
            members.append({
                "name": var_name,
                "type": var_type
            })

        values.append({
            "binding": binding,
            "name": struct_name,
            "member": members,
            "alia": struct_alias
        })
    return values


def parse_shader_uniforms(content: str) -> List[UniformVarible]:
    values: List[UniformVarible] = []
    variable_pattern = re.compile(
        r'layout\s*\(\s*binding\s*=\s*(\d+)\s*\)\s+uniform\s+(\w+)\s+(\w+)\s*;'
    )
    for match in variable_pattern.finditer(content):
        binding_index = int(match.group(1))
        var_type = match.group(2)
        var_name = match.group(3)
        values.append({
            "type": var_type,
            "binding": binding_index,
            "name": var_name,
        })

    # storage buffer
    storage_pattern = re.compile(
        r'layout\s*\(\s*binding\s*=\s*(\d+).*\)\s+buffer\s+(\w+)\s*{([^}]*)};'
    )
    for match in storage_pattern.finditer(content):
        binding_index = int(match.group(1))
        var_type = "buffer"
        var_name = match.group(2)
        values.append({
            "type": var_type,
            "binding": binding_index,
            "name": var_name,
        })

    return values


def parse_shader_outputs(content: str) -> List[OutputVariable]:

    # output should in frag content
    pattern = r"#ifdef FRAGMENT(.*?)#endif"
    matches = re.findall(pattern, content, re.DOTALL)
    if (len(matches) == 0):
        raise ValueError("未找到片段着色器内容")
    frag_content = matches[0]

    re_text = r"layout\s*\(\s*location\s*=\s*(\d+)\s*\)\s*out\s+(\w+)\s+(\w+)\s*;"
    re_text += r"(?:\s*/\*\{(.*?)\}\*/)?"
    output_pattern = re.compile(
        re_text,
        re.DOTALL
    )
    outputs: List[OutputVariable] = []
    for match in output_pattern.finditer(frag_content):
        location = int(match.group(1))
        out_type = match.group(2)
        out_name = match.group(3)

        extra_info = match.group(4)
        json_data = None

        format = "srgba32f"  # default format
        if extra_info:
            try:
                json_data = json.loads("{" + extra_info + "}")
                format = json_data.get("format", format)

            except json.JSONDecodeError as e:
                print(f"JSON decode error: {e} in {extra_info}")

        outputs.append({
            "location": location,
            "type": out_type,
            "name": out_name,
            "format": format
        })
    return outputs


class ShaderHeaderGenerator:
    _output_val: List[OutputVariable]
    _uniforms: List[UniformVarible]
    _structs: List[UnifromStructVariable]
    _vertex_shader: str
    _fragment_shader: str
    _shader_name: str
    _shader_path: str
    _glslc_path: str

    def __init__(self, vertex_shader: str, fragment_shader: str,
                 uniforms: List[UniformVarible], structs: List[UnifromStructVariable],
                 outputs_val: List[OutputVariable], shader_path: str,
                 glslc_path: str = "glslc"):
        self._vertex_shader = vertex_shader
        self._fragment_shader = fragment_shader
        self._uniforms = uniforms
        self._structs = structs
        self._output_val = outputs_val
        self._glslc_path = glslc_path
        self._shader_path = shader_path
        self._shader_name = os.path.basename(shader_path).split('.')[0]

    def _generate_unifrom_structs(self) -> str:
        result = ""
        for struct in self._structs:
            size = 0
            code = f"struct {struct['name']} {{\n"
            for member in struct['member']:
                member_size = g_glsl_c_typesize_map.get(member['type']) or 0
                ssize = 16 - size % 16
                if (member_size > ssize and ssize != 0) or (member["type"] == "mat3" or member["type"] == "vec3"):
                    code += "alignas(16) "  # ensure alignment
                    size = 0

                code += f"{g_glsl_c_type_map.get(member['type'])} {member['name']};\n"
                size += member_size
            code += "};\n"
            result += code
        return result

    def _clean_binary_output(self):
        dic = os.path.dirname(self._shader_path)
        if not os.path.exists(dic):
            return
        
        # Remove existing binary files
        dicallfiles = os.listdir(dic)
        for file in dicallfiles:
            if file.startswith(self._shader_name) and file.endswith(('.spv', '.vert', '.frag')):
                file_path = os.path.join(dic, file)
                if os.path.isfile(file_path):
                    os.remove(file_path)

    def _generate_binary_output(self):
        dic = os.path.dirname(self._shader_path)
        if not os.path.exists(dic):
            raise FileNotFoundError(
                f"Shader directory '{dic}' does not exist.")

        # subprocess.run([self._glslc_path, self._shader_path, "-o", os.path.join(dic, f"{self._shader_name}.spv")],
        #                check=True, capture_output=True)
        with open(os.path.join(dic, f"{self._shader_name}.vert"), 'wb') as f:
            f.write(self._vertex_shader.encode('utf-8'))
        with open(os.path.join(dic, f"{self._shader_name}.frag"), 'wb') as f:
            f.write(self._fragment_shader.encode('utf-8'))
        try:
            subprocess.run([self._glslc_path, os.path.join(
                dic, f"{self._shader_name}.vert"), "-o", os.path.join(dic, f"{self._shader_name}.vert.spv")])
            subprocess.run([self._glslc_path, os.path.join(
                dic, f"{self._shader_name}.frag"), "-o", os.path.join(dic, f"{self._shader_name}.frag.spv")])
        except subprocess.CalledProcessError as e:
            print(f"Error generating SPIR-V binary: {e}")
            self._clean_binary_output()
            sys.exit(1)

        vertex_spv = ""
        fragment_spv = ""
        with open(os.path.join(dic, f"{self._shader_name}.vert.spv"), 'rb') as v_file:
            vertex_spv = v_file.read()
        with open(os.path.join(dic, f"{self._shader_name}.frag.spv"), 'rb') as f_file:
            fragment_spv = f_file.read()
        self._clean_binary_output()

        # binary
        code = f"static constexpr uint32_t vertex_spv[] = {{\n"
        for i in range(0, len(vertex_spv), 4):
            code += f"    0x{int.from_bytes(vertex_spv[i:i + 4], 'little'):08x},"
        code += f"}};\n\n"
        code += f"static constexpr uint32_t fragment_spv[] = {{\n"
        for i in range(0, len(fragment_spv), 4):
            code += f"    0x{int.from_bytes(fragment_spv[i:i + 4], 'little'):08x},"
        code += f"}};\n\n"
        return code
        pass

    def generate_header(self) -> str:
        code = "// Auto-generated shader header file\n"
        code += "#pragma once\n\n"
        code += "#include <vulkan/vulkan_core.h>\n"
        code += "#include <glm/glm.hpp>\n"
        # namespce_begin
        code += f"namespace shader_gen\n{{\n"

        # class begin
        code += f"class {self._shader_name}\n{{\n"
        code += f"public:\n"

        # define struct
        code += self._generate_unifrom_structs()
        for struct in self._structs:
            code += f"static constexpr struct {{\n"
            code += f"{struct['name']} data;\n"
            code += f"uint32_t binding;\n"
            code += f"VkDescriptorType desc_type;\n"
            code += f"}} {struct['alia']} = {{{{}}, {struct['binding']}, {g_glsl_c_desc_type_map['ubo']}}};\n\n"

        for uniform in self._uniforms:
            code += f"static constexpr struct {{\n"
            code += f"const char* type;\n"
            code += f"uint32_t binding;\n"
            code += f"VkDescriptorType desc_type;\n"
            code += f"}} {uniform['name']} = {{\"{uniform['type']}\", {uniform['binding']}, {g_glsl_c_desc_type_map.get(uniform['type'])}}};\n\n"

        for output in self._output_val:
            code += f"static constexpr struct {{\n"
            code += f"const char* type;\n"
            code += f"uint32_t location;\n"
            code += f"VkFormat format;\n"
            code += f"}} {output['name']} = {{\"{output['type']}\", {output['location']}, {g_glsl_c_output_format_map.get(output['format'])}}};\n\n"
        
        code += self._generate_binary_output()
        code += f"}};\n"

        code += f"}}\n"
        # namespce_end
        return code


def main():
    parser = argparse.ArgumentParser(
        description='Generate cpp header for glsl shader')
    parser.add_argument('input', type=str, help='Path to the glsl file')
    parser.add_argument('--output', type=str, default='',
                        help='Output header file output')
    parser.add_argument("--glslc", type=str, default="glslc",
                        help="Path to the glslc compiler")

    args = parser.parse_args()
    input_file = os.path.abspath(args.input)
    output_file = os.path.abspath(
        args.output) if args.output else os.path.splitext(input_file)[0] + '.gen.h'
    if not os.path.exists(input_file):
        raise FileNotFoundError(f"Input file '{input_file}' does not exist.")
    try:
        with open(input_file, 'r') as f:
            shader_code = f.read()
        # vertex_shader, fragment_shader = get_shader_content(shader_code)
        uniforms = parse_shader_uniforms(shader_code)
        structs = parse_shader_structs_uniforms(shader_code)
        color_outputs = parse_shader_outputs(shader_code)

        shader_name = os.path.basename(input_file).split('.')[0]
        content_version = f"#version 450\n"
        content = shader_code.removeprefix(content_version)
        vertex_code = content_version + f"#define VERTEX\n" + content
        fragment_code = content_version + f"#define FRAGMENT\n" + content

        header_generator = ShaderHeaderGenerator(
            vertex_shader=vertex_code,
            fragment_shader=fragment_code,
            uniforms=uniforms,
            structs=structs,
            outputs_val=color_outputs,
            shader_path=input_file,
            glslc_path=args.glslc
        )

        c_code = header_generator.generate_header()
        # print(c_code)
        with open(output_file, 'w') as f:
            f.write(c_code)

    except Exception as e:
        print(f"Error processing file {input_file}: {e}")


if __name__ == "__main__":
    main()
