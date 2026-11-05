#include <aes.hpp>
#include <utils.hpp>
#include <cstring>
#include <stdexcept>
#include <iostream>
#ifdef _WIN32
	#include <windows.h>
#else
	#include <termios.h>
	#include <unistd.h>
#endif
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

std::string readPassword(const std::string& prompt)
{
	std::cout << prompt << std::flush;
	std::string password;

#ifdef _WIN32
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD oldMode = 0;
	GetConsoleMode(hStdin, &oldMode);
	SetConsoleMode(hStdin, oldMode & ~ENABLE_ECHO_INPUT);   // turn off echo

	std::getline(std::cin, password);

	SetConsoleMode(hStdin, oldMode);                        // restore
#else
	termios oldTerm;
	tcgetattr(STDIN_FILENO, &oldTerm);
	termios newTerm = oldTerm;
	newTerm.c_lflag &= ~ECHO;                               // turn off echo
	tcsetattr(STDIN_FILENO, TCSANOW, &newTerm);

	std::getline(std::cin, password);

	tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm);             // restore
#endif

	std::cout << std::endl;   // the Enter keypress wasn't echoed on either platform
	return password;
}