#include <Arduino.h>
#include <Base32-Decode.h>

class Key {
private:

    //supports up-to 32 byte long TOTP keys
    uint8_t m_decodedKey[32];
    uint8_t m_keyLength;

    void decodeKey(const char* base32Key) {
        //get the key's length in bytes
        m_keyLength = base32decode(base32Key, NULL, 0);

        //decode the base32 key
        base32decode(base32Key, m_decodedKey, 32);
    };        

public:
    Key(const char* base32Key){
        decodeKey(base32Key);
    };

    uint8_t* getKey() { return m_decodedKey; };
    uint8_t getKeyLength() { return m_keyLength; };


};