//Definition of the Pointer Class

#ifndef Pointer_hpp
#define Pointer_hpp
#include <stdio.h>
class Pointer {
  public:
    Pointer();
    unsigned int X;
    unsigned int Y;
    unsigned int Orientation;
    void rotate();
};
#endif
