#ifndef IMAGE_WRITER_H
#define IMAGE_WRITER_H

class ImageWriter
{
public:
	static int PpmWrite(const char* filename, const float* pixels, int width, int height);
};

#endif