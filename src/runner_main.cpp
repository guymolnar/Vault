#include <iostream>
#include <aes.hpp>
#include <steg.hpp>
#include <utils.hpp>

int main(int argc, char* argv[]) {
    std::cout << "GhostRun Runner" << std::endl;
    if (argc != 2)
    {
        std::cout << "Wrong Usage! Try: " << argv[0] << " <PhotoPath>" << std::endl;
        return 1;
    }
    const char *ImagePath = argv[1];

    try
    {
        std::vector<uint8_t> encryptedDataRawBytes = extractData(ImagePath);
        encryptedDataStruct encryptedData = deserialize(encryptedDataRawBytes);
        std::vector<uint8_t> decryptedData = aesDecrypt(encryptedData, AES_KEY);
        std::string decryptedDataString(decryptedData.begin(), decryptedData.end());
        for (const auto& c : decryptedData)
        {
            std::cout << c;
        }
    }
    catch (std::exception& e)
    {
        std::cout << "Caught Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
