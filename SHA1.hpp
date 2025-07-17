#pragma once

#include <iostream>
#include <string>
#include "TinySHA1.hpp"

using namespace std;

class SHA1_maker{
public:
    string SHA(const string& content){
        sha1::SHA1 s;
        s.processBytes(content.c_str(), content.size());
        uint32_t digest[5];
        s.getDigest(digest);	
        char tmp[48];
        snprintf(tmp, 45, "%08x%08x%08x%08x%08x", digest[0], digest[1], digest[2], digest[3], digest[4]);
        return tmp;
    }
};
