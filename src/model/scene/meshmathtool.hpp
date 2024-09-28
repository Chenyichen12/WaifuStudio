#pragma once

#include <QRectF>
#include <vector>

namespace WaifuL2d {
template <typename T>
concept HasXY = requires(T t1, T t2)
                {
                  { t1.x() } -> std::convertible_to<double>;
                  { t1.y() } -> std::convertible_to<double>;
                } || requires(T t1, T t2)
                {
                  { t1->x() } -> std::convertible_to<double>;
                  { t1->y() } -> std::convertible_to<double>;
                };

template <typename T>
concept HasUV = requires(T t1, T t2)
                {
                  { t1.u() } -> std::convertible_to<double>;
                  { t1.v() } -> std::convertible_to<double>;
                } || requires(T t1, T t2)
                {
                  { t1->u() } -> std::convertible_to<double>;
                  { t1->v() } -> std::convertible_to<double>;
                };

template <typename T>
concept CanSetXY = requires(T t1, double x)
                   {
                     { t1.setX(x) };
                     { t1.setY(x) };
                   } || requires(T t1, double x)
                   {
                     { t1->setX(x) };
                     { t1->setY(x) };
                   };

template <HasXY Point>
class MeshMathTool {
  template <typename T>
  static auto& deref(T& t) {
    if constexpr (std::is_pointer_v<T>) {
      return *t;
    } else {
      return t;
    }
  }

public:
  static double cross(double x1, double y1, double x2, double y2) {
    return x1 * y2 - x2 * y1;
  }

  static double cross(const Point& aVec, const Point& bVec) {
    const auto& a = deref(aVec);
    const auto& b = deref(bVec);
    return cross(a.x(), a.y(), b.x(), b.y());
  }

  static double cross(const Point& a, const Point& p1, const Point& b,
                      const Point& p2) {
    const auto& aRef = deref(a);
    const auto& p1Ref = deref(p1);
    const auto& bRef = deref(b);
    const auto& p2Ref = deref(p2);
    return cross(aRef.x() - p1Ref.x(), aRef.y() - p1Ref.y(),
                 bRef.x() - p2Ref.x(), bRef.y() - p2Ref.y());
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

    const auto& aRef = deref(a);
    const auto& bRef = deref(b);
    const auto& cRef = deref(c);

    double x = lambda1 * aRef.x() + lambda2 * bRef.x() + lambda3 * cRef.x();
    double y = lambda1 * aRef.y() + lambda2 * bRef.y() + lambda3 * cRef.y();

    return std::make_pair(x, y);
  }

  static std::pair<double, double> fromBarycentricCoordinates(
      const std::array<double, 3>& lambdas, const Point& a, const Point& b,
      const Point& c)
    requires HasUV<Point> {
    double lambda1 = lambdas[0];
    double lambda2 = lambdas[1];
    double lambda3 = lambdas[2];
    const auto& aRef = deref(a);
    const auto& bRef = deref(b);
    const auto& cRef = deref(c);

    double x = lambda1 * aRef.u() + lambda2 * bRef.u() + lambda3 * cRef.u();
    double y = lambda1 * aRef.v() + lambda2 * bRef.v() + lambda3 * cRef.v();

    return std::make_pair(x, y);
  }

  /**
   * use to calculate Graphics item bound
   * @param vector scene position
   * @return bound
   */
  static QRectF calculateBoundRect(const Point* vector, size_t size) {
    const auto& first = deref(vector[0]);
    float left = first.x();
    float right = first.x();
    float top = first.y();
    float bottom = first.y();

    for (size_t i = 0; i < size; i++) {
      const auto& item = deref(vector[i]);
      if (item.x() < left) {
        left = item.x();
      }
      if (item.x() > right) {
        right = item.x();
      }
      if (item.y() < top) {
        top = item.y();
      }
      if (item.y() > bottom) {
        bottom = item.y();
      }
    }
    return QRectF(QPointF(left, top), QPointF(right, bottom));
  }

  static void resizePointInBound(const QRectF& startBound,
                                 const QRectF& resizeBound, Point* vector,
                                 size_t size, bool isXFlip = false,
                                 bool isYFlip = false)
    requires CanSetXY<Point> {
    for (size_t i = 0; i < size; i++) {
      auto& item = vector[i];
      auto& itemRef = deref(item);
      auto u = (itemRef.x() - startBound.left()) / startBound.width();
      auto v = (itemRef.y() - startBound.top()) / startBound.height();
      if (isXFlip) {
        u = 1 - u;
      }
      if (isYFlip) {
        v = 1 - v;
      }
      itemRef.setX(resizeBound.left() + u * resizeBound.width());
      itemRef.setY(resizeBound.top() + v * resizeBound.height());
    }
  }

  static void rotatePoints(double angle, const Point& center, Point* points,
                           size_t size)
    requires CanSetXY<Point> {
    const auto& centerRef = deref(center);
    for (int i = 0; i < size; i++) {
      auto& point = points[i];
      auto& pointRef = deref(point);
      auto x = pointRef.x();
      auto y = pointRef.y();
      pointRef.setX(centerRef.x() + (x - centerRef.x()) * cos(angle) -
                    (y - centerRef.y()) * sin(angle));
      pointRef.setY(centerRef.y() + (x - centerRef.x()) * sin(angle) +
                    (y - centerRef.y()) * cos(angle));
    }
  }
};
} // namespace WaifuL2d