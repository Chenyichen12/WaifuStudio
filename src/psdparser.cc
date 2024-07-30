//
// Created by chen_yichen on 2024/7/30.
//

#include "include/psdparser.h"
#include "LayeredFile/LayeredFile.h"

using namespace PhotoshopAPI;
namespace Parser {
struct PsdParseCallBack : public ProgressCallback {
  void increment() noexcept override { ProgressCallback::increment(); }
};
PsdParser::PsdParser(const QString &path) { this->path = path; }

void PsdParser::Parse() {
  File f = File(path.toStdString());
  auto psFile = std::make_unique<PhotoshopFile>();
  auto callback = PsdParseCallBack();
  psFile->read(f, callback);
  auto &&header = psFile->m_Header;
  parseHeight = (int)header.m_Height;
  parseWidth = (int)header.m_Width;
  for (const auto &item : psFile->m_LayerMaskInfo.m_LayerInfo.m_LayerRecords) {
    if (item.m_Top == 0 && item.m_Left == 0 && item.m_Right == 0 &&
        item.m_Bottom == 0) {
      if (item.m_LayerName.getString() == "</Layer group>") {
        std::cout << "devider" << '\n';
      } else
        std::cout << item.m_LayerName.getString() << '\n';
    }
  }
}
} // namespace Parser
