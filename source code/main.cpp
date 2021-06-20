#include <iostream>
using namespace std;
#include <string>
#include <unistd.h>
#include <vector>
#include <limits.h>
#include "Tokenizer.h"
#include "CommandExecuter.h"

int main()
{
 string command;
 vector<TokenLexemePair> *command_vector=nullptr;
 char path[PATH_MAX+1];

 while (1)
 {
   getcwd(path, PATH_MAX);
   cout<<path<<":   ";
   getline(cin, command);
   if  ( (command.length()==4 && command=="exit" ) || (command.length()>4 && command[0]=='e' && command[1]=='x' && command[2]=='i' && command[3]=='t' && command[4]==' ') )
	   return 1;

   command_vector=Tokenizer::tokenizeCommand(command);
   CommandExecuter::executeCommand(*command_vector);
   delete command_vector;
   command_vector=nullptr;
   cout<<endl;
 }
 
}
