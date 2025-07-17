#include <iostream>
#include <vector>

using namespace std;

enum class Command{
    init,
    log,
    history,
    ignore,
    add,
    remove,
    commit,
    check,
    revert,
    branch,
    merge,
    invalid
};

class Parser{
private:
    vector<string> tokens;
    int token_size;
public:
    Parser() = default;
    Parser(vector<string> &cmd,int n):tokens(cmd),token_size(n){};
    ~Parser() = default;
    Command parse(){
        //does parsing
        if(token_size == 2){
            if(tokens[1] == "init"){
                return Command::init;
            }
            else if(tokens[1] == "log"){
                return Command::log;
            }
            else if(tokens[1] == "history"){
                return Command::history;
            }
        }
        else if(token_size == 3){
            if(tokens[1] == "ignore"){
                return Command::ignore;
            }
            else if(tokens[1] == "add"){
                return Command::add;
            }
            else if(tokens[1] == "remove"){
                return Command::remove;
            }
            else if(tokens[1] == "commit"){
                return Command::commit;
            }
            else if(tokens[1] == "check"){
                return Command::check;
            }
            else if(tokens[1] == "revert"){
                return Command::revert;
            }
            else if(tokens[1] == "branch"){
                return Command::branch;
            }
            else if(tokens[1] == "merge"){
                return Command::merge;
            }
        }
        return Command::invalid;
    }
};
