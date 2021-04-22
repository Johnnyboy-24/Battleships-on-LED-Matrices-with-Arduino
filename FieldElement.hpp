//Definition of the player Class
#include "Ship.hpp"
#ifndef FieldElement_hpp
#define FieldElement_hpp
#include <stdio.h>
class FieldElement {
  public:
    FieldElement();
    unsigned int Status; // Muss noch geschützt werden : getter/setter
    Ship ship; 
    bool preview; 
   
};
#endif
