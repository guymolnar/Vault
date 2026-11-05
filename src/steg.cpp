#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <steg/steg.hpp>

#include <stdexcept>

void embedData(const std::vector<uint8_t> &data, const char *imagePath, const char *outputPath)
{
	int width, height, channels;
	uint8_t* pixels = stbi_load(imagePath, &width, &height, &channels, 3);
	if (!pixels)
	{
		throw std::runtime_error("failed to load image");
	}
	int totalPixelsBytes = width * height * 3;
	if (data.size() > totalPixelsBytes / 8)
	{
		stbi_image_free(pixels);
		throw std::runtime_error("image too big");
	}
	for (int i = 0; i < data.size() * 8; i++)
	{
		uint8_t byte = data[i / 8]; //getting the current byte (each byte is 8 bits)
		uint8_t bitPos = 7 - (i % 8); //Getting our current bit location in the byte
		uint8_t bit = (byte >> bitPos) & 1; //Shifting the byte by bitPos making the target bit
		//in position 0, and then 'anding' it with 1 to isolate it
		pixels[i] = (pixels[i] & 0xFE) | bit; //Clears the lsb of the pixel
		//By anding it with 11111110 (0xFE) and setting it to the desired bit
	}
	stbi_write_png(outputPath, width, height, 3, pixels, width * 3);
	stbi_image_free(pixels);
}

std::vector<uint8_t> extractData(const char *imagePath)
{
	int width, height, channels;
	uint8_t* pixels = stbi_load(imagePath, &width, &height, &channels, 3);
	if (!pixels)
	{
		throw std::runtime_error("failed to load image");
	}
	uint8_t lenBytes[4] = {0};
	for (int i = 0; i < 32; i++)
	{
		uint8_t bit = pixels[i] & 1;
		lenBytes[i / 8] = (lenBytes[i / 8] << 1) | bit;
	}
	uint32_t dataLen = 0;
	std::memcpy(&dataLen, lenBytes, 4);
	int totalBytes = 32 + static_cast<int>(dataLen);
	if (totalBytes * 8 > width * height * 3) {
		stbi_image_free(pixels);
		throw std::runtime_error("Data size exceeds image capacity");
	}
	std::vector<uint8_t> raw(totalBytes, 0);
	for (int i = 0; i < totalBytes * 8; i++)
	{
		uint8_t bit = pixels[i] & 1; //getting the LSB
		raw[i / 8] = (raw[i / 8] << 1) | bit; //shifting the raw buffer and 'oring' it to get the bit
	}
	stbi_image_free(pixels);
	return raw;
}