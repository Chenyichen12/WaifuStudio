#include <QPointF>
#include "model/scene/editmeshcontroller.h"
#include "gtest/gtest.h"

class EditMeshCdtTest : public testing::Test {
 protected:
  void SetUp() override {
    incident = {0, 1, 2, 0, 2, 3};
    point = {{0, 0}, {100, 0}, {100, 100}, {0, 100}};
  }
  void TearDown() override{}

 public:
  std::vector<unsigned int> incident;
  std::vector<QPointF> point;
};

TEST_F(EditMeshCdtTest, test_incident_cdt) {
  auto edge =  Scene::EditMeshController::incidentToEdge(incident);
  ASSERT_EQ(edge.size(), 5);
}
