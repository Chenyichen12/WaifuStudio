//
// Created by chen_yichen on 2024/7/30.
//

#include "include/psdparser.h"

#include "LayeredFile/LayeredFile.h"
#include "include/id_allocation.h"
#include "include/layer_model.h"
#include "include/tree_manager.h"

#include <stack>
using namespace PhotoshopAPI;
namespace Parser {

LayerBitmap* createBitmap(const LayerRecord& info, ChannelImageData& data) {
  const auto& aVec = data.extractImageData<bpp8_t>(
      data.getChannelIndex(Enum::ChannelID::Alpha));
  const auto& rVec =
      data.extractImageData<bpp8_t>(data.getChannelIndex(Enum::ChannelID::Red));
  const auto& gVec = data.extractImageData<bpp8_t>(
      data.getChannelIndex(Enum::ChannelID::Green));
  const auto& bVec = data.extractImageData<bpp8_t>(
      data.getChannelIndex(Enum::ChannelID::Blue));

  auto* imagePtr = new unsigned char[aVec.size() * 4];
  for (int i = 0; i < aVec.size(); ++i) {
    imagePtr[i * 4] = aVec[i];
    imagePtr[i * 4 + 1] = rVec[i];
    imagePtr[i * 4 + 2] = gVec[i];
    imagePtr[i * 4 + 3] = bVec[i];
  }
  int id = IdAllocation::getInstance().allocate();

  auto bitmap = new LayerBitmap(id);

  bitmap->top = info.m_Top;
  bitmap->left = info.m_Left;
  bitmap->width = info.getWidth();
  bitmap->height = info.getHeight();

  bitmap->rawBytes = imagePtr;
  return bitmap;
}

struct PsdParseCallBack : public ProgressCallback {
  void increment() noexcept override { ProgressCallback::increment(); }
};
PsdParser::PsdParser(const QString& path) { this->path = path; }

void PsdParser::Parse() {
  File f = File(path.toStdString());
  auto psFile = std::make_unique<PhotoshopFile>();
  auto callback = PsdParseCallBack();
  psFile->read(f, callback);
  auto&& header = psFile->m_Header;
  parseHeight = (int)header.m_Height;
  parseWidth = (int)header.m_Width;

  auto bitmapManagerPtr = std::unique_ptr<BitmapManager>();

  for (int i = psFile->m_LayerMaskInfo.m_LayerInfo.m_LayerRecords.size() - 1;
       i >= 0; --i) {
    const auto& record = psFile->m_LayerMaskInfo.m_LayerInfo.m_LayerRecords[i];

    auto& image = psFile->m_LayerMaskInfo.m_LayerInfo.m_ChannelImageData[i];

    if (record.m_Top == 0 && record.m_Left == 0 && record.m_Right == 0 &&
        record.m_Bottom == 0) {
      if (record.m_LayerName.getString() == "</Layer group>") {
        std::cout << "devider" << '\n';
      }
    } else {
      auto* bitmap = createBitmap(record, image);
      bitmapManagerPtr->addAsset(bitmap);
    }
  }
}

}  // namespace Parser
