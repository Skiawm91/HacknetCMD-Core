#include "crypto.h"
#ifdef _WIN32
#include <winrt/base.h>
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Security.Cryptography.Core.h>
#include <winrt/Windows.Storage.Streams.h>
using namespace winrt;
using namespace Windows::Security::Cryptography;
using namespace Windows::Security::Cryptography::Core;
using namespace Windows::Storage::Streams;
#elif __APPLE__
#include <CommonCrypto/CommonDigest.h>
#endif
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

#ifdef _WIN32
string SHA256Encrypt(const string& rawText) {
    static bool apartmentInitialized = false;
    if (!apartmentInitialized) {
        init_apartment();
        apartmentInitialized = true;
    }
    auto data = CryptographicBuffer::ConvertStringToBinary(to_hstring(rawText), BinaryStringEncoding::Utf8);
    HashAlgorithmProvider hashProvider = HashAlgorithmProvider::OpenAlgorithm(HashAlgorithmNames::Sha256());
    IBuffer hashBuffer = hashProvider.HashData(data);
    com_array<uint8_t> hashData;
    CryptographicBuffer::CopyToByteArray(hashBuffer, hashData);
    stringstream ss;
    for (uint8_t b : hashData) {
        ss << hex << setw(2) << setfill('0') << (int)b;
    }
    return ss.str();
}
#elif __APPLE__
string SHA256Encrypt(const string& rawText) {
    unsigned char hash[CC_SHA256_DIGEST_LENGTH];
    CC_SHA256(rawText.c_str(), (CC_LONG)rawText.size(), hash);
    stringstream ss;
    for(int i = 0; i < CC_SHA256_DIGEST_LENGTH; i++)
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    return = ss.str();
}
#endif