#pragma once
#include <QObject>
class QUndoStack;
class QStandardItemModel;
class QItemSelectionModel;
class QGraphicsScene;
class QUndoGroup;

namespace WaifuL2d {
class LayerModel;
class LayerSelectionModel;
class MainStageScene;
struct Project : public QObject {
  LayerModel* model = nullptr;
  LayerSelectionModel* selectionModel = nullptr;
  MainStageScene* scene = nullptr;

  void setParentManager();
};

class SceneController;

#ifdef QT_DEBUG
class ProjectServiceTest;
#endif

class ProjectService : public QObject {
#ifdef QT_DEBUG
  friend ProjectServiceTest;
#endif
  Q_OBJECT
 private:
  std::unique_ptr<Project> project;  // will be nullptr if no project
  SceneController* sceneController;

  QUndoStack* mainUndoStack;
  QUndoGroup* undoGroup;

 protected:
  virtual void finizateProject(Project* project);

 public:
  explicit ProjectService(QObject* parent = nullptr);
  ~ProjectService() override;

  /**
   * @return 0 if success, otherwise return error code
   */
  int initProjectFromPsd(const QString& path);

  QStandardItemModel* getLayerModel() const;
  QItemSelectionModel* getLayerSelectionModel() const;
  QGraphicsScene* getScene() const;
  SceneController* getSceneController() const;

 public slots:
  void setLayerVisible(const QModelIndex& index, bool visible, bool isStart);
  void setLayerLock(const QModelIndex& index, bool lock);

  void undo();
  void redo();
 signals:
  void projectChanged();
};
}  // namespace WaifuL2d