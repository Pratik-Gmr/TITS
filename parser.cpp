#include <iostream>
#include <vector>

using namespace std;

class Command{
    private:
        vector<string> tokens;
        int token_size;
    public:
        Command() = default;
        Command(vector<string> &cmd,int n):tokens(cmd),token_size(n){};
        ~Command() = default;
        void parse(){
            //does parsing
            if(token_size == 2){
                if(tokens[1] == "init"){                    
                    //initialization
                    return;
                }
                else if(tokens[1] == "log"){
                    //display staging area
                    return;
                }
                else if(tokens[1] == "history"){
                    //display commit history
                    return;
                }
            }
            else if(token_size == 3){
                if(tokens[1] == "commit"){
                    //commit the staging area files
                    return;
                }
                else if(tokens[1] == "add"){
                    //add files to staging area
                    return;
                }
                else if(tokens[1] == "check"){
                    //check that commit
                    return;
                }
                else if(tokens[1] == "revert"){
                    //revert to that commit point
                    return;
                }
                else if(tokens[1] == "remove"){
                    //remove files from staging area
                    return;
                }
            }
            cout << "Given command doesnt exist please consult TITS documentation on its github repository" << endl;
        }
};