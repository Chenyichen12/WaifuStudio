#pragma once

#include <QRectF>
#include <vector>

namespace WaifuL2d {
template <typename T>
concept HasXY = requires(T t1, T t2)
{
  { t1.x() } -> std::convertible_to<double>;
  { t1.y() } -> std::convertible_to<double>;
};

template <typename T>
concept HasUV = requires(T t1, T t2)
{
  { t1.u() } -> std::convertible_to<double>;
  { t1.v() } -> std::convertible_to<double>;
};

template <HasXY Point>
class MeshMathTool {
public:
  static double cross(double x1, double y1, double x2, double y2) {
    return x1 * y2 - x2 * y1;
  }

  static double cross(const Point& aVec, const Point& bVec) {
    return cross(aVec.x(), aVec.y(), bVec.x(), bVec.y());
  }

  static double cross(const Point& a, const Point& p1, const Point& b,
                      const Point& p2) {
    return cross(a.x() - p1.x(), a.y() - p1.y(), b.x() - p2.x(),
                 b.y() - p2.y());
  }

  static bool isInTriangle(const Point& point, const Point& a, const Point& b,
                           const Point& c) {
    auto pab = cross(a, point, b, point);
    auto pbc = cross(b, point, c, point);
    auto pca = cross(c, point, a, point);

    if (pab * pbc < 0 || pab * pca < 0 || pbc * pca < 0) {
      return false;
    }
    return true;
  }

  static std::array<double, 3> barycentricCoordinates(const Point& p,
    const Point& a,
    const Point& b,
    const Point& c) {
    double areaABC = cross(a, b, a, c);
    double areaPBC = cross(p, b, p, c);
    double areaPCA = cross(p, c, p, a);
    double areaPAB = cross(p, a, p, b);

    double lambda1 = areaPBC / areaABC;
    double lambda2 = areaPCA / areaABC;
    double lambda3 = areaPAB / areaABC;

    return {lambda1, lambda2, lambda3};
  }

  static std::pair<double, double> fromBarycentricCoordinates(
      const std::array<double, 3>& lambdas, const Point& a, const Point& b,
      const Point& c) {
    double lambda1 = lambdas[0];
    double lambda2 = lambdas[1];
    double lambda3 = lambdas[2];

    double x = lambda1 * a.x() + lambda2 * b.x() + lambda3 * c.x();
    double y = lambda1 * a.y() + lambda2 * b.y() + lambda3 * c.y();

    return std::make_pair(x, y);
  }

  static std::pair<double, double> fromBarycentricCoordinates(
      const std::array<double, 3>& lambdas, const Point& a, const Point& b,
      const Point& c)
    requires HasUV<Point> {
    double lambda1 = lambdas[0];
    double lambda2 = lambdas[1];
    double lambda3 = lambdas[2];

    double x = lambda1 * a.u() + lambda2 * b.u() + lambda3 * c.u();
    double y = lambda1 * a.v() + lambda2 * b.v() + lambda3 * c.v();

    return std::make_pair(x, y);
  }

  /**
   * use to calculate Graphics item bound
   * @param vector scene position
   * @return bound
   */
  static QRectF calculateBoundRect(const Point* vector, size_t size) {
    float left = vector[0].x();
    float right = vector[0].x();
    float top = vector[0].y();
    float button = vector[0].y();
    for (size_t i = 0; i < size; i++) {
      const Point& item = vector[i];
      if (item.x() < left) {
        left = item.x();
      }
      if (item.x() > right) {
        right = item.x();
      }
      if (item.y() < top) {
        top = item.y();
      }
      if (item.y() > button) {
        button = item.y();
      }
    }
    return QRectF(QPointF(left, top), QPointF(right, button));
  }

  static void resizePointInBound(const QRectF& startBound,
                                 const QRectF& resizeBound,
                                 Point* vector,
                                 size_t size,
                                 bool isXFlip = false,
                                 bool isYFlip = false)
    requires requires(Point p)
    {
      { p.setX(0.0) };
      { p.setY(0.0) };
    } {
    for (size_t i = 0; i < size; i++) {
      auto& item = vector[i];
      auto u = (item.x() - startBound.left()) / startBound.width();
      auto v = (item.y() - startBound.top()) / startBound.height();
      if (isXFlip) {
        u = 1 - u;
      }
      if (isYFlip) {
        v = 1 - v;
      }
      item.setX(resizeBound.left() + u * resizeBound.width());
      item.setY(resizeBound.top() + v * resizeBound.height());
    }
  }

  static void rotatePoints(double angle, Point* points, size_t size) {
    auto center = calculateBoundRect(points, size).center();
    for (int i = 0; i < size; i++) {
      auto& point = points[i];
      auto x = point.x();
      auto y = point.y();
      point.setX(center.x() + (x - center.x()) * cos(angle) -
                 (y - center.y()) * sin(angle));
      point.setY(center.y() + (x - center.x()) * sin(angle) +
                 (y - center.y()) * cos(angle));
    }
  }
};
} // namespace WaifuL2d