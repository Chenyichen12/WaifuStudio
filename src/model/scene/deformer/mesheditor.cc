#include "mesheditor.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QUndoCommand>

#include "operatepoint.h"

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
      editor->setPoints(data.oldPoints);
    }

    void redo() override {
      editor->setPoints(data.newPoints);
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
  penSurface->setRect(rect);
  prepareGeometryChange();
}

QList<QPointF> MeshEditor::getPoints() const {
  QList<QPointF> result;
  for (const auto& p : points) {
    result.push_back(p->pos());
  }
  return result;
}

QRectF MeshEditor::boundingRect() const { return penSurface->rect(); }

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
  updatePoints(scenePoints);
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

void MeshEditor::updatePoints(const QList<QPointF>& pointPositions) {
  if (points.size() == pointPositions.size()) {
    for (int i = 0; i < pointPositions.size(); i++) {
      points[i]->setPos(pointPositions[i]);
    }
    return;
  }

  if (points.size() < pointPositions.size()) {
    for (size_t i = 0; i < points.size(); i++) {
      points[i]->setPos(pointPositions[i]);
    }

    for (size_t i = points.size(); i < pointPositions.size(); i++) {
      auto* point = createPoint(pointPositions[i]);
      points.push_back(point);
      point->data = i;
    }
    return;
  }

  if (points.size() > pointPositions.size()) {
    for (size_t i = 0; i < pointPositions.size(); i++) {
      points[i]->setPos(pointPositions[i]);
    }

    for (size_t i = pointPositions.size(); i < points.size(); i++) {
      delete points[i];
    }
    points.resize(pointPositions.size());
    return;
  }
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
  //  updatePoints(points);
}

MeshEditor::MeshEditor(const QList<QPointF>& initPoints,
                       const QList<unsigned int>& initIncident,
                       QGraphicsItem* parent)
  : QGraphicsObject(parent) {
  updatePoints(initPoints);
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
  penSurface = pen;
}
} // namespace WaifuL2d