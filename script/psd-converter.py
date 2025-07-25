from psd_tools import PSDImage
from psd_tools.api.layers import Group, Layer, PixelLayer
from typing import cast, List, Dict, Any, Optional
import os
import json
import argparse


def main():
    parser = argparse.ArgumentParser(
        description='Convert PSD file to pixel layers.')
    parser.add_argument('input', type=str,
                        help='Path to the PSD file to convert')
    parser.add_argument('--output', type=str, default='.',
                        help='Output directory for pixel layers and metadata')
    args = parser.parse_args()

    input_file = os.path.abspath(args.input)
    output_dir = os.path.abspath(args.output)

    if not os.path.exists(input_file):
        raise FileNotFoundError(f"Input file '{input_file}' does not exist.")
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    psd = PSDImage.open(args.input)

    pixel_layers: List[PixelLayer] = []

    def get_pixel_layer(layer: Layer):
        if (layer.kind == 'pixel'):
            pixel_layers.append(cast(PixelLayer, layer))
        if (layer.kind == 'group'):
            group = cast(Group, layer)
            for child in group:
                get_pixel_layer(child)
        pass

    for layer in psd:
        get_pixel_layer(layer)

    def save_pixel_layers(layers: List[PixelLayer], output_dir: str):
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        for layer in layers:
            print (f"Saving layer: {layer.name}")
            comp = layer.composite()
            name = layer.name.replace(" ", "").replace('\x00', "")
            assert comp is not None, f"Layer {name} has no composite image."
            output_path = os.path.join(output_dir, f"{name}.png")
            comp.save(output_path)

    def generate_layer_points(layer: PixelLayer) -> Dict[str, Any]:
        left = layer.left
        top = layer.top
        right = layer.right
        bottom = layer.bottom
        return {
            "position": [left, top, right, top, right, bottom, left, bottom],
            "uv": [0, 1, 2, 0, 2, 3],
        }
    # should in order to list

    def generate_pixel_layers_meta(layers: List[PixelLayer], pev: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        meta = {}
        if (pev is not None):
            meta = pev

        meta['layers'] = []
        for layer in layers:
            layer_meta = {
                'path': layer.name.strip().replace('\x00', "") + '.png',
                'vertices': generate_layer_points(layer),
                'width': layer.width,
                'height': layer.height
            }
            meta['layers'].append(layer_meta)

        return meta
        pass

    def save_pixel_layer_meta(meta: Dict[str, Any], output_path: str):
        with open(output_path, 'w', encoding='utf-8') as f:
            json.dump(meta, f, ensure_ascii=False, indent=4)
    
    print("Writing project data")
    meta = {}
    meta['version'] = '0.0.1'
    meta['canvas'] = {
        "width": psd.width,
        "height": psd.height
    }
    meta = generate_pixel_layers_meta(pixel_layers, meta)

    save_pixel_layer_meta(meta, os.path.join(output_dir, 'layers.json'))
    save_pixel_layers(pixel_layers, output_dir)


if __name__ == '__main__':
    main()
