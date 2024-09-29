#include <qpoint.h>
#include "gtest/gtest.h"
#include "model/scene/meshmathtool.hpp"
#include <QPointF>

#include <array>
TEST(mathtool, testcross) {
  auto crossA = WaifuL2d::MeshMathTool<QPointF>::cross(1, 2, 3, 4);
  auto crossB =
      WaifuL2d::MeshMathTool<QPointF>::cross(QPointF(1, 2), QPointF(3, 4));
  auto crossC = WaifuL2d::MeshMathTool<QPointF>::cross(
      QPointF(1, 2), QPointF(2, 4), QPointF(5, 6), QPointF(8, 10));
  ASSERT_EQ(crossA, -2);
  ASSERT_EQ(crossB, -2);
  ASSERT_EQ(crossC, -2);


  // triangle
  std::array triangle = {QPointF(0, 0), QPointF(1, 0),
                                     QPointF(0, 1)};

  auto isInTriangle = WaifuL2d::MeshMathTool<QPointF>::isInTriangle(
      QPointF(0.5, 0.5), triangle[0], triangle[1], triangle[2]);
  ASSERT_TRUE(isInTriangle);
  isInTriangle = WaifuL2d::MeshMathTool<QPointF>::isInTriangle(
      QPointF(0.5, 0.6), triangle[0], triangle[1], triangle[2]);
  ASSERT_FALSE(isInTriangle);

  isInTriangle = WaifuL2d::MeshMathTool<QPointF>::isInTriangle(
      QPointF(0,0), triangle[0], triangle[0], triangle[0]);
  ASSERT_TRUE(isInTriangle);
}


TEST(mathtool, barycentricCoordinates) {
  auto a = QPointF(0, 0);
  auto b = QPointF(1, 1);
  auto c = QPointF(2,0);

  auto p = QPointF (1, 0.5);

  auto zuo = WaifuL2d::MeshMathTool<QPointF>::barycentricCoordinates(p, a, b, c);
  ASSERT_EQ(std::get<1>(zuo), 0.5);

  auto p2 =
      WaifuL2d::MeshMathTool<QPointF>::fromBarycentricCoordinates(zuo, a, b, c);
  ASSERT_EQ(QPointF(p2.first,p2.second), p);

}
