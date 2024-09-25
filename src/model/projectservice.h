#pragma once
#include <QObject>
class QStandardItemModel;
class QItemSelectionModel;
class QGraphicsScene;

namespace WaifuL2d {
struct Project;
class SceneController;

class ProjectService : public QObject {
  Q_OBJECT

private:
  std::unique_ptr<Project> project = nullptr;
  SceneController* sceneController;

protected:
  virtual void finizateProject(Project* project);

public:
  ProjectService(QObject* parent = nullptr);
  ~ProjectService() override;

  /**
   * @return 0 if success, otherwise return error code
   */
  int initProjectFromPsd(const QString& path);

  QStandardItemModel* getLayerModel() const;
  QItemSelectionModel* getLayerSelectionModel() const;
  QGraphicsScene* getScene() const;
  SceneController* getSceneController() const;

  void initGL();

public slots:
  void setLayerVisible(const QModelIndex& index, bool visible, bool isStart);
  void setLayerLock(const QModelIndex& index, bool lock);

  void undo();
  void redo();
signals:
  void projectChanged();
};
} // namespace WaifuL2d