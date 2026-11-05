#pragma once
#include <aes/aes.hpp>
std::vector<uint8_t> serialize(const encryptedDataStruct& encryptedData);
encryptedDataStruct deserialize(const std::vector<uint8_t>& encryptedDataBytes);