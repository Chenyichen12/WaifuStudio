#include "psdparser.h"

#include <QStack>

#include "LayeredFile/LayeredFile.h"
namespace WaifuL2d {
PsdParser::PsdParser(const std::wstring& path) : path(path) {
}
ParseResult* PsdParser::getResult() { return result.get(); }

// rgba
std::unique_ptr<unsigned char[]> extractBitmap(
    PhotoshopAPI::ChannelImageData& data) {
  const int a = data.getChannelIndex(PhotoshopAPI::Enum::ChannelID::Alpha);
  const int r = data.getChannelIndex(PhotoshopAPI::Enum::ChannelID::Red);
  const int g = data.getChannelIndex(PhotoshopAPI::Enum::ChannelID::Green);
  const int b = data.getChannelIndex(PhotoshopAPI::Enum::ChannelID::Blue);
  const auto& aVec = data.extractImageData<PhotoshopAPI::bpp8_t>(a);
  const auto& rVec = data.extractImageData<PhotoshopAPI::bpp8_t>(r);
  const auto& gVec = data.extractImageData<PhotoshopAPI::bpp8_t>(g);
  const auto& bVec = data.extractImageData<PhotoshopAPI::bpp8_t>(b);

  auto imagePtr = std::make_unique<unsigned char[]>(aVec.size() * 4);
  for (int i = 0; i < aVec.size(); ++i) {
    imagePtr[i * 4] = rVec[i];
    imagePtr[i * 4 + 1] = gVec[i];
    imagePtr[i * 4 + 2] = bVec[i];
    imagePtr[i * 4 + 3] = aVec[i];
  }
  return imagePtr;
}

void PsdParser::parse() {
  using namespace PhotoshopAPI;
  result = std::make_unique<ParseResult>();
  result->success = false;

  File f = File(path);
  auto psFile = std::make_unique<PhotoshopFile>();
  ProgressCallback callback{};
  psFile->read(f, callback);
  auto&& header = psFile->m_Header;
  result->height = header.m_Height;
  result->width = header.m_Width;

  // setup root node
  int idIncreasement = 0;
  auto rootNode = new TreeNode();
  rootNode->id = idIncreasement++;
  rootNode->name = "root";
  rootNode->type = -1;
  result->root = rootNode;

  QStack<TreeNode*> parseStack;
  parseStack.push(rootNode);

  try {
    for (int i = psFile->m_LayerMaskInfo.m_LayerInfo.m_LayerRecords.size() - 1;
         i >= 0; i--) {
      const auto& record =
          psFile->m_LayerMaskInfo.m_LayerInfo.m_LayerRecords[i];
      auto& image = psFile->m_LayerMaskInfo.m_LayerInfo.m_ChannelImageData[i];
      if (record.m_Top == 0 && record.m_Left == 0 && record.m_Right == 0 &&
          record.m_Bottom == 0) {
        if (record.m_LayerName.getString() == "</Layer group>") {
          parseStack.pop();
          continue;
        }

        auto item = new TreeNode();
        item->id = idIncreasement++;
        item->type = 2;
        item->name = QString::fromStdString(record.m_LayerName.getString());
        item->visible = !record.m_BitFlags.m_isHidden;
        item->parent = parseStack.top();
        parseStack.top()->children.append(item);
        parseStack.push(item);
      } else {
        auto imagePtr = extractBitmap(image);
        auto layeritem = new TreeNode();
        layeritem->id = idIncreasement++;
        layeritem->type = 1;
        layeritem->name =
            QString::fromStdString(record.m_LayerName.getString());
        layeritem->visible = !record.m_BitFlags.m_isHidden;
        layeritem->parent = parseStack.top();
        parseStack.top()->children.append(layeritem);

        MeshNode meshNode;
        meshNode.bindTreeid = layeritem->id;
        auto rawImagePtr = imagePtr.release();
        meshNode.texture = QImage(
            rawImagePtr, record.getWidth(), record.getHeight(),
            QImage::Format_RGBA8888,
            [](void* ptr) { delete[] static_cast<unsigned char*>(ptr); },
            rawImagePtr);
        meshNode.scenePosition = {QPointF(record.m_Left, record.m_Top),
                                  QPointF(record.m_Right, record.m_Top),
                                  QPointF(record.m_Right, record.m_Bottom),
                                  QPointF(record.m_Left, record.m_Bottom)};
        meshNode.uvs = {
            QPointF(0, 0),
            QPointF(1, 0),
            QPointF(1, 1),
            QPointF(0, 1),
        };
        meshNode.incident = {0, 1, 2, 0, 2, 3};

        result->meshNode.insert(layeritem->id, std::move(meshNode));
      }
    }
    result->success = true;
  } catch (...) {
    std::cout << "bad parse";
    return;
  }
}
}  // namespace WaifuL2d