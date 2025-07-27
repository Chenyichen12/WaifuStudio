layout(binding = 0, std140) uniform UniformBufferObject{
    mat3 transform;
} ubo;

layout(binding = 1) uniform sampler2D main_tex;

#ifdef VERTEX
layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec2 out_uv;

void main(){
    vec3 pos = ubo.transform * vec3(in_pos, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
    out_uv = in_uv;
}
#endif

#ifdef FRAGMENT
layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 out_color; /*
{
    "format": "srgba32",
}
*/

void main(){
    vec4 result_color = texture(main_tex, in_uv);
    if (result_color.a < 0.01) {
        discard; // 丢弃透明像素
    }
    out_color = result_color;   
}

#endif