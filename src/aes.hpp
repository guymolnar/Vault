#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <openssl/evp.h>
#include <openssl/rand.h>

struct encryptedDataStruct {
	std::vector<uint8_t> salt;
	std::vector<uint8_t> iv;
	std::vector<uint8_t> tag;
	std::vector<uint8_t> data;
};

std::vector<uint8_t> deriveKey(const std::string& password, const std::vector<uint8_t>& salt);

encryptedDataStruct encryptDataAes(const std::vector<uint8_t>& plaintext, const uint8_t* key);
std::vector<uint8_t> aesDecrypt(const encryptedDataStruct& encrypted, const uint8_t* key);