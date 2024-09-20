#pragma once
#include <QHash>
#include <QImage>
#include <QList>
#include <QPointF>
#include <QString>
namespace WaifuL2d {

struct TreeNode {
  int id;

  QString name;
  TreeNode* parent = nullptr;
  QList<TreeNode*> children;
  int type = -1;
  bool visible = true;
  bool locked = false;

  ~TreeNode();
};

struct MeshNode {
  int bindTreeid;
  QImage texture;
  QList<QPointF> scenePosition;
  QList<QPointF> uvs;
  QList<unsigned int> incident;
};

struct ParseResult {
  bool success;

  unsigned int height;
  unsigned int width;
  TreeNode* root = nullptr;
  QHash<int, MeshNode> meshNode;
  ParseResult() : success(false), root(nullptr) {}
  ~ParseResult();
};
class IParser {
 public:
  virtual void parse() = 0;
  /**
   * temp result, the result will be invalid after the next parse
   */
  virtual ParseResult* getResult() = 0;
};
}  // namespace WaifuL2d