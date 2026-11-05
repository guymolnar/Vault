#include <aes.hpp>
#include <cstring>
#include <stdexcept>

std::vector<uint8_t> serialize(const encryptedDataStruct& encryptedData) {
	std::vector<uint8_t> out;
	auto len = static_cast<uint32_t>(encryptedData.data.size());
	out.insert(out.end(), reinterpret_cast<uint8_t*>(&len), reinterpret_cast<uint8_t*>(&len) + 4);
	out.insert(out.end(), encryptedData.salt.begin(), encryptedData.salt.end());
	out.insert(out.end(), encryptedData.iv.begin(),   encryptedData.iv.end());
	out.insert(out.end(), encryptedData.tag.begin(),  encryptedData.tag.end());
	out.insert(out.end(), encryptedData.data.begin(), encryptedData.data.end());
	return out;
}

encryptedDataStruct deserialize(const std::vector<uint8_t>& encryptedDataBytes)
{
	encryptedDataStruct result;
	// Header is len(4) + salt(16) + iv(12) + tag(16) = 48 bytes.
	if (encryptedDataBytes.size() < 48)
	{
		throw std::runtime_error("serialized data too small (missing header)");
	}
	uint32_t len = 0;
	std::memcpy(&len, encryptedDataBytes.data(), 4);
	if (encryptedDataBytes.size() < 48 + static_cast<size_t>(len))
	{
		throw std::runtime_error("serialized data too small (missing data)");
	}
	result.salt.assign(encryptedDataBytes.data() + 4,  encryptedDataBytes.data() + 20);
	result.iv.assign(encryptedDataBytes.data() + 20, encryptedDataBytes.data() + 32);
	result.tag.assign(encryptedDataBytes.data() + 32, encryptedDataBytes.data() + 48);
	result.data.assign(encryptedDataBytes.data() + 48, encryptedDataBytes.data() + 48 + len);
	return result;
}