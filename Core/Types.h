#ifndef TYPES_H
#define TYPES_H

#include <string>

struct LoadError {
    enum Code {
        FileNotFound,
        DecodingFailed,
        InvalidFormat
    };
    Code code;
    std::string detail;
};

#endif 