#pragma once
struct GLFWwindow;	// use forward declaration

class WindowSystem
{
public:
	WindowSystem() noexcept = default;
	WindowSystem(WindowSystem&&) = default;
	WindowSystem& operator=(WindowSystem&&) = default;
	~WindowSystem() = default;
	// Disable Copying
	WindowSystem(const WindowSystem&) = delete;
	WindowSystem& operator=(const WindowSystem&) = delete;
	GLFWwindow* Init();
	void SwapBuffers() const;
	[[nodiscard]] bool IsCursorVisible() const
	{
		return m_showCursor;
	}
	[[nodiscard]] bool ShouldClose() const
	{
		return m_shouldClose;
	}

	void Update();
	void Shutdown() const;
private:
	GLFWwindow* m_window{ nullptr };
	bool m_showCursor{ false };
	bool m_shouldClose{ false };
};