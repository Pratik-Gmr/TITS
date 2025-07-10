#include <iostream>
#include <vector>

#include "parser.cpp"

using namespace std;

int main(int argc, char** argv){
    vector<string> args(argv, argv+argc);
    Command command(args, argc);
    command.parse();
    //Do the programme thingies!!
    
    return 0;
}