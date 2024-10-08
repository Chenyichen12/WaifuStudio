#include "projectservice.h"

#include <QStack>
#include <QUndoGroup>
#include <QUndoStack>

#include "controller/scenecontroller.h"
#include "layerselectionmodel.h"
#include "model/scene/deformer/meshdeformer.h"
#include "parser/psdparser.h"
#include "scene/deformercommand.h"
#include "scene/mainstagescene.h"
#include "scene/mesh/mesh.h"
#include "scene/mesh/rendergroup.h"
#include "tree/layer.h"
#include "tree/layermodel.h"
#include "undo/editfinishcommand.h"
#include "undo/lockcommand.h"
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
void Project::setParentManager() {
  model->setParent(this);
  selectionModel->setModel(model);
  scene->setParent(this);
}

void ProjectService::finizateProject(Project* project) {
  Q_ASSERT(project->model != nullptr);
  Q_ASSERT(project->selectionModel != nullptr);
  Q_ASSERT(project->scene != nullptr);

  project->setParentManager();

  // clear undo
  mainUndoStack->clear();
  sceneController->clearUndo();
  undoGroup->setActiveStack(mainUndoStack);

  // controller
  this->sceneController->setScene(project->scene);
  connect(project->scene, &MainStageScene::deformerCommand, this,
          [this](std::shared_ptr<DeformerCommand> command) {
            // TODO: some new things can be done here
            auto com = command->createUndoCommand();
            this->mainUndoStack->push(com);
          });

  connect(sceneController, &SceneController::editModeChange,
          project->selectionModel, [this](bool isEdit) {
            if (isEdit) {
              this->project->selectionModel->clear();
            }

            this->project->selectionModel->setEnable(!isEdit);
          });
  connect(sceneController, &SceneController::editFinishCommand, project,
          [this](std::shared_ptr<EditFinishCommandWrapper> w) {
            this->mainUndoStack->push(w->createCommand());
            auto id = w->getTargetDeformer()->getBindId();

            // may not enable after edit, set enable first
            this->project->selectionModel->setEnable(true);
            this->project->selectionModel->selectById({id});
          });
}

ProjectService::ProjectService(QObject* parent) : QObject(parent) {
  this->project = nullptr;
  this->sceneController = new SceneController(this);
  undoGroup = new QUndoGroup(this);
  mainUndoStack = new QUndoStack(undoGroup);
  undoGroup->addStack(mainUndoStack);
  undoGroup->addStack(sceneController->getEditModeUndoStack());
  undoGroup->setActiveStack(mainUndoStack);

  // change the undo stack when enter into the edit mode
  // may move to another function but lambda is more clear now
  connect(sceneController, &SceneController::editModeChange, this,
          [this](bool isEdit) {
            if (isEdit) {
              this->undoGroup->setActiveStack(
                  this->sceneController->getEditModeUndoStack());
            } else {
              this->undoGroup->setActiveStack(this->mainUndoStack);
            }
          });
}

ProjectService::~ProjectService() {};

int ProjectService::initProjectFromPsd(const QString& path) {
  auto parser = PsdParser(path.toStdWString());
  parser.parse();
  if (!parser.getResult()->success) {
    return -1;
  }
  const auto& result = parser.getResult();
  auto proj = std::make_unique<Project>();
  proj->model = new LayerModel(this);
  // create scene
  proj->scene = new MainStageScene();

  proj->selectionModel = new LayerSelectionModel(proj->model, proj->scene);
  auto renderGroup =
      new RenderGroup(QRectF(0, 0, result->width, result->height));
  // two main components
  proj->scene->setRenderGroup(renderGroup);
  auto parseStack = QStack<TreeNode*>();
  parseStack.push(result->root);
  while (!parseStack.empty()) {
    auto current = parseStack.pop();
    auto l = PsdLayerSimpleFactory::createLayer(current, result->meshNode);

    if (l != nullptr) {
      proj->model->addLayer(l);
      auto m = PsdLayerSimpleFactory::createMesh(current, result->meshNode);
      if (m != nullptr) {
        renderGroup->addMesh(m);
        auto deformer = new MeshDeformer(m);
        deformer->setBindId(l->getId());
        proj->scene->addDeformer(deformer);
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

void ProjectService::setLayerLock(const QModelIndex& index, bool lock) {
  if (project == nullptr) {
    return;
  }
  auto layer = project->model->layerFromIndex(index);
  if (layer == nullptr) {
    return;
  }

  LockCommand* command = new LockCommand(project->model, layer->getId());
  command->setShouldLock(lock);
  mainUndoStack->push(command);
}

void ProjectService::undo() { undoGroup->undo(); }

void ProjectService::redo() { undoGroup->redo(); }

void ProjectService::setLayerVisible(const QModelIndex& index, bool visible,
                                     bool isStart) {
  if (project == nullptr) {
    return;
  }
  // undo command
  auto command =
      new VisibleCommand(project->model, project->scene, index, visible);
  command->setStart(isStart);
  mainUndoStack->push(command);
}

SceneController* ProjectService::getSceneController() const {
  return sceneController;
}
}  // namespace WaifuL2d