#include "renderer.h"


namespace rdc {
    ModelRenderer::ModelRenderer(VulkanDriver *driver) {
        _driver = driver;
    }
    ModelRenderer::~ModelRenderer(){}

}