#ifndef BMP_LOADER_H
#define BMP_LOADER_H

// struct BMP_image
// {
// 	int width,
// 	int height,
// 	unsigned char* pixels,
// };

unsigned char* loadbmp(const char* fileName, unsigned int* width, unsigned int* height);

#endif