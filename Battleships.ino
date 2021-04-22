#include "Ship.hpp"
#include "FieldElement.hpp"
#include "Player.hpp"
#include "Pointer.hpp"
#include <Adafruit_NeoPixel.h>
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN_1    6
#define LED_PIN_2    7
#define LED_PIN_3    8
#define LED_PIN_4    9

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 64

//The game consists of two game-stages. First: In the placement stage booth players place their ships on their fields.
//Then: The players take turn fireing at the opponents field
#define fireing true
#define placement false
bool gamestage = placement;

// Declare our NeoPixel strip objects:
Adafruit_NeoPixel One_top(LED_COUNT, LED_PIN_1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel One_bottom(LED_COUNT, LED_PIN_2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel Two_top(LED_COUNT, LED_PIN_3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel Two_bottom(LED_COUNT, LED_PIN_4, NEO_GRB + NEO_KHZ800);

// Arduino pin numbers---- Please Check before Use!!!
const int SW_pin_1 = 2; // digital pin connected to switch output-----
const int X_pin_1 = A1; // analog pin connected to X output-----------| PlayerOne_Joystick
const int Y_pin_1 = A2; // analog pin connected to Y output-----------

const int SW_pin_2 = 3; // digital pin connected to switch output-----
const int X_pin_2 = A3; // analog pin connected to X output-----------| PlayerTwo_Joystick
const int Y_pin_2 = A4; // analog pin connected to Y output-----------

//This boolean will be used as a flag for the buttons of player One & Two
bool Joystick_1_pressed = false;
bool Joystick_2_pressed = false;

//Declaration of the different states and their displayed colours
#define Water  0 // light-Blue
#define Boat 1 //Green
#define HitShip 2 //Orange
#define SunkenShip 3 //Red
#define MissedMissle 4 //Blue
#define Courser 5  //Pink

//As we have 4 different LED Matracies we have 4 different Adafruit_NeoPixel objects
//For that reason we unfortunately have to declare each colour used 4 times
#define lightblue One_top.Color (0,150,255)
#define green One_top.Color (0,255,0)
#define orange One_top.Color (255,150,0)
#define red One_top.Color (255,0,0)
#define blue One_top.Color (0,0,255)
#define pink One_top.Color (255,0,255)
#define lightblue1 One_bottom.Color (0,150,255)
#define green1 One_bottom.Color (0,255,0)
#define orange1 One_bottom.Color (255,150,0)
#define red1 One_bottom.Color (255,0,0)
#define blue1 One_bottom.Color (0,0,255)
#define pink1 One_bottom.Color (255,0,255)
#define lightblue2 Two_top.Color (0,150,255)
#define green2 Two_top.Color (0,255,0)
#define orange2 Two_top.Color (255,150,0)
#define red2 Two_top.Color (255,0,0)
#define blue2 Two_top.Color (0,0,255)
#define pink2 Two_top.Color (255,0,255)
#define lightblue3 Two_bottom.Color (0,150,255)
#define green3 Two_bottom.Color (0,255,0)
#define orange3 Two_bottom.Color (255,150,0)
#define red3 Two_bottom.Color (255,0,0)
#define blue3 Two_bottom.Color (0,0,255)
#define pink3 Two_bottom.Color (255,0,255)


//Definition of the possible Pointer_Orientations
#define horizontal 0
#define vertical 1

// Our two players- later they'll be able to contain the login data of a player
Player PlayerOne;
Player PlayerTwo;

// For the "missle-fire" stage of the game we need to keep track which player's turn it is
bool Currentplayer;
#define firstplayer true
#define secondplayer false

//Dislpays which Koordinates the Curser is currently on and in case a ship is to be placed, it also stores the current orientation
Pointer::Pointer() {
  X = 0;
  Y = 0;
  Orientation = horizontal;
}

// The ship initiates with negative lives, so fields without actual ships don't interfere with the game-logic
Ship::Ship() {
  lives = -1;
}

FieldElement::FieldElement() {
  Status = Water;
  Ship ship;
  preview = false;

}

Player::Player() {

  Mypointer = Pointer();
  InitiateField();
  shipsplaced = 0;
  ships[0] = Ship();
  ships[0].lives = 2;
  ships[1] = Ship();
  ships[1].lives = 3;
  ships[2] = Ship();
  ships[2].lives = 3;
  ships[3] = Ship();
  ships[3].lives = 4;
  allshipsplaced = false;

}
bool Player::Checkplaceable(Ship s) {
  //To be implemented
  for (int i = 0; i < s.lives; i++) {
    if (Mypointer.Orientation == horizontal) {
      if ((field[Mypointer.Y][Mypointer.X + i]).Status != Water)
      {
        return false;
      }
    }
    if (Mypointer.Orientation == vertical) {
      if ((field[Mypointer.Y + i][Mypointer.X]).Status != Water)
      {
        return false;
      }
    }
  }
  return true;


}
void Player::Placeship() {
  //To be implemented
  if (shipsplaced > 3) {
    allshipsplaced = true;
    UpdateCouser();
    return;
  }

  else if (Checkplaceable(ships[shipsplaced]))
  {
    for (int i = 0; i < ships[shipsplaced].lives; i++) {

      if (Mypointer.Orientation == horizontal) {
        (field[Mypointer.Y][Mypointer.X + i]).ship = ships[shipsplaced];
        (field[Mypointer.Y][Mypointer.X + i]).Status = Boat;
      }
      if (Mypointer.Orientation == vertical) {
        (field[Mypointer.Y + i][Mypointer.X]).ship = ships[shipsplaced];
        (field[Mypointer.Y + i][Mypointer.X ]).Status = Boat;
      }
    }
    shipsplaced++;
    if (shipsplaced > 3)
    {
      allshipsplaced = true;
      UpdateCouser();

    }
  }
}



// Checks if the given field has benn fired on- if not it changes its Status
bool Player::RecieveFire(unsigned int X, unsigned int Y) {

  if ((field[Y][X]).Status == Boat) {
    if ((field[Y][X]).ship.lives > 0) {
      (field[Y][X]).Status = HitShip;
      (field[Y][X]).ship.lives --;
      return true;
    }
  }
  return false;
}

void Player::InitiateField() {
  for (int a = 0; a < 8; a++) {
    for (int b = 0; b < 8; b++) {
      field[a][b] = FieldElement();
    }
  }
}


// This function sets a boolean on the field elements.
// If "allshipsplaced" is false, the game will be able to not only disply the Pointer/Courser,
// but the entire ship a player is about to place(Only during ship placement)
void Player::UpdateCouser() {

  for (int a = 0; a < 8; a++) {
    for (int b = 0; b < 8; b++) {
      field[a][b].preview = false;
    }
  }

  if (!allshipsplaced) {

    for (int i = 0; i < ships[shipsplaced].lives; i++) {

      if (Mypointer.Orientation == horizontal) {
        (field[Mypointer.Y][Mypointer.X + i]).preview = true;
      }
      if (Mypointer.Orientation == vertical) {
        (field[Mypointer.Y + i][Mypointer.X ]).preview = true;
      }
    }
  }
  else {
    (field[Mypointer.Y][Mypointer.X]).preview = true;
  }

}

void changeplayer() {
  if (Currentplayer == firstplayer) {
    Currentplayer = secondplayer;
  }
  else {
    Currentplayer = firstplayer;
  }
}

//This function is meant for debugging only and prints The status of each field-element of the players via Serial Monitor
/*void updateboards() {
  sinkships()
  Serial.println();
  Serial.println("Player 1:");
  Serial.println(PlayerOne.shipsplaced);
  for (int i = 0; i < 8; i++) {
    Serial.println();
    for (int u = 0; u < 8; u++) {
      if ((PlayerOne.field[i][u]).preview) {
        Serial.print(Courser);
      }
      else {
        Serial.print((PlayerOne.field[i][u]).Status);
      }
    }
  }
  Serial.println();
  Serial.println("Player 2:");
  for (int i = 0; i < 8; i++) {
    Serial.println();
    for (int u = 0; u < 8; u++) {
      if ((PlayerTwo.field[i][u]).preview) {
        Serial.print(Courser);
      }
      else {
        Serial.print((PlayerTwo.field[i][u]).Status);
      }

    }
  }
  }
*/
void Pointer :: rotate()
{
  if (Orientation == horizontal)
  {
    Orientation = vertical;
  }
  else {
    Orientation = horizontal;
  }
}

bool anotherturn;
// First this function check, wether a button was pressed. Then it checks in what stage the game is(ship placement/ Misslefireing) and then either places/rotates a ships or fires
void Checkpressed()
{
  if (Joystick_1_pressed || Joystick_2_pressed)
  {
    delay(500);
    if (gamestage == fireing)
    {
      if (Joystick_1_pressed && (Currentplayer == firstplayer))
        anotherturn = PlayerTwo.RecieveFire(PlayerOne.Mypointer.X, PlayerOne.Mypointer.Y);
      if (Joystick_2_pressed && (Currentplayer == secondplayer))
        anotherturn = PlayerOne.RecieveFire(PlayerTwo.Mypointer.X, PlayerTwo.Mypointer.Y);
      if (anotherturn) {
        return;
      }
      else {
        changeplayer();
      }
    }
    else
    {

      if (digitalRead(SW_pin_1) == 0) {
        PlayerOne.Placeship();
      }
      else if (Joystick_1_pressed) {
        PlayerOne.Mypointer.rotate();
      }
      if (digitalRead(SW_pin_2) == 0) {
        PlayerTwo.Placeship();
      }
      else if (Joystick_2_pressed) {
        PlayerTwo.Mypointer.rotate();
      }
    }
  }
  Joystick_1_pressed = false;
  Joystick_2_pressed = false;

}

//ISR triggered by a pressed button: This Routine sets the "Joystickpressed" Flag
void ISR_1() {

  Joystick_1_pressed = true;
}
void ISR_2() {

  Joystick_2_pressed = true;
}

//This function iterates over the fields of both players and sets the status to 3 if a ship on that field has 0 lives.
void sinkships() {
  for (int i = 0; i < 8; i++) {
    for (int u = 0; u < 8; u++) {
      if ((PlayerOne.field[i][u].ship.lives) == 0) {
        (PlayerOne.field[i][u]).Status = SunkenShip;
      }
    }
  }
  for (int i = 0; i < 8; i++) {
    for (int u = 0; u < 8; u++) {
      if ((PlayerTwo.field[i][u].ship.lives) == 0) {
        (PlayerTwo.field[i][u]).Status = SunkenShip;
      }
    }
  }

}

//This function will update the Pixel-colours of the matracies- currently only 1 matrix, for player one
void updatepixels()
{
  sinkships();
  for (int i = 0; i < 8 ; i++) {
    for (int u = 0; u < 8 ; u++) {


      if ((PlayerOne.field[i][u]).preview && gamestage) {
        One_top.setPixelColor(i * 8 + u, pink1);
      }
      else {
        switch ((PlayerTwo.field[i][u]).Status) {
          case 2: One_top.setPixelColor(i * 8 + u, orange1); break;
          case 3: One_top.setPixelColor(i * 8 + u, red1); break;
          case 4: One_top.setPixelColor(i * 8 + u, blue1); break;
          default: One_top.setPixelColor(i * 8 + u, lightblue1); break;

        }
      }
      if ((PlayerTwo.field[i][u]).preview && gamestage) {
        Two_top.setPixelColor(i * 8 + u, pink1);
      }
      else {
        switch ((PlayerOne.field[i][u]).Status) {
          case 2: Two_top.setPixelColor(i * 8 + u, orange1); break;
          case 3: Two_top.setPixelColor(i * 8 + u, red1); break;
          case 4: Two_top.setPixelColor(i * 8 + u, blue1); break;
          default: Two_top.setPixelColor(i * 8 + u, lightblue1); break;

        }
      }
      if ((PlayerOne.field[i][u]).preview && (!PlayerOne.allshipsplaced)) {
        One_bottom.setPixelColor(i * 8 + u, pink1);
      }
      else {
        switch ((PlayerOne.field[i][u]).Status) {
          case 1: One_bottom.setPixelColor(i * 8 + u, green1); break;
          case 2: One_bottom.setPixelColor(i * 8 + u, orange1); break;
          case 3: One_bottom.setPixelColor(i * 8 + u, red1); break;
          case 4: One_bottom.setPixelColor(i * 8 + u, blue1); break;
          default: One_bottom.setPixelColor(i * 8 + u, lightblue1); break;

        }
      }
      if ((PlayerTwo.field[i][u]).preview && (!PlayerTwo.allshipsplaced)) {
        Two_bottom.setPixelColor(i * 8 + u, pink3);
      }
      else {
        switch ((PlayerTwo.field[i][u]).Status) {

          case 1: Two_bottom.setPixelColor(i * 8 + u, green3); break;
          case 2: Two_bottom.setPixelColor(i * 8 + u, orange3); break;
          case 3: Two_bottom.setPixelColor(i * 8 + u, red3); break;
          case 4: Two_bottom.setPixelColor(i * 8 + u, blue3); break;
          case 5: Two_bottom.setPixelColor(i * 8 + u, pink3); break;
          default: Two_bottom.setPixelColor(i * 8 + u, lightblue3); break;
        }
      }

    }
  }
  One_bottom.show();
  Two_bottom.show();
  One_top.show();
  Two_top.show();
}

// Move the Pointer around the y-axis and the X-axis
int temp_X_1;
int temp_Y_1;
int temp_X_2;
int temp_Y_2;


void updatepointers()
{
  temp_X_1 = analogRead(X_pin_1);
  temp_Y_1 = analogRead(Y_pin_1);

  if (temp_X_1 < 150) {
    if (PlayerOne.Mypointer.X > 0) {
      PlayerOne.Mypointer.X--;
    }
  }
  if (temp_X_1 > 850) {
    if (PlayerOne.Mypointer.X < 7) {
      PlayerOne.Mypointer.X++;
    }
  }
  if (temp_Y_1 < 150) {
    if (PlayerOne.Mypointer.Y > 0) {
      PlayerOne.Mypointer.Y--;
    }
  }
  if (temp_Y_1 > 850) {
    if (PlayerOne.Mypointer.Y < 7) {
      PlayerOne.Mypointer.Y++;
    }
  }

  temp_X_2 = analogRead(X_pin_2);
  temp_Y_2 = analogRead(Y_pin_2);

  if (temp_X_2 < 150) {
    if (PlayerTwo.Mypointer.X > 0) {
      PlayerTwo.Mypointer.X--;
    }
  }
  if (temp_X_2 > 850) {
    if (PlayerTwo.Mypointer.X < 7) {
      PlayerTwo.Mypointer.X++;
    }
  }
  if (temp_Y_2 < 150) {
    if (PlayerTwo.Mypointer.Y > 0) {
      PlayerTwo.Mypointer.Y--;
    }
  }
  if (temp_Y_2 > 850) {
    if (PlayerTwo.Mypointer.Y < 7) {
      PlayerTwo.Mypointer.Y++;
    }
  }
  delay(250);
}

void checkallshipsplaced() {
  if (PlayerOne.allshipsplaced & PlayerTwo.allshipsplaced)
    gamestage = fireing;
}

void setup() {
  // put your setup code here, to run once:
  attachInterrupt(digitalPinToInterrupt(SW_pin_1), ISR_1, FALLING );
  attachInterrupt(digitalPinToInterrupt(SW_pin_2), ISR_2, FALLING );
  pinMode(SW_pin_1, INPUT);
  pinMode(SW_pin_2, INPUT);
  digitalWrite(SW_pin_1, HIGH);
  digitalWrite(SW_pin_2, HIGH);
  // Serial.begin(9600); //Only for use of the serial monitor/ testing. Comment this line for use of Pixels
  Currentplayer = firstplayer;
  One_top.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  One_top.show();
  One_top.setBrightness(2); // Set BRIGHTNESS to about 1/5 (max = 255)
  One_bottom.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  One_bottom.show();
  One_bottom.setBrightness(2); // Set BRIGHTNESS to about 1/5 (max = 255)
  Two_top.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  Two_top.show();
  Two_top.setBrightness(2); // Set BRIGHTNESS to about 1/5 (max = 255)
  Two_bottom.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  Two_bottom.show();
  Two_bottom.setBrightness(2); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void loop() {
  // put your main code here, to run repeatedly:
  updatepointers();
  checkallshipsplaced();
  PlayerOne.UpdateCouser();
  PlayerTwo.UpdateCouser();
  //  updateboards();  //Only for use of the serial monitor/ testing. Comment this line for use of Pixels
  updatepixels();
  Checkpressed();
  Serial.println(PlayerTwo.field[0][0].ship.lives);


  //delay(1000); //Only for use of the serial monitor/ testing. Comment this line for use of Pixels
}
