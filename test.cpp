#include <iostream>
#include "SHA1.cpp"

using namespace std;

int main(){
    SHA1_maker s;
    string a = s.SHA(R"(#include <iostream>
#include "SHA1.cpp"

abstract class Objects{
    protected:
        string hash;
    public:
        Objects(const string& content){
            SHA1_maker s;
            hash = s.SHA(content);
        }
    })");
    cout<<a<<endl;
    return 0;
}