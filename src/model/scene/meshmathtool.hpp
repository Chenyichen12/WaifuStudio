#pragma once

#include <QRectF>
#include <vector>
#include <array>

namespace Scene {

template <typename T>
concept HasXY = requires(T t1, T t2) {
  { t1.x() } -> std::convertible_to<double>;
  { t1.y() } -> std::convertible_to<double>;
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

  static std::tuple<double, double, double> barycentricCoordinates(
      const Point& p, const Point& a, const Point& b, const Point& c) {
    double areaABC = cross(a, b, a, c);
    double areaPBC = cross(p, b, p, c);
    double areaPCA = cross(p, c, p, a);
    double areaPAB = cross(p, a, p, b);

    double lambda1 = areaPBC / areaABC;
    double lambda2 = areaPCA / areaABC;
    double lambda3 = areaPAB / areaABC;

    return std::make_tuple(lambda1, lambda2, lambda3);
  }

  static Point fromBarycentricCoordinates(
      const std::tuple<double, double, double>& lambdas, const Point& a,
      const Point& b, const Point& c) {
    double lambda1 = std::get<0>(lambdas);
    double lambda2 = std::get<1>(lambdas);
    double lambda3 = std::get<2>(lambdas);

    double x = lambda1 * a.x() + lambda2 * b.x() + lambda3 * c.x();
    double y = lambda1 * a.y() + lambda2 * b.y() + lambda3 * c.y();

    return Point(x, y);
  }
  /**
   * use to calculate Graphics item bound
   * @param vector scene position
   * @return bound
   */
  static QRectF calculateBoundRect(const std::vector<Point>& vector) {
    float left = vector[0].x();
    float right = vector[0].x();
    float top = vector[0].y();
    float button = vector[0].y();
    for (const auto& item : vector) {
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

  /**
   * use to calculate Item bound
   * @param vector scene position
   * @return the index of the vector
   * top bottom left right
   */
  static std::array<int,4> calculateBoundIndex(const std::vector<Point>& vector) {
    float left = vector[0].x();
    float right = vector[0].x();
    float top = vector[0].y();
    float button = vector[0].y();

    int leftIndex = 0;
    int rightIndex = 0;
    int topIndex = 0;
    int bottomIndex = 0;

    for (int i = 0; i < vector.size();++i) {
      auto&& item = vector[i];
      if (item.x() < left) {
        left = item.x();
        leftIndex = i;
      }
      if (item.x() > right) {
        right = item.x();
        rightIndex = i;
      }
      if (item.y() < top) {
        top = item.y();
        topIndex = i;
      }
      if (item.y() > button) {
        button = item.y();
        bottomIndex = i;
      }
    }
    return {topIndex, bottomIndex, leftIndex, rightIndex};
  }
};

}  // namespace Scene