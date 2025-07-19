#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>

#include "objects.hpp"

using namespace std;
namespace fs = filesystem;

vector<files> diff_betwn_commits(Commit start, Commit end);

class Branch {
    string name;
    string latest_hash;

friend void revert(const string &hash);
friend void history(const string& flag);

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

    void merge(const string& branch_name) {
        if(branch_name == "main" || branch_name == "DETACHED") {
            cout << "You cannot merge main or DETACHED to other branches\nDETACHED doesnt support merge at all while u can only merge other branches *to* main\n";
            return;
        }
        fs::path branch_path(".tits/branches/" + branch_name);
        if(!fs::is_regular_file(branch_path)) {
            cout << "There exists no " << branch_name << " branch to merge\n";
            return;
        }
        std::ifstream branch_file(branch_path);
        if(!branch_file) {
            throw runtime_error("Couldnt open branch file\n");
        }
        string branch_hash;
        branch_file >> branch_hash;
        Commit source(branch_hash);
        Commit destination(latest_hash);
        Commit common(find_common_ancestor(source, destination));
        //gotta find 3 way diffs betwn source, destination and common
        vector<files> changes_in_source = diff_betwn_commits(common, source);
        vector<files> changes_in_destination = diff_betwn_commits(common, destination);
        destination.unroll();
        for (auto& file : changes_in_source) {
            fs::path file_path(file.filepath);
            for (auto& change : changes_in_destination) {
                if (file.filepath == change.filepath && file.hash != change.hash) {
                    fs::path before = file.filepath.parent_path();
                    fs::path after = file.filepath.filename();
                    after = "(" + branch_name + ")" + after.string();
                    file_path = before / after;
                }
            }
            Blob blob(file.hash);
            blob.unroll(file_path);
        }
        string all(".");
        string time = get_timestamp();
        Indexer i;
        i.add(all);
        Commit commit(branch_name + " merged into " + name, time, branch_hash);
        delete_branch(branch_name);
    }

};

vector<files> diff_betwn_commits(Commit start, Commit end) {
    string start_tree_hash = start.get_tree_hash();
    Tree start_tree(start_tree_hash);
    vector<files> start_items = start_tree.get_items();
    string end_tree_hash = end.get_tree_hash();
    Tree end_tree(end_tree_hash);
    vector<files> end_items = end_tree.get_items();
    vector<files> changes;
    for (const auto& end_item : end_items) {
        bool not_found = true;
        for (const auto& start_item : start_items) {
            if (end_item.filepath == start_item.filepath) {
                not_found = false;
                if (end_item.hash != start_item.hash) {
                    changes.push_back(end_item);
                }
                break;
            }
        }
        if (not_found) changes.push_back(end_item);
    }
    return changes;
}

