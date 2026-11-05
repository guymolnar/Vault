#pragma once
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <string>

#define MAGIC 0x47535452 //GSTR

enum PayloadType
{
	MESSAGE,
	COMMAND,
	BINARY,
	SCRIPT,
	STAGER
};

extern std::unordered_map<std::string, PayloadType> stringToPayloadType;

#pragma pack(push, 1)
struct PayloadHeader
{
	uint32_t magic;
	PayloadType type;
	uint32_t dataSize;
};
#pragma pack(pop)

struct Payload
{
	PayloadHeader header;
	std::vector<uint8_t> data;
};

Payload createPayload(PayloadType type, const std::vector<uint8_t>& data);