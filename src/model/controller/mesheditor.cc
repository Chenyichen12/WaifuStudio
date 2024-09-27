#include "mesheditor.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QUndoCommand>

#include "model/scene/deformer/operatepoint.h"

namespace {
class PenSurface : public QGraphicsRectItem {
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override {
    if (this->getExistPoints) {
      auto points = this->getExistPoints();
      for (int i = 0; i < points.size(); i++) {
        if (points[i]->isHitPoint(event->scenePos())) {
          if (this->penHitPoint) {
            this->penHitPoint(i);
            event->accept();
          } else {
            event->ignore();
          }
          return;
        }
      }
    }

    event->accept();
    if (shouldAddPoint) {
      shouldAddPoint(event->scenePos());
    }
  }

public:
  std::function<QList<WaifuL2d::OperatePoint*>()> getExistPoints = nullptr;
  std::function<void(const QPointF&)> shouldAddPoint = nullptr;
  std::function<void(int index)> penHitPoint = nullptr;

  PenSurface(QGraphicsItem* parent = nullptr) : QGraphicsRectItem(parent) {
  }
};

class MeshPointMoveCommand : public WaifuL2d::MeshEditorCommand {
public:
  struct MoveData {
    QList<QPointF> oldPoints;
    QList<QPointF> newPoints;
    bool isStart;

    /**
     * for this undo command, it will call update cdt
     * after update the cdt, the fixed edge and all edge may be changed
     * So it is necessary to record the undo command old edge information
     * the following command is the same
     * the move command may not need it. But it seems work well here. keep it until it performs bad
     */
    CDT::EdgeUSet oldFixedEdge = {};
    CDT::EdgeUSet oldAllEdge = {};
  } data;

private:
  class UndoCommand : public QUndoCommand {
    MoveData data;
    WaifuL2d::MeshEditor* editor;

  public:
    explicit UndoCommand(MoveData data, WaifuL2d::MeshEditor* editor,
                         QUndoCommand* parent)
      : QUndoCommand(parent), data(std::move(data)), editor(editor) {
      if (this->data.isStart) {
        this->data.oldFixedEdge = editor->getFixedEdges();
        this->data.oldAllEdge = editor->getAllEdges();
      }
    }

    void undo() override {
      editor->updatePointsPos(data.oldPoints);
      editor->setEdges(data.oldFixedEdge, data.oldAllEdge);
    }

    void redo() override {
      editor->updatePointsPos(data.newPoints);
      editor->upDateCDT();
    }

    int id() const override { return 100; }

    bool mergeWith(const QUndoCommand* other) override {
      if (id() != other->id()) {
        return false;
      }

      auto otherCommand =
          static_cast<const MeshPointMoveCommand::UndoCommand*>(other);
      if (otherCommand->data.isStart) {
        return false;
      }
      return true;
    }
  };

  WaifuL2d::MeshEditor* editor;

public:
  QUndoCommand* createUndoCommand(QUndoCommand* parent) override {
    return new UndoCommand(data, editor, parent);
  }

  MeshPointMoveCommand(WaifuL2d::MeshEditor* editor, MoveData data)
    : data(std::move(data)), editor(editor) {
  }
};

class MeshPointAddCommand : public WaifuL2d::MeshEditorCommand {
public:
  struct AddData {
    QPointF pos;
    int addIndex;
    std::optional<int> connectIndex;
    CDT::EdgeUSet oldFixedEdge = {};
    CDT::EdgeUSet oldAllEdge = {};
  } data;

  WaifuL2d::MeshEditor* editor;

private:
  class UndoCommand : public QUndoCommand {
    AddData data;
    WaifuL2d::MeshEditor* editor;

  public:
    UndoCommand(const AddData& data, WaifuL2d::MeshEditor* editor,
                QUndoCommand* parent)
      : QUndoCommand(parent), data(data), editor(editor) {
      this->data.oldFixedEdge = editor->getFixedEdges();
      this->data.oldAllEdge = editor->getAllEdges();
    }

    void redo() override {
      editor->addPoint(data.pos, data.addIndex);
      if (data.connectIndex.has_value()) {
        editor->connectFixEdge(data.addIndex, data.connectIndex.value());
      }
      editor->selectPoints({data.addIndex});
      editor->upDateCDT();
    }

    void undo() override {
      editor->removePoint(data.addIndex);
      // restore it
      editor->setEdges(data.oldFixedEdge, data.oldAllEdge);
    }
  };

public:
  QUndoCommand* createUndoCommand(QUndoCommand* parent) override {
    return new UndoCommand(data, editor, parent);
  }

  explicit MeshPointAddCommand(WaifuL2d::MeshEditor* editor,
                               const AddData& data)
    : data(data), editor(editor) {
  }
};


class FixedEdgeConnectCommand : public WaifuL2d::MeshEditorCommand {
public:
  struct ConnectData {
    int index1;
    int index2;
    CDT::EdgeUSet oldFixedEdge = {};
    CDT::EdgeUSet oldAllEdge = {};
  } data;

  WaifuL2d::MeshEditor* editor;

private:
  class UndoCommand : public QUndoCommand {
    ConnectData data;
    WaifuL2d::MeshEditor* editor;

  public:
    UndoCommand(const ConnectData& data, WaifuL2d::MeshEditor* editor,
                QUndoCommand* parent)
      : QUndoCommand(parent), data(data), editor(editor) {
      this->data.oldFixedEdge = editor->getFixedEdges();
      this->data.oldAllEdge = editor->getAllEdges();
    }

    void redo() override {
      editor->connectFixEdge(data.index1, data.index2);
      editor->upDateCDT();
    }

    void undo() override {
      editor->disconnectFixEdge(data.index1, data.index2);
      editor->setEdges(data.oldFixedEdge, data.oldAllEdge);
    }
  };

public:
  QUndoCommand* createUndoCommand(QUndoCommand* parent) override {
    return new UndoCommand(data, editor, parent);
  }

  FixedEdgeConnectCommand(WaifuL2d::MeshEditor* editor, const ConnectData& data)
    : data(data), editor(editor) {
  }
};
} // namespace

namespace WaifuL2d {
MeshEditor::MeshEditor(QGraphicsItem* parent) : MeshEditor({}, {}, parent) {
}

void MeshEditor::setHandleRect(const QRectF& rect) {
  for (auto& surface : toolSurface) {
    surface->setRect(rect);
  }
  prepareGeometryChange();
}

QList<QPointF> MeshEditor::getPoints() const {
  QList<QPointF> result;
  for (const auto& p : points) {
    result.push_back(p->pos());
  }
  return result;
}

QRectF MeshEditor::boundingRect() const { return toolSurface[0]->rect(); }

void MeshEditor::paint(QPainter* painter,
                       const QStyleOptionGraphicsItem* option,
                       QWidget* widget) {
  auto pen = QPen();
  pen.setWidth(1 / painter->transform().m11());
  pen.setColor(Qt::white);

  painter->setPen(pen);
  for (const auto& edge : allEdges) {
    painter->drawLine(points[edge.v1()]->pos(), points[edge.v2()]->pos());
  }

  pen.setColor(Qt::black);
  painter->setPen(pen);

  for (const auto& edge : fixedEdges) {
    painter->drawLine(points[edge.v1()]->pos(), points[edge.v2()]->pos());
  }
}

void MeshEditor::setPoints(const QList<QPointF>& scenePoints) {
  if (scenePoints.size() == points.size()) {
    updatePointsPos(scenePoints);
  }
  //TODO: not complete
}

void MeshEditor::selectPoints(const QList<int>& indexes) {
  for (int i = 0; i < points.size(); i++) {
    points[i]->setSelected(indexes.contains(i));
  }
  update();
}

void MeshEditor::upDateCDT() {
  const auto& cdt = calculateCDT();
  this->fixedEdges = cdt.fixedEdges;
  this->allEdges = CDT::extractEdgesFromTriangles(cdt.triangles);
  this->update();
}

CDT::EdgeUSet MeshEditor::getFixedEdges() const { return fixedEdges; }

CDT::EdgeUSet MeshEditor::getAllEdges() const { return allEdges; }

void MeshEditor::setEdges(const CDT::EdgeUSet& fixedEdges,
                          const CDT::EdgeUSet& allEdges) {
  this->fixedEdges = fixedEdges;
  this->allEdges = allEdges;
  update();
}


void MeshEditor::setEditTool(EditToolType type) {
  auto index = static_cast<int>(type);
  for (int i = 0; i < toolSurface.size(); i++) {
    toolSurface[i]->setVisible(i == index);
  }
  update();
}

void MeshEditor::addPoint(const QPointF& point) {
  auto* operatePoint = createPoint(point);
  points.push_back(operatePoint);
  operatePoint->data = points.size() - 1;
  update();
}

void MeshEditor::addPoint(const QPointF& point, int index) {
  auto* operatePoint = createPoint(point);
  points.insert(index, operatePoint);
  for (int i = index; i < points.size(); i++) {
    points[i]->data = i;
  }
  update();
}

void MeshEditor::connectFixEdge(unsigned int index1, unsigned int index2) {
  Q_ASSERT(index1 < points.size());
  Q_ASSERT(index2 < points.size());
  fixedEdges.insert({index1, index2});
  allEdges.insert({index1, index2});
  update();
}

void MeshEditor::disconnectFixEdge(unsigned int index1, unsigned int index2) {
  Q_ASSERT(index1 < points.size());
  Q_ASSERT(index2 < points.size());
  fixedEdges.erase({index1, index2});
  allEdges.erase({index1, index2});
  update();
}


OperatePoint* MeshEditor::createPoint(const QPointF& pos) {
  auto* point = new OperatePoint(this);
  point->setRadius(4);
  point->setPos(pos);
  point->pointShouldMove = [this](const QPointF& pos, bool isStart,
                                  const QVariant& data) {
    handlePointShouldMove(pos, isStart, data);
  };
  return point;
}


void MeshEditor::handlePointShouldMove(const QPointF& pos, bool isStart,
                                       const QVariant& data) {
  auto index = data.toInt();
  auto oldPoints = getPoints();
  auto newPoints = oldPoints;
  newPoints[index] = pos;

  auto command = std::make_shared<MeshPointMoveCommand>(
      this, std::move(MeshPointMoveCommand::MoveData{
          oldPoints, newPoints, isStart
      }));
  emit editorCommand(command);
}

void MeshEditor::handleShouldAddPoint(const QPointF& pos) {
  auto selectPoint = getSelectedPoint();
  std::optional<int> connectIndex;
  if (selectPoint.size() == 1) {
    connectIndex = selectPoint[0]->data.toInt();
  }
  auto addData = MeshPointAddCommand::AddData{
      pos, static_cast<int>(points.size()), connectIndex};

  auto command = std::make_shared<MeshPointAddCommand>(this, addData);
  emit editorCommand(command);
}

void MeshEditor::handleShouldConnect(int index1, int index2) {
  if (index1 == index2) {
    return;
  }

  auto edge = CDT::Edge{static_cast<unsigned int>(index1),
                        static_cast<unsigned int>(index2)};
  if (fixedEdges.contains(edge)) {
    return;
  }

  auto command = std::make_shared<FixedEdgeConnectCommand>(
      this, FixedEdgeConnectCommand::ConnectData{index1, index2});
  emit editorCommand(command);
}

CDT::Triangulation<double> MeshEditor::calculateCDT() const {
  CDT::Triangulation<double> cdt;
  std::vector<CDT::V2d<double>> editPoint;
  for (const auto& p : this->points) {
    editPoint.push_back({p->pos().x(), p->pos().y()});
  }
  cdt.insertVertices(editPoint);
  CDT::EdgeVec vec;
  for (const auto& p : fixedEdges) {
    vec.push_back(p);
  }
  // may throw the not allowable error
  // the cdt will auto resolve the error
  // and make the mesh allowable
  try {
    cdt.insertEdges(vec);
  } catch (...) {
    // TODO: remind error
    qDebug() << "cdt error";
  }

  cdt.eraseOuterTriangles();
  return cdt;
}

QList<OperatePoint*> MeshEditor::getSelectedPoint() const {
  QList<OperatePoint*> result;
  for (int i = 0; i < points.size(); i++) {
    if (points[i]->isSelected()) {
      result.push_back(points[i]);
    }
  }
  return result;
}

MeshEditor::MeshEditor(const QList<QPointF>& initPoints,
                       const QList<unsigned int>& initIncident,
                       QGraphicsItem* parent)
  : QGraphicsObject(parent) {
  for (const auto& point : initPoints) {
    addPoint(point);
  }

  for (int i = 0; i < initIncident.size(); i += 3) {
    CDT::Edge e1 = {initIncident[i], initIncident[i + 1]};
    CDT::Edge e2 = {initIncident[i + 1], initIncident[i + 2]};
    CDT::Edge e3 = {initIncident[i + 2], initIncident[i]};

    fixedEdges.insert(e1);
    fixedEdges.insert(e2);
    fixedEdges.insert(e3);
  }
  this->allEdges = fixedEdges;

  auto pen = new PenSurface(this);
  pen->shouldAddPoint = [this](const auto& point) {
    this->handleShouldAddPoint(point);
  };
  pen->penHitPoint = [this](int index) {
    std::optional<int> index1;
    auto selectPoints = this->getSelectedPoint();
    if (selectPoints.size() == 1) {
      index1 = selectPoints[0]->data.toInt();
    }
    this->selectPoints({index});

    if (index1.has_value()) {
      this->handleShouldConnect(index1.value(), index);
    }
  };
  pen->getExistPoints = [this]() {
    return this->points;
  };

  toolSurface[0] = new QGraphicsRectItem(this);
  toolSurface[0]->setZValue(1);

  toolSurface[1] = pen;
  toolSurface[1]->setZValue(1);

  toolSurface[2] = new QGraphicsRectItem(this);
  toolSurface[2]->setZValue(1);

  setEditTool(EditToolType::Cursor);
}

void MeshEditor::removePoint(int index) {
  Q_ASSERT(index >= 0 && index < points.size());

  for (auto edge = fixedEdges.begin(); edge != fixedEdges.end();) {
    if (edge->v1() == index || edge->v2() == index) {
      edge = fixedEdges.erase(edge);
    } else {
      ++edge;
    }
  }
  for (auto edge = allEdges.begin(); edge != allEdges.end();) {
    if (edge->v1() == index || edge->v2() == index) {
      edge = allEdges.erase(edge);
    } else {
      ++edge;
    }
  }

  auto point = points[index];
  points.removeAt(index);
  delete point;
  for (int i = index; i < points.size(); i++) {
    points[i]->data = i;
  }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void MeshEditor::removePoints(const QList<int>& indexes) {
  for (auto index : indexes) {
    Q_ASSERT(index >= 0 && index < points.size());
  }
  // TODO: remove points
}

void MeshEditor::updatePointsPos(const QList<QPointF>& newPoints) {
  Q_ASSERT(newPoints.size() == points.size());
  for (size_t i = 0; i < newPoints.size(); i++) {
    points[i]->setPos(newPoints[i]);
  }
}
} // namespace WaifuL2d