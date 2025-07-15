#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>
#include <ctime>

#include "SHA1.cpp"
//#include "indexing.cpp"

using namespace std;
namespace fs = filesystem;
using json = nlohmann::json;

struct files {
    fs::path filepath;
    string sha;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(files, filepath, sha)

class Tree;

class Blob {
    string content;
    string hash;

    friend class Tree;

    fs::path normalize_path(
        const fs::path &file_path) { // for consistency we normalize all paths
        fs::path repo = fs::canonical(".tits").parent_path();
        fs::path absolute = fs::absolute(file_path);
        fs::path Relative = fs::relative(absolute, repo);
        return Relative.lexically_normal(); // remove dots, redundant slashes
    }

public:
    Blob() = default;

    Blob(fs::path file_path) {
        // for inp fille
        std::ifstream in_file(file_path, ios::binary);
        if (!in_file) {
            throw runtime_error("Unexpected Error has occurred while reading the "
                                "input file while creating a blob\n");
        }
        // gotta first resize our content string before filling it
        in_file.seekg(0, ios::end);
        size_t size = in_file.tellg();
        // bringing file pointer back
        in_file.seekg(0, ios::beg);
        content.resize(size);
        // storing into content string
        in_file.read(&content[0], size);
        SHA1_maker sha;
        hash = sha.SHA(content);
        // for outp file
        string blob_dir_path = ".tits/objects/blobs/";
        string final_path = blob_dir_path + hash;
        fs::path blob_file_path(final_path);
        blob_file_path = normalize_path(blob_file_path);
        std::ofstream out_file(blob_file_path, ios::binary);
        if (!out_file) {
            throw runtime_error("Unexpected Error has occurred while writing the "
                                "input file while creating a blob\n");
        }
        in_file.seekg(0, ios::beg);
        out_file << in_file.rdbuf();
        in_file.close();
        out_file.close();
    }

    Blob(string h, string c) : hash(h), content(c) {}
};

class Tree {
    vector<files> items;
    string hash;

    fs::path normalize_path(
        const fs::path &file_path) { // for consistency we normalize all paths
        fs::path repo = fs::canonical(".tits").parent_path();
        fs::path absolute = fs::absolute(file_path);
        fs::path Relative = fs::relative(absolute, repo);
        return Relative.lexically_normal(); // remove dots, redundant slashes
    }

    void create_tree_file() {
        string content;
        json j = items;
        fs::path temp_path = ".tits/objects/trees/temp";
        std::ofstream temp_file(temp_path, ios::binary);
        if (!temp_file) {
            throw runtime_error("Unexpected Error has occurred while reading the "
                                "input file while creating a blob\n");
        }
        temp_file << j.dump(4);
        temp_file.close();
        std::ifstream in_file(temp_path, ios::binary);
        if (!in_file) {
            throw runtime_error("Unexpected Error has occurred while reading the "
                                "input file while creating a blob\n");
        }
        // gotta first resize our content string before filling it
        in_file.seekg(0, ios::end);
        size_t size = in_file.tellg();
        // bringing file pointer back
        in_file.seekg(0, ios::beg);
        content.resize(size);
        // storing into content string
        in_file.read(&content[0], size);
        SHA1_maker sha;
        hash = sha.SHA(content);
        fs::path tree_file_path = normalize_path(".tits/objects/trees/" + hash);
        fs::rename(temp_path, tree_file_path);
    }

public:

    Tree() {
        Indexer index;
        vector<fs::path> stage = index.get_stage();
        for (const auto &staged_file : stage) {
            Blob blob(staged_file);
            struct files blob_file;
            blob_file.filepath = staged_file;
            blob_file.sha = blob.hash;
            items.push_back(blob_file);
        }
        create_tree_file();
    }

};

class Commit {
    string hash;
    string message;
    vector<string> parents;
    vector<string> children;
    string tree_hash;
    string time;

    std::string get_timestamp() {
        std::time_t now = std::time(nullptr);
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return std::string(buf);
    }

public:
    Commit() {
        Tree tree;
    };
};
