//Definition of the player Class
#include "Ship.hpp"
#include "FieldElement.hpp"
#include "Pointer.hpp"
#ifndef Player_hpp
#define Player_hpp
#include <stdio.h>
class Player {  
  public:
    Player();
    FieldElement field [8][8];
    void Placeship();
    bool Checkplaceable(Ship s);
    bool RecieveFire(unsigned int X, unsigned int Y);
    void InitiateField();
    unsigned int shipsplaced;
    Ship ships [4];
    Pointer Mypointer; 
    void UpdateCouser();
    bool allshipsplaced;
};
#endif
