#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

class Initializer{
    bool initialized;
public:
    Initializer(){
        fs::path TITS = ".tits";
        if(fs::exists(TITS)) initialized = true;
        else initialized = false;
    }
    bool getStatus(){
        return initialized;
    }
    void init(){
        //creates boilerplate .tits folder
        fs::create_directory(".tits");
        //data member value remains false if error occurred
        //will be made true if initialization succeeds
        initialized = true;
    }
};