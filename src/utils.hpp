#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <aes.hpp>

std::vector<uint8_t> serialize(const encryptedDataStruct& encryptedData);
encryptedDataStruct deserialize(const std::vector<uint8_t>& encryptedDataBytes);
std::string readPassword(const std::string& prompt);