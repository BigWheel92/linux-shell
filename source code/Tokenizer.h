#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <vector>

enum TokenType
{
  WORD,
  PIPE,
  INPUT_FILE_REDIRECT,
  OUTPUT_FILE_REDIRECT
};

struct TokenLexemePair
{
  int tokenType;
  string lexeme;
};

class Tokenizer
{

public:
  static  vector<TokenLexemePair>*  tokenizeCommand(string const command);
  
private:
  static bool isWord(const std::string command, int & pos, TokenLexemePair &token);
  static bool isPipe(const std::string  command, int& pos, TokenLexemePair  &token);
  static bool isArrow(const std::string command, int& pos, TokenLexemePair  &token);
  static void ignoreWhiteSpaces(const std::string  command, int & pos);
  
};


#endif
