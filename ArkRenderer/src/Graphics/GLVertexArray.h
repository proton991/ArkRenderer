#pragma once
#include <glad/glad.h>

class GLVertexArray
{
public:
	enum BufferType : int
	{
		Array = GL_ARRAY_BUFFER,
		Element = GL_ELEMENT_ARRAY_BUFFER
	};

	enum DrawMode : int
	{
		Static = GL_STATIC_DRAW,
		Dynamic = GL_DYNAMIC_DRAW,
		Stream = GL_STREAM_DRAW
	};

	void Init() noexcept;

	void AttachBuffer(const BufferType type, const size_t size,
	                  const DrawMode mode, const void* data) noexcept;

	void Bind() const noexcept;
	void EnableAttribute(const unsigned int index, const int size,
	                     const unsigned int offset, const void* data) noexcept;
	void Delete() noexcept;

private:
	unsigned int m_vao{0};
};
