#include "error.h"

Error::Error()
    : occured(false), message(string("Nada aconteceu, feijoada")) {}

int Error::set_occurred() {
    this->occured = true;
    return this->occured;
}

int Error::set_not_occurred() {
    this->occured = false;
    return this->occured;
}

int Error::has_occurred() {
    return this->occured;
}

int Error::set_message(string message) {
    this->message = message;
    return this->occured;
}

string Error::get_message() {
    return this->message;
}