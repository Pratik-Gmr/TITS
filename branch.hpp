#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>

#include "objects.hpp"

using namespace std;
namespace fs = filesystem;

class Branch {
    string name;
    string latest_hash;

public:

    Branch() {
        fs::path file_path=".tits/HEAD";
        std::ifstream file(file_path);
        if(!file){
            throw std::runtime_error("Failed to create file");
        }
        file >> name;
        file.close();
        file_path=".tits/branches/" + name;
        file.open(file_path);
        if(!file){
            throw std::runtime_error("Failed to create file");
        }
        file >> latest_hash;
        file.close();
    }

    void switch_branch(const string& new_branch) {
        if(new_branch == name) {
            cout << "Already on the " << new_branch << endl;
            return;
        }
        if (new_branch == "DETACHED") {
            cout << "Can't change to branch DETACHED\n";
            return;
        }
        string new_branch_hash;
        fs::path new_branch_path(".tits/branches/" + new_branch);
        if (!fs::is_regular_file(new_branch_path)) {
            // create branch of that name
            std::ofstream new_branch_file(new_branch_path);
            if(!new_branch_file) {
                throw runtime_error("New branch file wasnt created\n");
            }
            new_branch_file << latest_hash;
            new_branch_file.close();
            new_branch_hash = latest_hash;
            cout << "New branch " << new_branch << " created\n";
        }
        else {
            std::ifstream new_branch_file(new_branch_path);
            if(!new_branch_file) {
                throw runtime_error("New branch file wasnt created\n");
            }
            new_branch_file >> new_branch_hash;
            new_branch_file.close();
        }
        fs::path head_path(".tits/HEAD");
        ofstream head_file(head_path);
        if(!head_file) {
            throw runtime_error("Head file couldnt open\n");
        }
        head_file << new_branch;
        head_file.close();
        Commit commit(new_branch_hash);
        commit.unroll();
        cout << "Switched to branch " << new_branch << endl;
    }

    void delete_branch(const string& new_branch) {
        if(new_branch == name) {
            cout << "Can't delete the branch you currently are on\n";
            return;
        }
        if (new_branch == "main" || new_branch == "DETACHED") {
            cout << "Can't delete main or DETACHED branches\nEssential branches for tits to work\n";
            return;
        }
        fs::path new_branch_path(".tits/branches/" + new_branch);
        if (!fs::is_regular_file(new_branch_path)) {
            cout << "The said branch doesnt exist\nCan't delete\n";
            return;
        }
        fs::remove(new_branch_path);
        cout << "Branch successfully deleted\n";
    }

};
