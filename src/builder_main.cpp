#include <iostream>
#include <vector>
#include <string>
#include <aes.hpp>
#include <cstring>
#include <filesystem>
#include <steg.hpp>
#include <utils.hpp>
#include <openssl/rand.h>

int main(int argc, char* argv[])
{
    //argv[0] = . | argv[1] = message data | argv[2] = original photo path | argv[3] = output photo path
    std::cout << "GhostRun Builder" << std::endl;
    if (argc != 4)
    {
        std::cout << "Wrong Usage!" << std::endl << "Try: " << argv[0] << " <Data> <OriginalPhotoPath> <NewPhotoPath>" << std::endl;
        std::cout << "Example: " << std::endl;
        std::cout << "./GhostRunBuilder \"this is a secret message\" photo.png output.png" << std::endl;
        return 1;
    }
    if (!std::filesystem::exists(argv[2]))
    {
        std::cout << "File does not exist: " << argv[2] << std::endl;
        return 1;
    }

    std::vector<uint8_t> data(argv[1], argv[1] + strlen(argv[1]));
    const char *originalImagePath = argv[2];
    const char *newImagePath = argv[3];

    std::string password = readPassword("Password: ");
    std::string confirm  = readPassword("Confirm password: ");
    if (password.empty())
    {
        std::cout << "Password cannot be empty." << std::endl;
        return 1;
    }
    if (password != confirm)
    {
        std::cout << "Passwords do not match." << std::endl;
        return 1;
    }

    try
    {
        std::vector<uint8_t> salt(16);
        if (RAND_bytes(salt.data(), salt.size()) != 1)
        {
            throw std::runtime_error("failed to generate salt");
        }
        std::vector<uint8_t> key = deriveKey(password, salt);

        encryptedDataStruct encryptedData = encryptDataAes(data, key.data());
        encryptedData.salt = salt;   // store the salt so the receiver can re-derive the key

        //When embedding the struct into the image, we firstly serialize it into raw bytes.
        std::vector<uint8_t> encryptedDataRawBytes = serialize(encryptedData);
        embedData(encryptedDataRawBytes, originalImagePath, newImagePath);
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