#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include <string>
using namespace std;

//Classe para captura e tratamento de erros
class Error {
    private:
        bool occured;
        string message;

    public:
        Error();
        
        /*
         * Marca a ocorrência de um erro
         */
        void set_occurred();
        
        /*
         * Marca a não ocorrência de um erro
         */
        void set_not_occurred();
        
        /*
         * Indica se ocorreu ou não um erro
         * 
         * @return error->occured, true se houve um erro, false caso contrário
         */
        int has_occurred();

        /*
         * Setter da mensagem de erro
         */
        void set_message(string message);

        /*
         * Getter da mensagem de erro
         * @return error->message 
         */
        string get_message();
};

#endif