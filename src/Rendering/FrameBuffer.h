#pragma once
#include <iostream>

class FrameBuffer
{
public:
	FrameBuffer(int width, int height);
	~FrameBuffer();
	unsigned int GetFrameTexture();
	void RescaleFrameBuffer(int width, int height);
	void Bind() const;
	void Unbind() const;
private:
	unsigned int fbo;
	unsigned int texture;
	unsigned int rbo;
};

