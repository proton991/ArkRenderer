#pragma once
#include "WindowSystem.h"
#include "RenderSystem.h"
#include "../Camera.h"
class ArkEngine
{
private:
	void Shutdown() const;
	WindowSystem m_window;
	Camera m_camera;
	RenderSystem m_renderer;
public:
	explicit ArkEngine();
	void Execute();
};