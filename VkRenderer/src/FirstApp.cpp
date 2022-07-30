#include "FirstApp.hpp"
#include "ArkCamera.hpp"
#include "SimpleRenderSystem.hpp"
//libs
//#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <array>
#include <stdexcept>

#include "InputController.hpp"
#include "Timer.hpp"


namespace Ark
{
  FirstApp::FirstApp()
  {
    LoadGameObjects();
  }

  FirstApp::~FirstApp()
  {
  }

  // temporary helper function, creates a 1x1x1 cube centered at offset
  std::unique_ptr<ArkModel> CreateCubeModel(ArkDevice& device, glm::vec3 offset)
  {
    ArkModel::Builder modelBuilder{};
    modelBuilder.vertices = {
      // left face (white)
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}}, {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}}, {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

      // right face (yellow)
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}}, {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, .5f}, {.8f, .8f, .1f}}, {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

      // top face (orange, remember y axis points down)
      {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}}, {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}}, {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

      // bottom face (red)
      {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}}, {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, .5f, .5f}, {.8f, .1f, .1f}}, {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

      // nose face (blue)
      {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}}, {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}}, {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

      // tail face (green)
      {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}}, {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}}, {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
    };
    for (auto& v : modelBuilder.vertices)
    {
      v.position += offset;
    }
    modelBuilder.indices = {
      0, 1, 2, 0, 3, 1, 4, 5, 6, 4, 7, 5, 8, 9, 10, 8, 11, 9, 12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17,
      20, 21, 22, 20, 23, 21
    };
    return std::make_unique<ArkModel>(device, modelBuilder);
  }

  void FirstApp::Run()
  {
    SimpleRenderSystem simpleRenderSystem{m_arkDevice, m_arkRenderer.GetSwapChainRenderPass()};
    ArkCamera camera{
      glm::vec3(.0f, .0f, -1.0f), glm::vec3(0.f, 0.f, 0.f), glm::radians(70.0f),
      m_arkRenderer.GetAspectRatio(), 0.1f, 100.0f
    };
    bool hasOneSecondPassed{false};
    Timer timer(1.0, [&]()
    {
      hasOneSecondPassed = true;
    });
    unsigned int numFramesRendered{0};
    while (!m_window.ShouldClose())
    {
      timer.Update(glfwGetTime());
      if (hasOneSecondPassed)
      {
        const auto frameTime{Timer::FrameTimeMilliseconds(numFramesRendered)};
        numFramesRendered = 0;
        hasOneSecondPassed = false;
      }

      float aspect = m_arkRenderer.GetAspectRatio();
      //camera.SetOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
      camera.SetAspect(aspect);
      const auto dt{timer.GetDelta()};
      InputManager::GetInstance().Update();
      m_window.Update();
      camera.Update(dt);
      if (auto commandBuffer = m_arkRenderer.BeginFrame())
      {
        m_arkRenderer.BeginSwapChainRenderPass(commandBuffer);
        simpleRenderSystem.RenderGameObjects(commandBuffer, m_gameObjects, camera);
        m_arkRenderer.EndSwapChainRenderPass(commandBuffer);
        m_arkRenderer.EndFrame();
      }
      ++numFramesRendered;
    }
    vkDeviceWaitIdle(m_arkDevice.Device());
  }

  void FirstApp::LoadGameObjects()
  {
    std::shared_ptr<ArkModel> arkModel = ArkModel::CreateModelFromFile(m_arkDevice, "models/smooth_vase.obj");
    auto gameObj = ArkGameObject::Create();
    gameObj.m_model = arkModel;
    gameObj.m_transform.translation = {-0.5f, 0.5f, 0.0f};
    gameObj.m_transform.scale = {1.5f, 1.5f, 1.5f};
    auto gameObj2 = ArkGameObject::Create();
    std::shared_ptr<ArkModel> flatModel = ArkModel::CreateModelFromFile(m_arkDevice, "models/flat_vase.obj");
    gameObj2.m_model = flatModel;
    gameObj2.m_transform.translation = { 0.5f, 0.5f, 0.0f };
    gameObj2.m_transform.scale = { 1.5f, 1.5f, 1.5f };
    m_gameObjects.push_back(std::move(gameObj));
    m_gameObjects.push_back(std::move(gameObj2));
  }
}
