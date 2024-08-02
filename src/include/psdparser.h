//
// Created by chen_yichen on 2024/7/30.
//

#pragma once
#include <QObject>
#include <QString>

#include "model/layer_bitmap.h"
#include "model/tree_manager.h"
namespace Parser {
class PsdParser : public QObject {
  Q_OBJECT
 private:
  QString path;
  int parseHeight = -1;
  int parseWidth = -1;
  std::unique_ptr<ProjectModel::TreeItemModel> _psTree{nullptr};
  std::unique_ptr<ProjectModel::TreeItemModel> _controllerTree{nullptr};
  std::unique_ptr<ProjectModel::BitmapManager> _bitmapManager{nullptr};

 public:
  explicit PsdParser(const QString& path);
  void Parse();
  ~PsdParser() override;

  inline int height() const { return parseHeight; }
  inline int width() const { return parseWidth; }
  inline ProjectModel::TreeItemModel* extractPsTree() { return _psTree.release(); }
  inline ProjectModel::TreeItemModel* extractControllerTree() {
    return _controllerTree.release();
  }
  inline ProjectModel::BitmapManager* extractBitmapManager() {
    return _bitmapManager.release();
  }
};

}  // namespace Parser
