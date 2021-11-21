#ifndef APPLE_PIE_EXCEPTION_H
#define APPLE_PIE_EXCEPTION_H

#include <string>

enum ExceptionType {UNDEFINED, UNIMPLEMENTED, INVALID_VARNAME, INVALID_FUNC_CALL};

class Exception {

    private:
        std::string message;

    public:
        Exception(const std::string& arg, ExceptionType type) {
            if (type == UNIMPLEMENTED) message = "Sorry, Apple Pie do not implement this.";
            else if (type == UNDEFINED) message = "Undefined Variable: " + arg;
            else if (type == INVALID_FUNC_CALL) message = "Invalid function call: " + arg;
        }    

        std::string what() {return message;}

};

#endif //APPLE_PIE_EXCEPTION_H