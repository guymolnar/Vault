#include <aes/aes.hpp>
#include <stdexcept>

encryptedDataStruct encryptDataAes(const std::vector<uint8_t>& plaintext, const uint8_t* key)
{
	encryptedDataStruct result;
	result.iv.resize(12);
	RAND_bytes(result.iv.data(), result.iv.size());
	result.tag.resize(16);
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (!ctx)
	{
		throw std::runtime_error("EVP_CIPHER_CTX_new failed");
	}
	EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, result.iv.data());

	result.data.resize(plaintext.size());
	int len = 0;
	EVP_EncryptUpdate(ctx, result.data.data(), &len, plaintext.data(), plaintext.size());

	int finalLen = 0;
	EVP_EncryptFinal_ex(ctx, result.data.data() + len, &finalLen);

	EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, result.tag.data());
	EVP_CIPHER_CTX_free(ctx);
	return result;
}

std::vector<uint8_t> aesDecrypt( const encryptedDataStruct& encrypted, const uint8_t* key) {
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (!ctx) throw std::runtime_error("Failed to create cipher context");

	EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, key, encrypted.iv.data());

	std::vector<uint8_t> plaintext(encrypted.data.size());
	int len = 0;
	EVP_DecryptUpdate(ctx, plaintext.data(), &len, encrypted.data.data(), encrypted.data.size());

	EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, const_cast<uint8_t*>(encrypted.tag.data()));

	if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) <= 0)
	{
		EVP_CIPHER_CTX_free(ctx);
		throw std::runtime_error("Decryption failed: authentication tag mismatch");
	}

	EVP_CIPHER_CTX_free(ctx);
	return plaintext;
}