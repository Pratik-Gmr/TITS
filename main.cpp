#include <iostream>
#include <vector>

#include "functions.hpp"
#include "indexing.hpp"
#include "initializer.hpp"
#include "objects.hpp"
#include "parser.hpp"
#include "branch.hpp"

using namespace std;

int main(int argc, char **argv) {
    try {
        vector<string> args(argv, argv + argc);
        Command command;
        Parser parser(args, argc);
        command = parser.parse();
        Initializer Init;
        switch (command) {
            case Command::init: {
                // initialization
                if (Init.getStatus()) {
                    cout << "This is already a TITS Directory.\n";
                    cout << "Re-initializing this directory will result in all your previous "
                        "progress being lost\n";
                    cout << "If you still wish to re-initialize this directory as a TITS "
                        "directory delete the '.tits' folder and try the 'tits init' "
                        "command again\n";
                    break;
                }
                Init.init();
                if (Init.getStatus())
                    cout << "The directory has been initialized to a TITS directory\n";
                else
                    cout << "Some unexpected Error has occurred\n";
                break;
            }
            case Command::log: {
                // shows staging area and files that has been set to be ignored by TITS
                if (!Init.getStatus()) {
                    cout
                        << "This is not a TITS directory\nFirst initialize this to a TITS "
                        "directory with 'tits init' for any other tits commands to work\n";
                    break;
                }
                if (Init.is_detached()) {
                    cout << "You are in a detached head state.\nUse 'tits branch "
                        "<branch_name>' to either switch to an existing branch or create "
                        "a new branch and switch to it to get out of this state\n";
                    break;
                }
                Indexer Staging_area;
                Staging_area.log();
                break;
            }
            // below these are TITS commands that take 3 inputs : TITS <cmmnd> <arg>
            case Command::ignore: {
                // to ignore said files/directories by the TITS
                if (!Init.getStatus()) {
                    cout
                        << "This is not a TITS directory\nFirst initialize this to a TITS "
                        "directory with 'tits init' for any other tits commands to work\n";
                    break;
                }
                if (Init.is_detached()) {
                    cout << "You are in a detached head state.\nUse 'tits branch "
                        "<branch_name>' to either switch to an existing branch or create "
                        "a new branch and switch to it to get out of this state\n";
                    break;
                }
                Indexer Staging_area;
                Staging_area.to_ignore(args[2]);
                break;
            }
            case Command::add: {
                // adds files/directories to staging area
                if (!Init.getStatus()) {
                    cout
                        << "This is not a TITS directory\nFirst initialize this to a TITS "
                        "directory with 'tits init' for any other tits commands to work\n";
                    break;
                }
                if (Init.is_detached()) {
                    cout << "You are in a detached head state.\nUse 'tits branch "
                        "<branch_name>' to either switch to an existing branch or create "
                        "a new branch and switch to it to get out of this state\n";
                    break;
                }
                Indexer Staging_area;
                Staging_area.add(args[2]);
                break;
            }
            case Command::remove: {
                // to remove certain file from staging area
                if (!Init.getStatus()) {
                    cout
                        << "This is not a TITS directory\nFirst initialize this to a TITS "
                        "directory with 'tits init' for any other tits commands to work\n";
                    break;
                }
                if (Init.is_detached()) {
                    cout << "You are in a detached head state.\nUse 'tits branch "
                        "<branch_name>' to either switch to an existing branch or create "
                        "a new branch and switch to it to get out of this state\n";
                    break;
                }
                Indexer Staging_area;
                Staging_area.remove(args[2]);
                break;
            }
            case Command::commit: {
                // commits the files/diretories in staging area
                if (!Init.getStatus()) {
                    cout
                        << "This is not a TITS directory\nFirst initialize this to a TITS "
                        "directory with 'tits init' for any other tits commands to work\n";
                    break;
                }
                if (Init.is_detached()) {
                    cout << "You are in a detached head state.\nUse 'tits branch "
                        "<branch_name>' to either switch to an existing branch or create "
                        "a new branch and switch to it to get out of this state\n";
                    break;
                }
                string time = get_timestamp();
                Commit commit(args[2], time);
                break;
            }
            case Command::check: {
                // to check out an specific commit
                if (!Init.getStatus()) {
                    cout
                        << "This is not a TITS directory\nFirst initialize this to a TITS "
                        "directory with 'tits init' for any other tits commands to work\n";
                    break;
                }
                check(args[2]);
                break;
            }
            case Command::revert: {
                // to revert back to a certain commit
                if (!Init.getStatus()) {
                    cout
                        << "This is not a TITS directory\nFirst initialize this to a TITS "
                        "directory with 'tits init' for any other tits commands to work\n";
                    break;
                }
                if (Init.is_detached()) {
                    cout << "You are in a detached head state.\nUse 'tits branch "
                        "<branch_name>' to either switch to an existing branch or create "
                        "a new branch and switch to it to get out of this state\n";
                    break;
                }
                revert(args[2]);
                break;
            }
            case Command::branch: {
                // to create and switch to a new branch or switch to existing branch
                if (!Init.getStatus()) {
                    cout
                        << "This is not a TITS directory\nFirst initialize this to a TITS "
                        "directory with 'tits init' for any other tits commands to work\n";
                    break;
                }
                Branch branch;
                branch.switch_branch(args[2]);
                break;
            }
            case Command::delete_: {
                // to merge current branch to aother branch
                if (!Init.getStatus()) {
                    cout
                        << "This is not a TITS directory\nFirst initialize this to a TITS "
                        "directory with 'tits init' for any other tits commands to work\n";
                    break;
                }
                if (Init.is_detached()) {
                    cout << "You are in a detached head state.\nUse 'tits branch "
                        "<branch_name>' to either switch to an existing branch or create "
                        "a new branch and switch to it to get out of this state\n";
                    break;
                }
                Branch branch;
                branch.delete_branch(args[2]);
                break;
            }
            case Command::merge: {
                // to merge current branch to aother branch
                if (!Init.getStatus()) {
                    cout
                        << "This is not a TITS directory\nFirst initialize this to a TITS "
                        "directory with 'tits init' for any other tits commands to work\n";
                    break;
                }
                if (Init.is_detached()) {
                    cout << "You are in a detached head state.\nUse 'tits branch "
                        "<branch_name>' to either switch to an existing branch or create "
                        "a new branch and switch to it to get out of this state\n";
                    break;
                }
                Branch branch;
                branch.merge(args[2]);
                break;
            }
            case Command::history: {
                // shows all the commit history from the initialization of the TITS
                // directory
                if (!Init.getStatus()) {
                    cout
                        << "This is not a TITS directory\nFirst initialize this to a TITS "
                        "directory with 'tits init' for any other tits commands to work\n";
                    break;
                }
                if (Init.is_detached()) {
                    cout << "You are in a detached head state.\nUse 'tits branch "
                        "<branch_name>' to either switch to an existing branch or create "
                        "a new branch and switch to it to get out of this state\n";
                    break;
                }
                history(args[2]);
                break;
            }
            case Command::invalid: {
                // wrong command case
                cout << "Given command doesnt exist please consult TITS documentation on "
                    "its github repository"
                    << endl;
                break;
            }
        }
    }
    catch (const runtime_error& e) {
        if (typeid(e) == typeid(runtime_error)) {
            cout << e.what();
        } else {
            throw;
        }
    }
    return 0;
}
