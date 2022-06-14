#pragma once
#include "WindowSystem.h"
class ArkEngine
{
private:
	void Shutdown() const;
	WindowSystem m_window;
public:
	explicit ArkEngine();
	void Execute();
};