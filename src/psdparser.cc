//
// Created by chen_yichen on 2024/7/30.
//

#include "psdparser.h"

#include <stack>
#include <string>
#include "LayeredFile/LayeredFile.h"
#include "model/id_allocation.h"
#include "model/layer.h"
#include "model/tree_manager.h"
using namespace PhotoshopAPI;
namespace Parser {

ProjectModel::BitmapAsset* createBitmap(const LayerRecord& info,
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

  const auto bitmap = new ProjectModel::BitmapAsset(id);

  bitmap->top = info.m_Top;
  bitmap->left = info.m_Left;
  bitmap->width = info.getWidth();
  bitmap->height = info.getHeight();

  bitmap->rawBytes = imagePtr;
  return bitmap;
}

/**
 * bitmap builder to build a BitmapLayer
 */
class BitmapLayerBuilder {
 private:
  std::optional<QIcon> icon;
  QString name;
  bool isVisible = true;
  int id = -1;

  int assetId = -1;

 public:
  std::unique_ptr<ProjectModel::BitmapLayer> build() {
    if (icon == std::nullopt) {
      icon = QIcon(":/icon/question.png");
    }
    if (name == "") {
      name = "no name";
    }

    auto bitmap = std::make_unique<ProjectModel::BitmapLayer>(id, assetId);
    bitmap->setText(name);
    bitmap->setVisible(isVisible);
    bitmap->setIcon(icon.value());
    return bitmap;
  }

  /**
   * use to set icon
   * @param bitmap raw byte of image information
   */
  void setIcon(const ProjectModel::BitmapAsset* bitmap) {
    auto image = QImage(bitmap->rawBytes, bitmap->width, bitmap->height,
                        QImage::Format_RGBA8888);
    image = image.scaledToHeight(20);
    auto pixmap = QPixmap::fromImage(image);
    this->icon = QIcon(pixmap);
    this->assetId = bitmap->ID();
  };

  void setText(const QString& name) { this->name = name; }

  void setText(std::string name) { this->name = QString::fromStdString(name); }

  void setVisible(bool vis) { this->isVisible = vis; }

  void allocationId() { this->id = IdAllocation::getInstance().allocate(); }
};

PsdParser::PsdParser(const QString& path) { this->path = path; }

/**
 * parse the psd file to project
 */
void PsdParser::Parse() {
#ifdef __Apple__
  char* charptr = path.toUtf8().data();
  std::string s = charptr;
#else
  auto s = path.toStdWString();
#endif
  
  File f = File(s);
  auto psFile = std::make_unique<PhotoshopFile>();
  auto callback = ProgressCallback();
  psFile->read(f, callback);
  auto&& header = psFile->m_Header;
  parseHeight = (int)header.m_Height;
  parseWidth = (int)header.m_Width;

  auto bitmapManagerPtr = std::make_unique<ProjectModel::BitmapManager>();

  auto parseStack = std::stack<QStandardItem*>();
  auto psTreeManager = std::make_unique<ProjectModel::TreeItemModel>();
  auto controllerTreeManger = std::make_unique<ProjectModel::TreeItemModel>();

  parseStack.push(psTreeManager->invisibleRootItem());

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
        int id = IdAllocation::getInstance().allocate();
        auto item = new ProjectModel::PsGroupLayer(id);

        item->setIcon(QIcon(":/icon/folder.png"));
        item->setText(QString::fromStdString(record.m_LayerName.getString()));
        item->setVisible(!record.m_BitFlags.m_isHidden);

        parseStack.top()->appendRow(item);
        parseStack.push(item);

      } else {
        auto* bitmap = createBitmap(record, image);
        bitmapManagerPtr->addAsset(bitmap);

        auto builder = BitmapLayerBuilder();
        builder.setIcon(bitmap);
        builder.setText(record.m_LayerName.getString());
        builder.setVisible(!record.m_BitFlags.m_isHidden);
        builder.allocationId();

        parseStack.top()->appendRow(builder.build().release());
        controllerTreeManger->appendRow(builder.build().release());
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
