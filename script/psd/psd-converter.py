from psd_tools import PSDImage
from psd_tools.api.layers import Group, Layer, PixelLayer
from typing import cast, List, Dict, Any, Optional
import os
import json

# test dir
__dirname = os.path.dirname(os.path.abspath(__file__))
test_file = os.path.join(__dirname, '../../test/res/zundamon.psd')
test_file = os.path.normpath(test_file)

psd = PSDImage.open(test_file)

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

# save pixel layer to disk


def save_pixel_layers(layers: List[PixelLayer], output_dir: str):
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    for layer in layers:
        comp = layer.composite()
        name = layer.name.strip().replace('\x00', "")
        assert comp is not None, f"Layer {name} has no composite image."
        output_path = os.path.join(output_dir, f"{name}.png")
        comp.save(output_path)

# should in order to list


def generate_pixel_layers_meta(layers: List[PixelLayer], pev: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
    meta = {}
    if (pev is not None):
        meta = pev

    meta['layers'] = []
    for layer in layers:
        layer_meta = {
            'path': layer.name.strip().replace('\x00', "") + '.png',
            'box': {
                'x': layer.left,
                'y': layer.top,
                'w': layer.width,
                'h': layer.height
            },
        }
        meta['layers'].append(layer_meta)

    return meta
    pass


def save_pixel_layer_meta(meta: Dict[str, Any], output_path: str):
    with open(output_path, 'w', encoding='utf-8') as f:
        json.dump(meta, f, ensure_ascii=False, indent=4)

meta = {}
meta['version'] = '0.0.1'
meta['canvas'] = {
    "width": psd.width,
    "height": psd.height
}
meta = generate_pixel_layers_meta(pixel_layers, meta)

save_pixel_layer_meta(meta, os.path.join(
    __dirname, '../../build/script/test/layers.json'))
save_pixel_layers(pixel_layers, os.path.join(
    __dirname, '../../build/script/test'))
