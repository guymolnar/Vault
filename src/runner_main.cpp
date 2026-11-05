#include <iostream>
#include <string>
#include <aes.hpp>
#include <steg.hpp>
#include <utils.hpp>

int main(int argc, char* argv[]) {
    std::cout << "Vault Runner" << std::endl;
    if (argc != 2)
    {
        std::cout << "Wrong Usage! Try: " << argv[0] << " <PhotoPath>" << std::endl;
        return 1;
    }
    const char *ImagePath = argv[1];

    std::string password = readPassword("Password: ");

    try
    {
        std::vector<uint8_t> encryptedDataRawBytes = extractData(ImagePath);
        encryptedDataStruct encryptedData = deserialize(encryptedDataRawBytes);
        std::vector<uint8_t> key = deriveKey(password, encryptedData.salt);
        std::vector<uint8_t> decryptedData = aesDecrypt(encryptedData, key.data());
        std::string decryptedDataString(decryptedData.begin(), decryptedData.end());
        std::cout << "Decrypted Data: " << decryptedDataString << std::endl;
    }
    catch (std::exception& e)
    {
        std::cout << "Caught Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}