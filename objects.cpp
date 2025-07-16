#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>
#include <ctime>

using namespace std;
namespace fs = filesystem;
using json = nlohmann::json;

struct files {
    fs::path filepath;
    string sha;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(files, filepath, sha)

class Commit;
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

    friend class Commit;

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
        bool empty_stage = true;
        for (const auto &staged_file : stage) {
            empty_stage = false;
            Blob blob(staged_file);
            struct files blob_file;
            blob_file.filepath = staged_file;
            blob_file.sha = blob.hash;
            items.push_back(blob_file);
        }
        if (empty_stage){
            cout << "Nothing on the stage to commit\n";
            return;
        }
        create_tree_file();
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

    fs::path normalize_path(
        const fs::path &file_path) { // for consistency we normalize all paths
        fs::path repo = fs::canonical(".tits").parent_path();
        fs::path absolute = fs::absolute(file_path);
        fs::path Relative = fs::relative(absolute, repo);
        return Relative.lexically_normal(); // remove dots, redundant slashes
    }

    std::string get_timestamp() {
        std::time_t now = std::time(nullptr);
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return std::string(buf);
    }

    void create_commit_file() {
        string content;
        json j = *this;
        fs::path temp_path = ".tits/objects/commits/temp";
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
        fs::path commit_file_path = normalize_path(".tits/objects/commits/" + hash);
        fs::rename(temp_path, commit_file_path);
    }

public:
    Commit(const string &m) : message(m) {
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
        time = get_timestamp();
        create_commit_file();
        std::ofstream out(file_path, std::ios::trunc);
        if(!out){
            throw std::runtime_error("Failed to create file");
        }
        out << hash;
        out.close();
        cout << "Commit successfully made with:\n";
        cout << "Message: " << message << endl;
        cout << "At: " << time << endl;
        cout << "On branch: " << branch << endl;
    };
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
