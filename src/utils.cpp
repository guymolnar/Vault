#include <aes.hpp>
#include <cstring>
#include <stdexcept>

std::vector<uint8_t> serialize(const encryptedDataStruct& encryptedData) {
	std::vector<uint8_t> out;
	//We firstly write the size of the data into the buffer, to know how much bytes to extract later. IV and Tag sizes are already fixed.
	auto len = static_cast<uint32_t>(encryptedData.data.size());
	out.insert(out.end(), reinterpret_cast<uint8_t*>(&len), reinterpret_cast<uint8_t*>(&len) + 4);
	out.insert(out.end(), encryptedData.iv.begin(), encryptedData.iv.end());
	out.insert(out.end(), encryptedData.tag.begin(), encryptedData.tag.end());
	out.insert(out.end(), encryptedData.data.begin(), encryptedData.data.end());
	return out;
}

encryptedDataStruct deserialize(const std::vector<uint8_t>& encryptedDataBytes)
{
	encryptedDataStruct encryptedDataStruct;
	if (encryptedDataBytes.size() < 32)
	{
		throw std::runtime_error("serialized data too small (missing header)");
	}
	uint32_t len = 0;
	std::memcpy(&len, encryptedDataBytes.data(), 4);
	if (encryptedDataBytes.size() < 32 + static_cast<size_t>(len))
	{
		throw std::runtime_error("serialized data too small (missing data)");
	}
	encryptedDataStruct.iv.assign(encryptedDataBytes.data() + 4, encryptedDataBytes.data() + 16);
	encryptedDataStruct.tag.assign(encryptedDataBytes.data() + 16, encryptedDataBytes.data() + 32);
	encryptedDataStruct.data.assign(encryptedDataBytes.data() + 32, encryptedDataBytes.data() + 32 + len);
	return encryptedDataStruct;
}