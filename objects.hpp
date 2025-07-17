#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

#include "SHA1.hpp"
#include "indexing.hpp"

using namespace std;
namespace fs = filesystem;
using json = nlohmann::json;

struct files {
    fs::path filepath;
    string hash;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(files, filepath, hash)

class Commit;
class Tree;

class Blob {
    string content;
    string hash;

    friend class Tree;

    fs::path normalize_path(const fs::path &file_path) { // for consistency we normalize all paths
        fs::path repo = fs::canonical(".tits").parent_path();
        fs::path absolute = fs::absolute(file_path);
        fs::path Relative = fs::relative(absolute, repo);
        return Relative.lexically_normal(); // remove dots, redundant slashes
    }

public:
    Blob() = default;

    Blob(const fs::path &file_path) {
        // for inp fille
        std::ifstream in_file(file_path, ios::binary);
        if (!in_file) {
            throw runtime_error("Unexpected Error has occurred while reading the input file while creating a blob\n");
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
            throw runtime_error("Unexpected Error has occurred while reading the input file while creating a blob\n");
        }
        in_file.seekg(0, ios::beg);
        out_file << in_file.rdbuf();
        in_file.close();
        out_file.close();
    }

    Blob(const string& h) : hash(h) {
        fs::path file_path(".tits/objects/blobs/" + hash);
        std::ifstream in_file(file_path, ios::binary);
        if (!in_file) {
            throw runtime_error("Unexpected Error has occurred while reading the input file while creating a blob\n");
        }
        // gotta first resize our content string before filling it
        in_file.seekg(0, ios::end);
        size_t size = in_file.tellg();
        // bringing file pointer back
        in_file.seekg(0, ios::beg);
        content.resize(size);
        // storing into content string
        in_file.read(&content[0], size);
    }

    void unroll(const fs::path &file_path) {
        std::ofstream file(file_path, ios::binary);
        if (!file) {
            throw runtime_error("Unexpected Error has occurred while reading the input file while creating a blob\n");
        }
        file << content;
        file.close();
    }

};

class Tree {
    vector<files> items;
    string hash;

    friend class Commit;

    fs::path normalize_path(const fs::path &file_path) { // for consistency we normalize all paths
        fs::path repo = fs::canonical(".tits").parent_path();
        fs::path absolute = fs::absolute(file_path);
        fs::path Relative = fs::relative(absolute, repo);
        return Relative.lexically_normal(); // remove dots, redundant slashes
    }

    void create_tree_file() {
        json j = items;
        string content(j.dump(4));
        SHA1_maker sha;
        hash = sha.SHA(content);
        fs::path tree_file_path = normalize_path(".tits/objects/trees/" + hash);
        std::ofstream tree_file(tree_file_path, ios::binary);
        if (!tree_file) {
            throw runtime_error("Unexpected Error has occurred while reading the input file while creating a blob\n");
        }
        tree_file << j.dump(4);
        tree_file.close();
    }

public:

    Tree() {
        Indexer index;
        vector<fs::path> stage = index.get_stage();
        bool empty_stage = true;
        for (const auto &staged_file : stage) {
            empty_stage = false;
            Blob blob(staged_file);
            struct files blob_file;
            blob_file.filepath = staged_file;
            blob_file.hash = blob.hash;
            items.push_back(blob_file);
        }
        if (empty_stage){
            cout << "Nothing on the stage to commit\n";
            return;
        }
        create_tree_file();
    }

    Tree(const string& h) : hash(h) {
        fs::path file_path(".tits/objects/trees/" + hash);
        std::ifstream file(file_path);
        if (!file) {
            throw runtime_error("Unexpected Error has occurred while reading the input file while creating a blob\n");
        }
        json j;
        file >> j;
        file.close();
        this -> items = j;
    }

    void unroll() {
        for (const auto& item : items){
            Blob blob(item.hash);
            blob.unroll(item.filepath);
        }
    }

};

class Commit {
    string hash;
    string message;
    vector<string> parents;
    string tree_hash;
    string time;

    friend void to_json(json& j, const Commit& c);
    friend void from_json(const json& j, Commit& c);

    fs::path normalize_path(const fs::path &file_path) { // for consistency we normalize all paths
        fs::path repo = fs::canonical(".tits").parent_path();
        fs::path absolute = fs::absolute(file_path);
        fs::path Relative = fs::relative(absolute, repo);
        return Relative.lexically_normal(); // remove dots, redundant slashes
    }

    void create_commit_file() {
        json j = *this;
        string content(j.dump(4));
        SHA1_maker sha;
        hash = sha.SHA(content);
        fs::path commit_file_path = normalize_path(".tits/objects/commits/" + hash);
        std::ofstream commit_file(commit_file_path, ios::binary);
        if (!commit_file) {
            throw runtime_error("Unexpected Error has occurred while reading the "
                                "input file while creating a blob\n");
        }
        commit_file << j.dump(4);
        commit_file.close();
    }

public:

    Commit() = default;

    Commit(const string &m, string t) : message(m), time(t) {
        string branch;
        fs::path file_path=".tits/HEAD";
        std::ifstream file(file_path);
        if(!file){
            throw std::runtime_error("Failed to create file");
        }
        file >> branch;
        file.close();
        string parent_hash;
        file_path=".tits/branches/" + branch;
        file.open(file_path);
        if(!file){
            throw std::runtime_error("Failed to create file");
        }
        file >> parent_hash;
        file.close();
        parents.push_back(parent_hash);
        Tree tree;
        if (tree.hash == "") return;
        tree_hash = tree.hash;
        create_commit_file();
        std::ofstream out(file_path, std::ios::trunc);
        if(!out){
            throw std::runtime_error("Failed to create file");
        }
        out << hash;
        out.close();
        //emptying the stage after fresh commit
        fs::path index_file_path=".tits/index.tits";
        std::ofstream index_file(index_file_path);
        if(!index_file){
            throw std::runtime_error("Failed to create file");
        }
        index_file.close();
        cout << "Commit successfully made with:\n";
        cout << "Message: " << message << endl;
        cout << "At: " << time << endl;
        cout << "On branch: " << branch << endl;
    };

    Commit(const string& h) : hash(h) {
        fs::path file_path(".tits/objects/commits/" + hash);
        std::ifstream file(file_path);
        if (!file) {
            throw runtime_error("Unexpected Error has occurred while reading the input file while creating a blob\n");
        }
        json j;
        file >> j;
        file.close();
        *this  = j;
        hash = h;
    }

    void unroll() {
        Tree tree(tree_hash);
        tree.unroll();
        //emptying the stage after fresh unrolling
        fs::path index_file_path=".tits/index.tits";
        std::ofstream index_file(index_file_path);
        if(!index_file){
            throw std::runtime_error("Failed to create file");
        }
        index_file.close();
        cout << "Your directory files has been updated to the state of commit :\n" << hash << endl;
    }

    vector<string> get_parents(){
        return parents;
    }

    void display() {
        cout << "****************************************************************\n";
        cout << "Hash: " << hash << endl;
        cout << "Message: " << message << endl;
        cout << "Time stamp: " << time << endl;
        cout << "parents:\n";
        for(const auto& parent : parents) {
            cout << parent << endl;
        }
    }

};

void to_json(json& j, const Commit& c) {
    j = json{
        {"message", c.message},
        {"parents", c.parents},
        {"tree_hash", c.tree_hash},
        {"time", c.time}
    };
}

void from_json(const json& j, Commit& c) {
    j.at("message").get_to(c.message);
    j.at("parents").get_to(c.parents);
    j.at("tree_hash").get_to(c.tree_hash);
    j.at("time").get_to(c.time);
}
