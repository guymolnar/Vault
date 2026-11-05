#include <payload/payload.hpp>

std::unordered_map<std::string, PayloadType> stringToPayloadType
{
    {"message", MESSAGE},
    {"command", COMMAND},
    {"binary",  BINARY},
    {"script",  SCRIPT},
    {"stager",  STAGER}
};

Payload createPayload(PayloadType type, const std::vector<uint8_t>& data)
{
	PayloadHeader payloadHeader;
	Payload payload;

	payloadHeader.magic = MAGIC;
	payloadHeader.type = type;
	payloadHeader.dataSize = static_cast<uint32_t>(data.size());
	payload.header = payloadHeader;
	payload.data = data;
	return payload;
}

