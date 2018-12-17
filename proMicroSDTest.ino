/*
Testing MicroSD card for Arduino Pro Mircro,
Board and Module Connection:

Arduino | module
VCC --> VCC
D15 --> SCK
D14 --> MISO
D16 --> MOSI
GND --> GND
D4  --> CS

*/

//include librariesR
NOTEPAD
Testing ghost keyboard


#include <SPI.h>
#include <SD.h>
#include "Keyboard.h"

//variables
File file; //creating file descriptor for read and write
boolean first = true;
String FILENAME = "script.txt";
int chipSelect = 4;

void setup() {
  Serial.begin(9600);
  pinMode(chipSelect, OUTPUT);

  if(!SD.begin(chipSelect)){
    Serial.println("Cannot Initialize SD Card");
  }
  if(SD.exists(FILENAME)){
    Serial.println("File script.txt existed");

    //Deleting existing file.
    if(SD.remove(FILENAME)){
      Serial.println("Delete Sucess");
    }else{
      Serial.println("Cannot Delete");
    }
  }
}

void loop() {

  //Trying to Write to SD Card
  file = SD.open(FILENAME, FILE_WRITE);
  if(file){
    int number = random(10);
    file.println(number);
    file.close();
    Serial.println("Wrote Sucess");
    Serial.println(number);
  }else{
    Serial.println("Cannot Open to write");
  }

  //We open to READ
  file = SD.open(FILENAME, FILE_READ);
  if(file){
    Serial.println("------Reading SD Card Content-----");
    char character;
    while((character = file.read()) != -1){
      Serial.print(character);
    }
    file.close();
    Serial.println("----end----");
  }else{
    Serial.println("Cannot Open to Read");
  }

  delay(10000); //wait 10 second

}
