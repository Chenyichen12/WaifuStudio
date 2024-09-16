#include "projectservice.h"

#include <QGraphicsScene>
#include <QItemSelectionModel>
#include <QStack>

#include "parser/psdparser.h"
#include "tree/layer.h"
#include "tree/layermodel.h"
namespace WaifuL2d {

class PsdLayerSimpleFactory {
 public:
  static Layer* createLayer(const TreeNode* node,
                            const QHash<int, MeshNode> bitmapCache) {
    switch (node->type) {
      // just save the image bitmap in psd parser
      case 1: {
        auto image = bitmapCache[node->id].texture;
        return new BitmapLayer(node->name, image);
      }
      default: {
        return nullptr;
      }
    }
  }
};

struct Project {
  LayerModel* model = nullptr;
  QItemSelectionModel* selectionModel = nullptr;
  QGraphicsScene* scene = nullptr;
  ~Project() {
    delete model;
    delete scene;
  }
};
void ProjectService::finizateProject(Project* project) {}
ProjectService::ProjectService(QObject* parent) : QObject(parent) {}

ProjectService::~ProjectService() = default;

int ProjectService::initProjectFromPsd(const QString& path) {
  auto parser = PsdParser(path.toStdString());
  parser.parse();
  if (!parser.getResult()->success) {
    return -1;
  }
  const auto& result = parser.getResult();
  auto proj = std::make_unique<Project>();
  proj->model = new LayerModel(this);
  proj->selectionModel = new QItemSelectionModel(proj->model);

  auto parseStack = QStack<TreeNode*>();
  parseStack.push(result->root);
  while (!parseStack.empty()) {
    auto current = parseStack.pop();
    auto l = PsdLayerSimpleFactory::createLayer(current, result->meshNode);
    if (l != nullptr) {
      proj->model->appendRow(l);
    }
    for (const auto& child : current->children) {
      parseStack.push(child);
    }
  }

  // create scene
  proj->scene = new QGraphicsScene();

  this->project = std::move(proj);
  finizateProject(project.get());
  emit projectChanged();
  return 0;
}
QStandardItemModel* ProjectService::getLayerModel() const {
  return project->model;
}

QItemSelectionModel* ProjectService::getLayerSelectionModel() const {
  return project->selectionModel;
}

QGraphicsScene* ProjectService::getScene() const { return project->scene; }

void ProjectService::setLayerLock(const QModelIndex& index, bool lock) {
  if (project == nullptr) {
    return;
  }
  auto layer = project->model->layerFromIndex(index);
  if (layer == nullptr) {
    return;
  }
  layer->setLocked(lock);

  //TODO: undo command
}

void ProjectService::setLayerVisible(const QModelIndex& index, bool visible) {
  if (project == nullptr) {
    return;
  }
  auto layer = project->model->layerFromIndex(index);
  if (layer == nullptr) {
    return;
  }

  if(layer->getLocked()){
    return;
  }
  layer->setVisible(visible);

  //TODO: undo command
}
}  // namespace WaifuL2d