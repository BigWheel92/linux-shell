#ifndef COMMAND_EXECUTER_CPP
#define COMMAND_EXECUTER_CPP
#include "CommandExecuter.h"
#include <unistd.h>
#include <error.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <iostream>
using namespace std;


void CommandExecuter::executeCommand(vector<TokenLexemePair>& command_vector)
{

    if (command_vector[0].lexeme == "cd")
    {
        changeDirectory(command_vector);
        return;
    }

    else
    {
        int i = 0;
        bool write_to_pipe = false;
        int pipe1[2];
        pipe1[0] = -1;
        pipe1[1] = -1;

        int prev_pipe = -1;

        int file_read_fd = -1, file_write_fd = -1;
        char** args = nullptr;

        while (i < command_vector.size())
        {

            if (command_vector[i].tokenType != TokenType::WORD)
            {
                cout << "unxpected syntax near: " << command_vector[i].lexeme << endl;
            }
            else
            {
                string command = command_vector[i].lexeme; //getting the command name

                //building arguments to pass to the command's executable
                ++i;
                list<string>* arguments = buildArguments(command_vector, i);
                args = nullptr;

                int j = 0;
                if (arguments != nullptr)
                {
                    args = new char* [arguments->size() + 2];

                    while (arguments->size())
                    {
                        ++j;
                        args[j] = new char[arguments->front().size() + 1];
                        strcpy(args[j], arguments->front().c_str());
                        arguments->pop_front();

                    }

                    delete arguments;
                }
                else args = new char* [2];

                args[0] = new char[command.size()];
                strcpy(args[0], command.c_str());
                args[j + 1] = nullptr;
                int size = j + 1;

                //checking whether there are any input and output redirection to a file
                while (i < command_vector.size() && (command_vector[i].tokenType == TokenType::INPUT_FILE_REDIRECT || command_vector[i].tokenType == TokenType::OUTPUT_FILE_REDIRECT))
                {

                    if (command_vector[i].tokenType == TokenType::INPUT_FILE_REDIRECT)
                    {
                        string redirectSymbol = command_vector[i].lexeme;
                        ++i;
                        //opening the file
                        if (i < command_vector.size() && command_vector[i].tokenType == TokenType::WORD)
                        {
                            if (file_read_fd != -1)
                                close(file_read_fd);

                            file_read_fd = open(command_vector[i].lexeme.c_str(), O_RDONLY, 0);
                            if (file_read_fd == -1)
                            {
                                string message = command_vector[i].lexeme + ": ";
                                write(2, message.c_str(), message.length());
                                perror(NULL);
                                delete2dArr(args, size); //deleting arguments since command will not be executed.
                                return;
                            }
                            ++i;
                        }
                        else
                        {
                            string message = "Expected a file or directory name after " + redirectSymbol;
                            write(2, message.c_str(), message.length());
                            delete2dArr(args, size); //deleting arguments since command will not be executed.
                            return;
                        }


                    }
                    else if (command_vector[i].tokenType == TokenType::OUTPUT_FILE_REDIRECT)
                    {
                        string redirectSymbol = command_vector[i].lexeme;
                        ++i;
                        //opening the file
                        if (i < command_vector.size() && command_vector[i].tokenType == TokenType::WORD)
                        {
                            if (file_write_fd != -1)
                                close(file_write_fd);

                            file_write_fd = open(command_vector[i].lexeme.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);

                            if (file_write_fd == -1)
                            {
                                string message = command_vector[i].lexeme + ": ";
                                write(2, message.c_str(), message.length());
                                perror(NULL);
                                delete2dArr(args, size); //deleting arguments since command will not be executed.
                                return;
                            }
                            ++i;
                        }
                        else
                        {
                            string message = "Expected a file or directory name after " + redirectSymbol;
                            write(2, message.c_str(), message.length());
                            delete2dArr(args, size); //deleting arguments since command will not be executed.
                            return;
                        }

                    }
                }//end of file input/output redirection loop

                //checking whether there is any output redirection to a pipe for current command
                if (i < command_vector.size() && command_vector[i].tokenType == TokenType::PIPE)
                {

                    int status = pipe(pipe1);
                    if (status == -1)
                    {
                        perror(NULL);
                        delete2dArr(args, size); //deleting arguments since command will not be executed.
                        return;
                    }
                    else
                    {
                        write_to_pipe = true;
                        ++i;
                    }

                }
                else {
                    pipe1[0] = pipe1[1] = -1;
                    write_to_pipe = false;
                }


                int status = fork();
                if (status == -1)
                {
                    perror(NULL);
                    delete2dArr(args, size); //deleting arguments since command will not be executed.
                    return;
                }
                else if (status == 0)
                {

                    if (prev_pipe != -1)
                    {
                        dup2(prev_pipe, 0);
                    }
                    else if (file_read_fd != -1)
                        dup2(file_read_fd, 0);


                    if (write_to_pipe == true)
                    {
                        dup2(pipe1[1], 1);
                    }
                    else if (file_write_fd != -1)
                        dup2(file_write_fd, 1);

                    status = execvp(command.c_str(), args);
                    if (status == -1)
                    {
                        perror(NULL);
                        exit(EXIT_FAILURE);
                    }


                }

                else if (status > 0)
                {
                    if (file_read_fd != -1)
                    {
                        close(file_read_fd);
                        file_read_fd = -1;
                    }

                    if (file_write_fd != -1)
                    {
                        close(file_write_fd);
                        file_write_fd = -1;
                    }

                    int status = 0;

                    wait(&status);

                    if (prev_pipe != -1)
                        close(prev_pipe);

                    if (write_to_pipe == true)
                    {
                        write_to_pipe = false;
                        close(pipe1[1]);
                        prev_pipe = pipe1[0]; //the next command will read from this pipe.
                    }
                    else prev_pipe = -1;

                    delete2dArr(args, size); //deleting arguments
                    args = nullptr;

                }

            }
        }//end of while loop

    }

}


list<string>* CommandExecuter::buildArguments(vector<TokenLexemePair>& command_vector, int& i)
{

    list<string>* args = nullptr;

    if (i < command_vector.size() && command_vector[i].tokenType == TokenType::WORD)
    {
        args = new list<string>;
    }

    while (i < command_vector.size() && command_vector[i].tokenType == TokenType::WORD)
    {
        args->push_back(command_vector[i++].lexeme);
    }
    return args;

}

void CommandExecuter::changeDirectory(vector<TokenLexemePair>& command_vector)
{

    if (command_vector.size() == 1)
    {
        write(2, "cd: too few arguments\n", 22); //writing on standard error
    }

    else if (command_vector.size() > 2)
    {
        write(2, "cd: too many arguments\n", 23); //writing on standard error
    }
    else
    {
        int status = chdir(command_vector[1].lexeme.c_str());
        if (status == -1)
            perror(NULL);

        return;
    }

}//end of changeDirectory function


void CommandExecuter::delete2dArr(char** arr, int const size)
{
    if (arr != nullptr)
    {
        for (int i = 0; i < size; i++)
        {
            delete[]arr[i];
        }

        delete[] arr;
    }
}
#endif