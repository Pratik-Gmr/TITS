#include <iostream>
#include <fstream>
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
        fs::create_directory(".tits/objects");
        fs::create_directory(".tits/objects/commits");
        fs::create_directory(".tits/objects/trees");
        fs::create_directory(".tits/objects/blobs");
        fs::path file_path=".tits/index.tits";
        std::ofstream file(file_path);
        if(!file){
            throw std::runtime_error("Failed to create file");
        }
        file.close();
        file_path=".tits/ignore.tits";
        file.open(file_path);
        if(!file){
            throw std::runtime_error("Failed to create file");
        }
        file<<".tits/"<<endl;//always ignore .tits directory
        file.close();
        file_path=".tits/head";
        file.open(file_path);
        if(!file){
            throw std::runtime_error("Failed to create file");
        }
        file.close();

        //data member value remains false if error occurred
        //will be made true if initialization succeeds
        initialized = true;
    }
};
