#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include <string>
using namespace std;

class Error {
    private:
        bool occured;
        string message;

    public:
        Error();
        int set_occurred();
        int has_occurred();

        int set_message(string message);
        string get_message();
};

#endif