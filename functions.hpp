#pragma once

#include <iostream>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <functional>

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

void current_branch_history(const string &current_hash, unordered_set<string> &visited, vector<Commit> &commits);
vector<Commit> current_branch_history(const string &current_hash) {
    unordered_set<string> visited;
    vector<Commit> commits;
    current_branch_history(current_hash, visited, commits);
    return commits;
}

void current_branch_history(const string &current_hash, unordered_set<string> &visited, vector<Commit> &commits) {
    if (visited.count(current_hash)) return;
    Commit commit(current_hash);
    commits.push_back(commit);
    visited.insert(current_hash);
    vector<string> parents = commit.get_parents();
    if (parents.size() > 2) {
        throw runtime_error("Commit with parents more than 2 found which shouldnt be possible\n");
    }
    for (const auto& parent_hash : parents) {
        if (parent_hash.empty()) continue;
        current_branch_history(parent_hash, visited, commits);
    }
}

vector<Commit> all_branch_history(){
    queue<string> branch_heads_hash;
    fs::path branches(".tits/branches/");
    for (const auto& branch_name : fs::directory_iterator(branches)) {
        if(is_regular_file(branch_name)) {
            if(branch_name.path().filename() != "DETACHED") {
                string hash;
                ifstream file(branch_name.path());
                if (!file) throw runtime_error("Failed to open branch file\n");
                file >> hash;
                file.close();
                branch_heads_hash.push(hash);
            }
        }
    }
    unordered_set<string> visited;
    vector<Commit> commits;
    while (!branch_heads_hash.empty()) {
        string current_hash = branch_heads_hash.front();
        branch_heads_hash.pop();
        current_branch_history(current_hash, visited, commits);
    }
    return commits;
}

vector<Commit> all_commits_history(){
    vector<Commit> commits;
    fs::path commits_directory(".tits/objects/commits/");
    for (const auto& commit_file : fs::directory_iterator(commits_directory)) {
        if (is_regular_file(commit_file)) {
            string hash = commit_file.path().filename().string();
            Commit commit(hash);
            commits.push_back(commit);
        }
    }
    return commits;
}

//needs redesigning
void history(const string& flag) {
    if (flag != "-a" && flag != "-b" && flag != "-c") {
        cout << "History only supports flags -a, -b, and -c\n";
        cout << "Do command 'tits history -a' for viewing history of all branches currently existing in the tits directory\n";
        cout << "Do command 'tits history -b' for viewing history of current branch only\n";
        cout << "Do command 'tits history -c' for viewing history if all the commits within this tits directory\n";
        return;
    }
    Branch branch;
    string current_hash = branch.latest_hash;
    fs::path file_path(".tits/branches/" + branch.name);
    if (current_hash == "") {
        cout << "No history to show\n";
        return;
    }
    vector<Commit> commits;
    if(flag == "-a") {
        commits = all_branch_history();
    }
    else if(flag == "-b") {
        commits = current_branch_history(current_hash);
    }
    else if(flag == "-c") {
        commits = all_commits_history();
    }
    sort(commits.begin(), commits.end(), greater<>()); //sorts in descending order by timestamp - latest to oldest
    for (auto& commit : commits) {
        commit.display();
    }
}

