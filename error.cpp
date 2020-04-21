#include "error.h"

Error::Error()
    : occured(false), message(string("Nada aconteceu, feijoada")) {}

void Error::set_occurred() {
    this->occured = true;
}

void Error::set_not_occurred() {
    this->occured = false;
}

int Error::has_occurred() {
    return this->occured;
}

void Error::set_message(string message) {
    this->message = message;
}

string Error::get_message() {
    return this->message;
}