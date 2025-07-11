#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>

using namespace std;
namespace fs = filesystem;

class Indexer{
    vector<fs::path> stage;
    vector<fs::path> ignore;

    void ignore_init(){
        fs::path file_path(".tits/ignore.tits");
        fstream file(file_path);
        if(!file){
            throw runtime_error("Unexpected Error has occurred while reading the staging area of this TITS directory\n");
        }
        string line;
        fs::path default_tits_path(".tits/");
        bool default_tits_path_found = false;
        while(getline(file, line)){
            fs::path ignored_files_path(line);
            if(ignored_files_path == default_tits_path) default_tits_path_found = true;
            ignore.push_back(ignored_files_path.string());
        }
        file.close();
        if(!default_tits_path_found) throw runtime_error("Your ignore.tits file has been overwritten incorrectly, Consult TITS documentation in its github repo\n");
    }

    bool is_ignored(const fs::path &file_path){
        size_t ignore_len = ignore.size();
        for(int i = 0; i < ignore_len; i++){
            if(!fs::is_regular_file(ignore[i]) && !fs::is_directory(ignore[i])) continue;
            if(file_path == normalize_path(ignore[i])) return true;
            fs::path relative = file_path.lexically_relative(normalize_path(ignore[i]));
            if(!relative.empty() && *relative.begin() != ".." ) return true;
        }
        return false;
    }

    void stage_init(){
        fs::path file_path(".tits/index.tits");
        fstream file(file_path);
        if(!file){
            throw runtime_error("Unexpected Error has occurred while reading the staging area of this TITS directory\n");
        }
        string line;
        while(getline(file, line)){
            fs::path staged_files_path(line);
            if(!is_regular_file(staged_files_path)) throw runtime_error("Files being staged are not regular files\n");
            staged_files_path = normalize_path(staged_files_path);
            if(is_ignored(staged_files_path)) continue; //dont write that file to stage
            stage.push_back(staged_files_path.string());
        }
        file.close();
        file.open(file_path, ios::out);
        if(!file){
            throw runtime_error("Unexpected Error has occurred while reading the staging area of this TITS directory\n");
        }
        for(const auto& iterated_path : stage){ //to write normalized version to the file as well,
            file<<iterated_path.string()<<endl; //if some user manually wrote paths to the file
        }
        file.close();
    }

    bool is_staged(const fs::path &file_path){
        size_t stage_len = stage.size();
        for(int i = 0; i < stage_len; i++){
            if(file_path == stage[i]) return true;
        }
        return false;
    }

    fs::path normalize_path(const fs::path &file_path) { //for consistency we normalize all paths
        fs::path repo = fs::canonical(".tits").parent_path();
        fs::path absolute = fs::absolute(file_path);
        fs::path Relative = fs::relative(absolute , repo);
        return Relative.lexically_normal();  // remove dots, redundant slashes
    }

public:
    Indexer(){
        ignore_init();
        stage_init();
    }
    ~Indexer() = default;
    
    void log(){
        size_t stage_len = stage.size();
        size_t ignore_len = ignore.size();
        if(!stage_len) cout<<"There are no files staged currently\n";
        else{
            cout<<"The files staged currently are:\n";
            for(int i = 0; i < stage_len; i++){
                cout<<stage[i].string()<<endl;
            }
        }
        if(!ignore_len) cout<<"Your ignore.tits file has been overwritten incorrectly, Consult TITS documentation in its github repo\n";
        else{
            cout<<"The files/directories ignored currently are:\n";
            for(int i = 0; i < ignore_len; i++){
                cout<<ignore[i].string()<<endl;
            }
        }
    }

    void add(string &string_path){
        fs::path file_path(string_path);
        if(!fs::is_regular_file(file_path) && !fs::is_directory(file_path)){
            throw runtime_error("error: the said file/directory doesn't exist\n");
        }
        file_path = normalize_path(file_path);
        fs::path index_file_path(".tits/index.tits");
        ofstream file(index_file_path,ios::app);
        if(!file){
            throw runtime_error("Unexpected Error has occurred while writing to the staging area of this TITS directory\n");
        }
        if(fs::is_directory(file_path)){ //maybe write an iterator over this later?
            for(const auto& iterated_file : fs::recursive_directory_iterator(file_path)){
                if(fs::is_regular_file(iterated_file)){
                    fs::path normalized_iterated_file = normalize_path(iterated_file.path());
                    if(!is_staged(normalized_iterated_file ) && !is_ignored(normalized_iterated_file )){
                        stage.push_back(normalized_iterated_file.string()); //this line could be removed but lets keep it in case it might be needed in future versions
                        file << normalized_iterated_file.string() << endl;
                    }
                }
            }
        }
        else{
            if(is_ignored(file_path)){
                throw runtime_error("ERROR: Said file is in ignore list, can't stage the file\n");
            }
            else if(is_staged(file_path)){
                throw runtime_error("ERROR: Said file is already staged\n");
            }
            stage.push_back(file_path);//same reasoning
            file<<file_path.string()<<endl;
        }
        file.close();
        cout<<"The said file/files has successfully been added\n";
    }

    void remove(string &string_path){
        fs::path file_path(string_path);
        if(!fs::is_regular_file(file_path) && !fs::is_directory(file_path)){
            throw runtime_error("error: the said file/directory doesn't exist\n");
        }
        file_path = normalize_path(file_path);
        size_t len = stage.size();
        if(fs::is_regular_file(file_path)){
            int i;
            for(i = 0; i < len; i++){
                if(file_path == stage[i]){
                    stage.erase(stage.begin() + i); //removes from the stage vector
                    break;
                }
            }
            if (i==len) throw runtime_error("ERROR : Said file is not in stage to remove it\n");
        }
        else{
            for(int i = 0; i < stage.size(); i++){
                fs::path relative = stage[i].lexically_relative(file_path);
                if(!relative.empty() && *relative.begin() != ".." ){
                    stage.erase(stage.begin() + i); //removes from the stage vector
                    i--;
                }
            }
        }
        fs::path new_file_path(".tits/index.tits");
        ofstream file(new_file_path);
        if(!file){
            throw runtime_error("Unexpected Error has occurred while reading the staging area of this TITS directory\n");
        }
        for(const auto& iterated_path : stage){
            file<<iterated_path.string()<<endl;
        }
        cout << "Successfully removed from the staging area\n";
        file.close();
    }

    void to_ignore(string &string_path){
        fs::path file_path(string_path);
        fs::path ignore_file_path(".tits/ignore.tits");
        ofstream file(ignore_file_path,ios::app);
        if(!file){
            throw runtime_error("Unexpected Error has occurred while writing to the ignoring area of this TITS directory\n");
        }
        ignore.push_back(file_path.string()); //again same reasoning to keep this.
        file << file_path.string() << endl;
        cout << "Said file/directory has been successfully added to ignore list\n";
    }

};
