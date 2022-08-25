#include "FirstApp.hpp"
#include "ArkCamera.hpp"
#include "ArkBuffer.hpp"
#include "systems/SimpleRenderSystem.hpp"
#include "systems/PointLightSystem.hpp"
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
  struct GlobalUbo
  {
    glm::mat4 projection{1.0f};
    glm::mat4 view{1.0f};
    glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f};
    glm::vec3 lightPosition{-1.f};
    alignas(16) glm::vec4 lightColor{1.f}; // (r, g, b, intensity)
    /*glm::vec3 lightDirection = glm::normalize(glm::vec3{3.f, 0.f, 0.f});*/
  };

  FirstApp::FirstApp()
  {
    m_globalPool = ArkDescriptorPool::Builder(m_arkDevice)
                   .SetMaxSets(ArkSwapChain::MAX_FRAMES_IN_FLIGHT)
                   .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, ArkSwapChain::MAX_FRAMES_IN_FLIGHT)
                   .Build();

    LoadGameObjects();
  }

  FirstApp::~FirstApp()
  {
  }

  void FirstApp::Run()
  {
    std::vector<std::unique_ptr<ArkBuffer>> uboBuffers(ArkSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < uboBuffers.size(); i++)
    {
      uboBuffers[i] = std::make_unique<ArkBuffer>(
        m_arkDevice,
        sizeof(GlobalUbo),
        1,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        m_arkDevice.properties.limits.minUniformBufferOffsetAlignment
      );
      uboBuffers[i]->Map();
    }
    auto globalSetLayout = ArkDescriptorSetLayout::Builder(m_arkDevice)
                           .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                           .Build();
    std::vector<VkDescriptorSet> globalDescriptorSets(ArkSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++)
    {
      auto bufferInfo = uboBuffers[i]->DescriptorInfo();
      ArkDescriptorWriter(*globalSetLayout, *m_globalPool)
        .WriteBuffer(0, &bufferInfo)
        .Build(globalDescriptorSets[i]);
    }

    SimpleRenderSystem simpleRenderSystem{
      m_arkDevice, m_arkRenderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout()
    };
    PointLightSystem pointLightSystem{
      m_arkDevice, m_arkRenderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout()
    };
    ArkCamera camera{
      glm::vec3(.0f, .0f, -2.5f), glm::vec3(0.f, 0.f, 0.f), glm::radians(70.0f),
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
      double frameTime = 0.0;
      timer.Update(glfwGetTime());
      if (hasOneSecondPassed)
      {
        frameTime = Timer::FrameTimeMilliseconds(numFramesRendered);
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
        int frameIndex = m_arkRenderer.GetFrameIndex();
        FrameInfo frameInfo{
          frameIndex,
          frameTime,
          commandBuffer,
          camera,
          globalDescriptorSets[frameIndex],
          m_gameObjects
        };
        // update
        GlobalUbo ubo{};
        ubo.projection = camera.GetProjMatrix();
        ubo.view = camera.GetViewMatrix();
        uboBuffers[frameIndex]->WriteToBuffer(&ubo);
        uboBuffers[frameIndex]->Flush();
        // render
        m_arkRenderer.BeginSwapChainRenderPass(commandBuffer);
        simpleRenderSystem.RenderGameObjects(frameInfo);
        pointLightSystem.Render(frameInfo);
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
    gameObj2.m_transform.translation = {0.5f, 0.5f, 0.0f};
    gameObj2.m_transform.scale = {1.5f, 1.5f, 1.5f};
    m_gameObjects.emplace(gameObj.GetId(), std::move(gameObj));
    m_gameObjects.emplace(gameObj2.GetId(), std::move(gameObj2));

    arkModel = ArkModel::CreateModelFromFile(m_arkDevice, "models/quad.obj");
    auto floor = ArkGameObject::Create();
    floor.m_model = arkModel;
    floor.m_transform.translation = {0.5f, 0.5f, 0.0f};
    floor.m_transform.scale = {3.f, 1.f, 3.f};
    m_gameObjects.emplace(floor.GetId(), std::move(floor));
  }
}
