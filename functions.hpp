#pragma once

#include <iostream>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <vector>
#include <unordered_set>

#include "objects.hpp"
#include "indexing.hpp"
#include "branch.hpp"

using namespace std;
namespace fs = filesystem;

void check(const string& hash) {
    fs::path file_path(".tits/objects/commits/" + hash);
    if (!fs::is_regular_file(file_path)){
        cout << "Given hash is not a commit id within this tits directory\n";
        return;
    }
    Commit commit(hash);
    commit.unroll();
    //make head point to detached
    fs::path detached_head_path(".tits/branches/DETACHED");
    std::ofstream detached_head_file(detached_head_path);
    if (!detached_head_file) {
        throw runtime_error("Unexpected Error has occurred while reading the input file while creating a blob\n");
    }
    detached_head_file << hash;
    detached_head_file.close();
    fs::path head_path(".tits/HEAD");
    std::ofstream head_file(head_path);
    if (!head_file) {
        throw runtime_error("Unexpected Error has occurred while reading the input file while creating a blob\n");
    }
    head_file << "DETACHED";
    head_file.close();
    cout << "Now you are in a detached head state\nLearn about it on documentation on official repository\n";
}

bool found_in_current_branchline(const string &current_hash, const string &needed_hash);
bool found_in_current_branchline(const string &current_hash, const string &needed_hash, unordered_set<string>& visited) {
    fs::path commit_path = ".tits/objects/commits/" + current_hash;
    if (!fs::is_regular_file(commit_path)) {
        throw std::runtime_error("Given hash doesn't point to a valid commit object file");
    }
    if(needed_hash == current_hash){
        return true;
    }
    visited.insert(current_hash);
    Commit commit(current_hash);
    vector<string> parents = commit.get_parents();
    if (parents.size() > 2) {
        throw runtime_error("Commit with parents more than 2 found which shouldnt be possible\n");
    }
    for (const auto& parent_hash : parents) {
        if (parent_hash.empty()) continue;
        if (!visited.count(parent_hash))
            if (found_in_current_branchline(parent_hash, needed_hash, visited)) return true;
    }
    return false;
}

bool found_in_current_branchline(const string &current_hash, const string &needed_hash) {
    unordered_set<string> visited;
    return found_in_current_branchline(current_hash,needed_hash,visited);
}

void revert(const string &hash) {
    Branch branch;
    string latest_hash = branch.latest_hash;
    fs::path file_path(".tits/branches/" + branch.name);
    if (!fs::is_regular_file(file_path)) throw runtime_error("WTF HOWWWW");
    if(found_in_current_branchline(latest_hash, hash)) {
        fs::path commit_path = ".tits/objects/commits/" + hash;
        if (!fs::is_regular_file(commit_path)) {
            throw std::runtime_error("Given hash doesn't point to a valid commit object file");
        }
        Commit commit(hash);
        commit.unroll();
        std::ofstream file(file_path);
        if(!file){
            throw std::runtime_error("Failed to open file");
        }
        file << hash;
        file.close();
        cout << "Successfully reverted to state: " << hash << endl;
    }
    else {
        cout << "Given hash is not a commit id within this branch\n";
        return;
    }
}

//needs redesigning
void history() {
    string branch;
    string current_hash;
    fs::path file_path=".tits/HEAD";
    std::ifstream file(file_path);
    if(!file){
        throw std::runtime_error("Failed to create file");
    }
    file >> branch;
    file.close();
    file_path=".tits/branches/" + branch;
    file.open(file_path);
    if(!file){
        throw std::runtime_error("Failed to create file");
    }
    file >> current_hash;
    file.close();
    bool empty_history = true;
    while (current_hash != "") {
        empty_history = false;
        Commit commit(current_hash);
        commit.display();
        vector<string> parents = commit.get_parents();
        if (parents.empty()) {
            return;
        }
        current_hash = parents[0];
    }
    if(empty_history) {
        cout << "No history to show\n";
        return;
    }
}

