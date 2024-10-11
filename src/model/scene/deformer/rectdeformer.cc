#include "rectdeformer.h"

#include <QPainter>

#include "model/scene/deformercommand.h"
#include "model/scene/mainstagescene.h"
#include "model/scene/meshmathtool.hpp"

namespace {
class RectOperterPoint : public WaifuL2d::OperatePoint {
 public:
  explicit RectOperterPoint(QGraphicsItem* parent = nullptr)
      : OperatePoint(parent) {}
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget = nullptr) override {
    Q_UNUSED(widget);
    Q_UNUSED(option);
    auto pen = QPen(Qt::black);
    pen.setWidthF(rect().width() / 10);
    painter->setPen(pen);
    if (this->isSelected()) {
      painter->setBrush(Qt::red);
    } else {
      painter->setBrush(Qt::white);
    }

    painter->drawRect(this->rect());
  }
};
}  // namespace

namespace WaifuL2d {

RectDeformer::RectDeformer(const QRectF& initRect, unsigned int row,
                           unsigned int column, QGraphicsItem* parent)
    : AbstractDeformer(parent), row_(row), column_(column) {
  for (int i = 0; i < row_ * column_; i++) {
    auto width = initRect.width() / column_;
    auto height = initRect.height() / row_;
    auto x = initRect.x() + width * static_cast<int>(i % column_);
    auto y = initRect.y() + height * static_cast<int>(i / column_);
    auto point = createOperatePoint(QPointF(x, y));
    point->data = i;
    operator_points_.push_back(point);
  }
}

OperatePoint* RectDeformer::createOperatePoint(const QPointF& p) {
  auto point = new RectOperterPoint(this);
  point->pointShouldMove = [this](const QPointF& point, bool isStart,
                                  const QVariant& data) {
    QList<QPointF> newPoints = this->getScenePoints();
    auto index = data.toInt();
    newPoints[index] = point;
    this->handlePointShouldMove(newPoints, isStart);
  };
  point->setPos(p);
  return point;
}
QList<QPointF> RectDeformer::getScenePoints() const {
  QList<QPointF> points;
  for (auto point : operator_points_) {
    points.push_back(point->scenePos());
  }
  return points;
}
void RectDeformer::setScenePoints(const QList<QPointF>& points) {
  Q_ASSERT(points.size() == operator_points_.size());
  for(int i = 0;  i < points.size(); i++) {
    operator_points_[i]->setPos(points[i]);
  }
  update();
}
QPointF RectDeformer::scenePointToLocal(const QPointF& point) const {
  // TODO: it must be implemented
  return point;
}
void RectDeformer::setDeformerSelect(bool select) {
  qDebug() << "set select" << select;
}
bool RectDeformer::isDeformerSelected() const { return true; }
QRectF RectDeformer::boundingRect() const {
  const auto& pos = getScenePoints();
  auto bound =
      MeshMathTool<QPointF>::calculateBoundRect(pos.data(), pos.size());
  return bound;
}
void RectDeformer::paint(QPainter* painter,
                         const QStyleOptionGraphicsItem* option,
                         QWidget* widget) {}

void RectDeformer::handlePointShouldMove(const QList<QPointF>& newPoints,
                                         bool isStart) {
  auto command = std::make_shared<DeformerCommand>();
  command->info.oldPoints = this->getScenePoints();
  command->info.newPoints = newPoints;

  command->info.isStart = isStart;
  command->info.target = this;
  auto sc = static_cast<MainStageScene*>(scene());
  sc->emitDeformerCommand(command);
}
}  // namespace WaifuL2d