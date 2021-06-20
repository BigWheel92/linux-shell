#ifndef COMMAND_EXECUTER_H
#define COMMAND_EXECUTER_H
#include <vector>
#include <string>
#include <list>
using std::vector;
using std::string;
using std::list;
#include "Tokenizer.h"

class CommandExecuter
{


  public:
  static void executeCommand(vector<TokenLexemePair> & command_vector);
  
  private:
	static void start(vector<TokenLexemePair> & command_vector, int &pos);
  static void changeDirectory(vector<TokenLexemePair> &command_vector);
  static void executeCurrentCommand(vector<TokenLexemePair> & command_vector, int i, bool readFromPipe);
  static list<string>* buildArguments(vector<TokenLexemePair> & command_vector, int &i);
  static void delete2dArr(char **arr, int const size);
  
};

#endif
