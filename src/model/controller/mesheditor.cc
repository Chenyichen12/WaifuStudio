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
      for (const auto& point : points) {
        if (point->isHitPoint(event->scenePos())) {
          event->ignore();
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

  PenSurface(QGraphicsItem* parent = nullptr) : QGraphicsRectItem(parent) {
  }
};

class MeshPointMoveCommand : public WaifuL2d::MeshEditorCommand {
public:
  struct MoveData {
    QList<QPointF> oldPoints;
    QList<QPointF> newPoints;
    bool isStart;
  } data;

private:
  class UndoCommand : public QUndoCommand {
    MoveData data;
    WaifuL2d::MeshEditor* editor;

  public:
    explicit UndoCommand(MoveData data, WaifuL2d::MeshEditor* editor,
                         QUndoCommand* parent)
      : QUndoCommand(parent), data(std::move(data)), editor(editor) {
    }

    void undo() override {
      editor->updatePointsPos(data.oldPoints);
    }

    void redo() override {
      editor->updatePointsPos(data.newPoints);
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
  pen.setColor(Qt::gray);

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
  // updatePoints(scenePoints);
  if (scenePoints.size() == points.size()) {
    updatePointsPos(scenePoints);
  }
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
  auto points = this->getPoints();
  points.push_back(pos);

  qDebug() << "add point at" << pos;
  // TODO: add undo command
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
  pen->getExistPoints = [this]() {
    return this->points;
  };
  pen->setVisible(true);

  toolSurface[0] = new QGraphicsRectItem(this);
  toolSurface[1] = pen;
  toolSurface[2] = new QGraphicsRectItem(this);

  setEditTool(EditToolType::Cursor);
}

void MeshEditor::removePoint(int index) {
  Q_ASSERT(index >= 0 && index < points.size());
  auto point = points[index];
  points.removeAt(index);
  delete point;
  for (int i = index; i < points.size(); i++) {
    points[i]->data = i;
  }
}

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