#include "projectservice.h"

#include <QGraphicsScene>
#include <QStack>
#include <QUndoStack>

#include "layerselectionmodel.h"
#include "model/scene/deformer/meshdeformer.h"
#include "parser/psdparser.h"
#include "scene/deformmanager.h"
#include "scene/mainstagescene.h"
#include "scene/mesh/mesh.h"
#include "scene/mesh/rendergroup.h"
#include "tree/layer.h"
#include "tree/layermodel.h"
#include "undo/visiblecommand.h"
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

  static Mesh* createMesh(const TreeNode* node,
                          const QHash<int, MeshNode> bitmapCache) {
    switch (node->type) {
      // just save the image bitmap in psd parser
      case 1: {
        auto bitmap = bitmapCache[node->id];
        QList<MeshVertex> vertices;
        for (int i = 0; i < bitmap.scenePosition.size(); i++) {
          vertices.append({glm::vec2(bitmap.scenePosition[i].x(),
                                     bitmap.scenePosition[i].y()),
                           glm::vec2(bitmap.uvs[i].x(), bitmap.uvs[i].y())});
        }
        auto mesh = new Mesh(vertices, bitmap.incident);
        mesh->setTexture(bitmap.texture);
        return mesh;
      }
      default: {
        return nullptr;
      }
    }
  }
};

struct Project : public QObject {
  LayerModel* model = nullptr;
  LayerSelectionModel* selectionModel = nullptr;
  MainStageScene* scene = nullptr;
  QUndoStack* undoStack;

  Project() { undoStack = new QUndoStack(); }

  void setParentManager() {
    model->setParent(this);
    selectionModel->setModel(model);
    scene->setParent(this);
    undoStack->setParent(this);
  }
};
void ProjectService::finizateProject(Project* project) {
  project->setParentManager();
  connect(project->scene->getDeformManager(), &DeformManager::deformCommand,
          this,[this](QSharedPointer<DeformerCommand> command) {
            this->project->undoStack->push(command->createUndoCommand());
          });
}
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
  // create scene
  proj->scene = new MainStageScene();

  // create deformer manager
  auto deformManager = new DeformManager();
  deformManager->setSmallSize(result->width/200);
  auto renderGroup =
      new RenderGroup(QRectF(0, 0, result->width, result->height));
  // two main components
  proj->scene->setRenderGroup(renderGroup);
  proj->scene->setDeformManager(deformManager);

  proj->selectionModel = new LayerSelectionModel(proj->model,proj->scene->getDeformManager());
  auto parseStack = QStack<TreeNode*>();
  parseStack.push(result->root);
  while (!parseStack.empty()) {
    auto current = parseStack.pop();
    auto l = PsdLayerSimpleFactory::createLayer(current, result->meshNode);
    if (l != nullptr) {
      proj->model->appendRow(l);
      auto m = PsdLayerSimpleFactory::createMesh(current, result->meshNode);
      if (m != nullptr) {
        renderGroup->addMesh(m);
        auto deformer = new MeshDeformer(m);
        deformer->setBindLayer(l);
        deformManager->addDeformer(deformer);
      }
    }

    for (const auto& child : current->children) {
      parseStack.push(child);
    }
  }

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

void ProjectService::initGL() {
  if (project == nullptr) {
    return;
  }
  project->scene->getRenderGroup()->initGL();
}

void ProjectService::setLayerLock(const QModelIndex& index, bool lock) {
  if (project == nullptr) {
    return;
  }
  auto layer = project->model->layerFromIndex(index);
  if (layer == nullptr) {
    return;
  }
  layer->setLocked(lock);

  // TODO: undo command
}

void ProjectService::undo() {
  if (project == nullptr) {
    return;
  }
  project->undoStack->undo();
}

void ProjectService::redo() {
  if (project == nullptr) {
    return;
  }
  project->undoStack->redo();
}

void ProjectService::setLayerVisible(const QModelIndex& index, bool visible,
                                     bool isStart) {
  if (project == nullptr) {
    return;
  }
  // undo command
  auto command =
      new VisibleCommand(project->scene->getRenderGroup(), project->model,
                         project->scene->getDeformManager(), index, visible);
  command->setStart(isStart);
  project->undoStack->push(command);
}

}  // namespace WaifuL2d