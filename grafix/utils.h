#pragma once
#include <glad/glad.h>
#include <iostream>
#include <string>
#include <chrono>
#include <stdexcept>

#define Infinity std::numeric_limits<double>::infinity()

#define rprint(stuff) std::cerr << "\r" << stuff << "          "

inline void print() {
    std::cerr << '\n';
}

template<typename T>
inline void print(T arg) {
    std::cerr << arg << '\n';
}

template<typename T, typename... Rest>
inline void print(T arg, Rest... rest) {
    std::cerr << arg << ' ';
    print(rest...);
}

struct GLcolor {
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;
};

inline double getTime() {
    return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

inline const char *glErrorString(int err) {
#define errcase(x) case x: return #x
switch (err) {
    errcase(GL_NO_ERROR);
    errcase(GL_INVALID_ENUM);
    errcase(GL_INVALID_VALUE);
    errcase(GL_INVALID_OPERATION);
    errcase(GL_INVALID_FRAMEBUFFER_OPERATION);
    errcase(GL_OUT_OF_MEMORY);
    default: return "(unknown)";
}
#undef errcase
}

inline void glCheckImpl(const char *file, int linenum) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
    	throw std::runtime_error(std::string(file) + " line " + std::to_string(linenum) + ": " + glErrorString(err));
    }
}

#define glCheck() glCheckImpl(__FILE__, __LINE__)
