#include <gtest/gtest.h>
#include <model/scene/mesh.h>
#include <model/scene/meshrendergroup.h>
#include <psdparser.h>

#include <filesystem>

#include "model/layer.h"

class SpriteRenderGroupTest : public testing::Test {
 protected:
  void SetUp() override {
    std::string s = __FILE__;
    std::filesystem::path pathObj(s);
    std::filesystem::path resPath =
        pathObj.parent_path() / "test_res" / "colorexample.psd";
    auto parser = Parser::PsdParser(QString::fromStdString(resPath.string()));
    parser.Parse();

    auto psTree = parser.extractPsTree();
    auto manager = parser.extractBitmapManager();
    render_group =
        new Scene::MeshRenderGroup(parser.width(), parser.height());
    psTree->forEach([&](auto&& c) {
      if (c->type() == ProjectModel::LayerTypes::BitmapLayerType) {
        auto item = static_cast<ProjectModel::BitmapLayer*>(c);
        auto builder = Scene::MeshBuilder();
        builder.setUpDefault(manager->getBitmap(item->getBitmapId()));
        auto mesh = builder.buildMesh().release();

        render_group->pushFrontMesh(mesh);

        if (item->text() == QString("layer1")) {
          redLayer = mesh;
        }
        if (item->text() == QString("layer2")) {
          greenLayer = mesh;
        }

        if (item->text() == QString("layer3")) {
          blueLayer = mesh;
        }
        if (item->text() == "WaifuStudio") {
          textLayer = mesh;
        }
      }
      return true;
    });

    delete psTree;
    delete manager;
  }
  void TearDown() override {}
  ~SpriteRenderGroupTest() override { delete render_group; }

 public:
  Scene::MeshRenderGroup* render_group;

  Scene::Mesh* redLayer = nullptr;
  Scene::Mesh* greenLayer = nullptr;
  Scene::Mesh* blueLayer = nullptr;
  Scene::Mesh* textLayer = nullptr;
};

TEST_F(SpriteRenderGroupTest, testvertices_bound) {
  ASSERT_NE(redLayer, nullptr);
  ASSERT_NE(greenLayer, nullptr);
  ASSERT_NE(blueLayer, nullptr);
  ASSERT_NE(textLayer, nullptr);

  ASSERT_EQ(redLayer->boundingRect().left(), 0);
  ASSERT_EQ(redLayer->boundingRect().top(), 0);

  ASSERT_EQ(redLayer->boundingRect().width(), 600);
  ASSERT_EQ(redLayer->boundingRect().height(), 600);

  ASSERT_EQ(blueLayer->boundingRect().left(), 0);
  ASSERT_EQ(blueLayer->boundingRect().width(), 860);
}

