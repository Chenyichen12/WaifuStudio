//
// Created by chen_yichen on 2024/7/30.
//

#include "include/psdparser.h"

#include <qcolor.h>

#include <stack>

#include "LayeredFile/LayeredFile.h"
#include "model/id_allocation.h"
#include "model/layer_model.h"
#include "model/tree_manager.h"
using namespace PhotoshopAPI;
namespace Parser {

ProjectModel::LayerBitmap* createBitmap(const LayerRecord& info,
                                        ChannelImageData& data) {
  const int a = data.getChannelIndex(Enum::ChannelID::Alpha);
  const int r = data.getChannelIndex(Enum::ChannelID::Red);
  const int g = data.getChannelIndex(Enum::ChannelID::Green);
  const int b = data.getChannelIndex(Enum::ChannelID::Blue);

  const auto& aVec = data.extractImageData<bpp8_t>(a);
  const auto& rVec = data.extractImageData<bpp8_t>(r);
  const auto& gVec = data.extractImageData<bpp8_t>(g);
  const auto& bVec = data.extractImageData<bpp8_t>(b);

  auto* imagePtr = new unsigned char[aVec.size() * 4];
  for (int i = 0; i < aVec.size(); ++i) {
    imagePtr[i * 4] = rVec[i];
    imagePtr[i * 4 + 1] = gVec[i];
    imagePtr[i * 4 + 2] = bVec[i];
    imagePtr[i * 4 + 3] = aVec[i];
  }
  const int id = IdAllocation::getInstance().allocate();

  const auto bitmap = new ProjectModel::LayerBitmap(id);

  bitmap->top = info.m_Top;
  bitmap->left = info.m_Left;
  bitmap->width = info.getWidth();
  bitmap->height = info.getHeight();

  bitmap->rawBytes = imagePtr;
  return bitmap;
}

PsdParser::PsdParser(const QString& path) { this->path = path; }

void PsdParser::Parse() {
  File f = File(path.toStdString());
  auto psFile = std::make_unique<PhotoshopFile>();
  auto callback = ProgressCallback();
  psFile->read(f, callback);
  auto&& header = psFile->m_Header;
  parseHeight = (int)header.m_Height;
  parseWidth = (int)header.m_Width;

  auto bitmapManagerPtr = std::make_unique<ProjectModel::BitmapManager>();

  typedef ProjectModel::TreeNode<ProjectModel::Layer> nodeType;
  auto parseStack = std::stack<nodeType*>();
  auto psTreeManager = std::make_unique<ProjectModel::TreeManager>();
  auto controllerTreeManger = std::make_unique<ProjectModel::TreeManager>();

  parseStack.push(psTreeManager->getRoot());

  try {
    for (int i = psFile->m_LayerMaskInfo.m_LayerInfo.m_LayerRecords.size() - 1;
         i >= 0; --i) {
      const auto& record =
          psFile->m_LayerMaskInfo.m_LayerInfo.m_LayerRecords[i];

      auto& image = psFile->m_LayerMaskInfo.m_LayerInfo.m_ChannelImageData[i];

      if (record.m_Top == 0 && record.m_Left == 0 && record.m_Right == 0 &&
          record.m_Bottom == 0) {
        if (record.m_LayerName.getString() == "</Layer group>") {
          parseStack.pop();
          continue;
        }
        auto layerInfo =
            std::shared_ptr<ProjectModel::Layer>(new ProjectModel::PsGroupLayer(
                QString::fromStdString(record.m_LayerName.getString())));

        auto* node = new nodeType(layerInfo);
        node->setParent(parseStack.top());
        parseStack.top()->getChildren().push_back(node);
        parseStack.push(node);

       
      } else {
        auto* bitmap = createBitmap(record, image);
        bitmapManagerPtr->addAsset(bitmap);

        auto layerInfo =
            std::shared_ptr<ProjectModel::Layer>(new ProjectModel::BitmapLayer(
                QString::fromStdString(record.m_LayerName.getString()),
                *bitmap));

        auto* node = new nodeType(layerInfo);
        parseStack.top()->getChildren().push_back(node);
        node->setParent(parseStack.top());

        auto controllerNode = new nodeType(layerInfo);
        controllerTreeManger->getRoot()->getChildren().push_back(
            controllerNode);
        controllerNode->setParent(controllerTreeManger->getRoot());
      }
    }

  } catch (...) {
    std::cout << "bad parse";
    return;
  }
  this->_controllerTree = std::move(controllerTreeManger);
  this->_bitmapManager = std::move(bitmapManagerPtr);
  this->_psTree = std::move(psTreeManager);
}

PsdParser::~PsdParser() = default;
}  // namespace Parser
