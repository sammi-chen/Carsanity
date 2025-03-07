//defining pins
const int piezoPin = 5;
const int rightLight = 2;
const int leftLight = 3;

#include <LiquidCrystal.h>  // LCD
//Motor things//
#include <Arduino.h>
#include <stdint.h>
#include "SCMD.h"
#include "SCMD_config.h" //Contains #defines for common SCMD register names and values
#include "Wire.h"

SCMD myMotorDriver; //This creates the main object of one motor driver and connected peripherals.

// define LCD pins
LiquidCrystal lcd(13, 12, 11, 6, 9, 7);

// movie list and critic ratings
String movies [] = {
  "Toy Story", "A Bug's Life", "Toy Story 2", "Monsters, Inc.", "Finding Nemo", 
  "The Incredibles", "Cars", "Ratatouille", "WALL-E", "Up", "Toy Story 3", 
  "Cars 2", "Brave", "Monsters University", "Inside Out", "The Good Dinosaur", 
  "Finding Dory", "Cars 3", "Coco", "Incredibles 2", "Toy Story 4", "Onward", 
  "Soul", "Luca", "Turning Red", "Lightyear", "Elemental", "Inside Out 2"
};

// critic ratings for each movie
String ratings [] = {
  "100%", "92%", "100%", "96%", "99%", "97%", "75%", "96%", "95%", "98%", 
  "98%", "40%", "79%", "80%", "98%", "75%", "94%", "69%", "97%", "93%", 
  "97%", "88%", "95%", "91%", "95%", "74%", "73%", "91%"
};

// Variables
int lastPotValue = 0;  // store the last potentiometer value
int movieIndex = 0;  // current movie index
const int movieCount = sizeof(movies) / sizeof(movies[0]);  // number of movies

void setup() {
  lcd.begin(16, 2);  // initialize the LCD with 16 columns and 2 rows
  lcd.clear();  // clear the screen
  lcd.setCursor(0, 0);  // set the cursor to the start
  ///Motor Things////
   pinMode(8, INPUT_PULLUP); //Use to halt motor movement (ground)

  Serial.begin(9600);
  Serial.println("Starting sketch.");

  //***** Configure the Motor Driver's Settings *****//
  //  .commInter face is I2C_MODE 
  myMotorDriver.settings.commInterface = I2C_MODE;

  //  set address if I2C configuration selected with the config jumpers
  myMotorDriver.settings.I2CAddress = 0x5D; //config pattern is "1000" (default) on board for address 0x5D

  //  set chip select if SPI selected with the config jumpers
  myMotorDriver.settings.chipSelectPin = 10;

  //*****initialize the driver get wait for idle*****//
  while ( myMotorDriver.begin() != 0xA9 ) //Wait until a valid ID word is returned
  {
    Serial.println( "ID mismatch, trying again" );
    delay(500);
  }
  Serial.println( "ID matches 0xA9" );

  //  Check to make sure the driver is done looking for peripherals before beginning
  Serial.print("Waiting for enumeration...");
  while ( myMotorDriver.ready() == false );
  Serial.println("Done.");
  Serial.println();

  //*****Set application settings and enable driver*****//

  //while( myMotorDriver.busy() );
  //myMotorDriver.inversionMode(0, 1); //invert motor 0

  //Uncomment code for motor 1 inversion
  while ( myMotorDriver.busy() ); //Waits until the SCMD is available.
  myMotorDriver.inversionMode(1, 1); //invert motor 1

  while ( myMotorDriver.busy() );
  myMotorDriver.enable(); //Enables the output driver hardware
  ///End of Motor Things///
  ////add other prins please
  pinMode(piezoPin, OUTPUT);
  pinMode(rightLight, OUTPUT);
  pinMode(leftLight, OUTPUT);
}

//more motor things
#define LEFT_MOTOR 0
#define RIGHT_MOTOR 1

void loop() {
  int potValue = analogRead(A0);  // read the potentiometer value (0 to 1023)

  // Map potentiometer value to movie index
  movieIndex = map(potValue, 0, 1023, 0, movieCount - 1);

  lcd.clear();   // clear the previous content

// display the movie name
  lcd.setCursor(0, 0);  // set the cursor to the first row
  lcd.print(movies[movieIndex]);  // display the movie title
  
  // display the critic rating
  lcd.setCursor(0, 1);  // set the cursor to the second row
  lcd.print("Rating: ");
  lcd.print(ratings[movieIndex]);  // display the rating
  
  delay(600);  // small delay to make scrolling smooth

  // is_cars check
  int is_cars = 0;
  if (movies[movieIndex] == "Cars" || 
      movies[movieIndex] == "Cars 2" || 
      movies[movieIndex] == "Cars 3") {
    is_cars = 1;
  }
  
  if (is_cars == 1) {  
    //turn lights on
    digitalWrite(rightLight, HIGH);   // Turn on the Right LED
    digitalWrite(leftLight, HIGH);   // Turn on the Left LED
    // Both motors speeding up
    for (int i = 0; i < 256; i++) {
      myMotorDriver.setDrive(LEFT_MOTOR, 0, i);
      myMotorDriver.setDrive(RIGHT_MOTOR, 0, i);
      delay(5);
    }

    delay(1000);  // Pause before slowing down, but let's put car noise here
    int frequency = 35;
    for (int j = 0; j < 3; j++) {  // Repeat 3 times
      int freq_inc = 2;
      for (int i = 0; i < 15; i++) {  // Run 15 times
        frequency += freq_inc;
        freq_inc += .5;
        tone(piezoPin, frequency);
        delay(100);
    }
    
      noTone(piezoPin); // Stop the sound
      frequency -= 10;  // Decrease frequency for next cycle
      delay(300);
    
  }
  noTone(piezoPin); // Stop the sound
    // Both motors slowing down
    for (int i = 255; i >= 0; i--) {
      myMotorDriver.setDrive(LEFT_MOTOR, 0, i);
      myMotorDriver.setDrive(RIGHT_MOTOR, 0, i);
      delay(2);
    }
    //turn lights off
    digitalWrite(leftLight, LOW);   // Turn on the Right LED
    digitalWrite(rightLight, LOW);   // Turn on the Left LED
    is_cars = 0;
  }

}
