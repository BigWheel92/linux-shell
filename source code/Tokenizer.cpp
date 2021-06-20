#ifndef TOKENIZER_CPP
#define TOKENIZER_CPP
#include <iostream>
using namespace std;
#include "Tokenizer.h"

vector<TokenLexemePair>* Tokenizer::tokenizeCommand(string const command)
{
    bool flag = false;
    vector<TokenLexemePair>* command_vector = new vector<TokenLexemePair>;

    int pos = 0;
    while (pos != command.length())
    {
        TokenLexemePair token;
        ignoreWhiteSpaces(command, pos); //ignoring any whitespace before the next token

        flag = isPipe(command, pos, token);

        if (flag == false)
            flag = isArrow(command, pos, token);


        if (flag == false)
            flag = isWord(command, pos, token);


        if (flag == false)
        {
            cout << "Incorrect Command Structure!" << endl << endl;
            return nullptr;
        }

        command_vector->insert(command_vector->end(), token);

    }

    return command_vector;

}

bool Tokenizer::isArrow(const string command, int& pos, TokenLexemePair& token)
{
    bool flag = false;
    if (pos < command.length() && command[pos] == '<' || command[pos] == '>')
    {
        if (command[pos] == '<')
            token.tokenType = TokenType::INPUT_FILE_REDIRECT;
        else token.tokenType = TokenType::OUTPUT_FILE_REDIRECT;

        token.lexeme = "";
        token.lexeme += command[pos];
        ++pos;
        flag = true;
    }
    return flag;
}

void Tokenizer::ignoreWhiteSpaces(const string command, int& pos)
{
    while (pos < command.length() && isspace(command[pos]))
        ++pos;
}


bool Tokenizer::isPipe(const string command, int& pos, TokenLexemePair& token)
{
    bool flag = false;

    if (pos < command.length() && command[pos] == '|')
    {
        ++pos;
        token.lexeme = "|";
        token.tokenType = TokenType::PIPE;
        flag = true;
    }

    return flag;

}

bool Tokenizer::isWord(const string command, int& pos, TokenLexemePair& token)
{
    int state = 0;
    string word = "";

    while (true)
    {
        switch (state)
        {
        case 0:
            if (pos == command.length())
                state = -1;

            else if (command[pos] == '"')
            {
                ++pos;
                state = 3;
            }

            else if (command[pos] == '\\')
            {
                ++pos;
                state = 2;
            }
            else if (command[pos] != '"' && command[pos] != '|' && command[pos] != '<' && command[pos] != '>' && !isspace(command[pos]))
            {
                word += command[pos++];
                state = 1;
            }
            else state = -1;

            break;

        case 1:
            if (pos == command.length())
                state = 5;

            else if (command[pos] == '\\')
            {
                ++pos;
                state = 2;
            }
            else if (command[pos] != '"' && command[pos] != '|' && command[pos] != '<' && command[pos] != '>' && !isspace(command[pos]))
            {
                word += command[pos++];
                state = 1;
            }
            else state = 5;
            break;

        case 2:
            word += command[pos++];
            state = 1;
            break;

        case 3:
            if (command[pos] == '\\')
            {
                pos++;
                state = 6;
            }
            else if (command[pos] == '"')
            {
                pos++;
                state = 5;
            }
            else
            {
                word += command[pos++];
                state = 4;
            }
            break;

        case 4:
            if (command[pos] == '"')
            {
                state = 5;
                pos++;
            }
            else if (command[pos] == '\\')
            {
                state = 6;
                pos++;
            }
            else {
                word += command[pos++];
                state = 4;
            }
            break;

        case 5:
            token.tokenType = TokenType::WORD;
            token.lexeme = word;

            return true;

        case 6:
            word += command[pos++];
            state = 4;
            break;

        case -1:
            return false;
        }
    }
}

#endif