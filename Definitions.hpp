#ifndef DEFINITIONS_H
#define  DEFINITIONS_H


extern const unsigned int NZ;
extern const unsigned int NX;
extern const unsigned int NY;

extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

#include <exception>
#include <system_error>
#include <cerrno>

#include "ExternalSrc/Include/glm/gtc/matrix_transform.hpp"

enum ReturnType{
    SUCCESS = 0,
    FAILURE = -1,
};
#endif // !DEFINITIONS_H

