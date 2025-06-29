#include <iostream>
#include "SHA1.cpp"

abstract class Objects{
    protected:
        string hash;
    public:
        Objects(const string& content){
            SHA1_maker s;
            hash = s.SHA(content);
        }
}