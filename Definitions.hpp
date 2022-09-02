#ifndef DEFINITIONS_H
#define  DEFINITIONS_H


extern const unsigned int NZ;
extern const unsigned int NX;
extern const unsigned int NY;

#include <exception>
#include <system_error>
#include <cerrno>

enum ReturnType{
    SUCCESS = 0,
    FAILURE = -1,
};
#endif // !DEFINITIONS_H

