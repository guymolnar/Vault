#pragma once
#include <vector>
#include <cstdint>

void embedData(const std::vector<uint8_t>& data, const char* imagePath, const char* outputPath);
std::vector<uint8_t> extractData(const char* imagePath);