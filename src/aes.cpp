#include <aes.hpp>
#include <stdexcept>

std::vector<uint8_t> deriveKey(const std::string& password, const std::vector<uint8_t>& salt)
{
	std::vector<uint8_t> key(32);
	int iterations = 600000;

	if (PKCS5_PBKDF2_HMAC(
			password.c_str(), password.size(),
			salt.data(), salt.size(),
			iterations, EVP_sha256(),
			key.size(), key.data()) != 1)
	{
		throw std::runtime_error("PBKDF2 key derivation failed");
	}
	return key;
}

encryptedDataStruct encryptDataAes(const std::vector<uint8_t>& plaintext, const uint8_t* key)
{
	encryptedDataStruct result;
	result.iv.resize(12);
	if (RAND_bytes(result.iv.data(), result.iv.size()) != 1)
	{
		throw std::runtime_error("RAND_bytes failed");
	}
	result.tag.resize(16);
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (!ctx)
	{
		throw std::runtime_error("EVP_CIPHER_CTX_new failed");
	}
	if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, result.iv.data()) != 1)
	{
		EVP_CIPHER_CTX_free(ctx);
		throw std::runtime_error("EVP_EncryptInit_ex failed");
	}

	result.data.resize(plaintext.size());
	int len = 0;
	if (EVP_EncryptUpdate(ctx, result.data.data(), &len, plaintext.data(), plaintext.size()) != 1)
	{
		EVP_CIPHER_CTX_free(ctx);
		throw std::runtime_error("EVP_EncryptUpdate failed");
	}

	int finalLen = 0;
	if (EVP_EncryptFinal_ex(ctx, result.data.data() + len, &finalLen) != 1)
	{
		EVP_CIPHER_CTX_free(ctx);
		throw std::runtime_error("EVP_EncryptFinal_ex failed");
	}

	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, result.tag.data()) != 1)
	{
		EVP_CIPHER_CTX_free(ctx);
		throw std::runtime_error("EVP_CTRL_GCM_GET_TAG failed");
	}
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