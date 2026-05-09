#include "crypto.h"
#include "PicoSHA2/picosha2.h"
#include <string>
using namespace std;

string SHA256Encrypt(const string& rawText) {
    return picosha2::hash256_hex_string(rawText);
}