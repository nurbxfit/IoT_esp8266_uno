
//testing MicroSD card on Uno

#include <SD.h>

//define our pin connection
int chipSelect = 4; //cs pin connect to uno digital pin 4;

File file; //create our file descriptor for read and write

void setup(){
  Serial.begin(9600); //start our serial with 9600 buds

  pinMode(chipSelect, OUTPUT); //cs pin must be set to OUTPUT mode

  if(!SD.begin(chipSelect)){ //initialize SD card using SD.begin();
    Serial.println("Could not Initialize SD card."); //error message if something wrong.
  }

  if(SD.exists("arduino.txt")){
    Serial.println("File arduino.txt exist");

    //try to deleted existing file
    if(SD.remove("arduino.txt")){
      Serial.println("Delete Sucess");
    }
    else{
      Serial.println("Cannot Delete");
    }
  }
}


void loop(){
  file = SD.open("arduino.txt", FILE_WRITE); //open file arduino.txt to write data
  
  //we try to write data to SD card
  if(file)
  {
    int number = random(10); //generate random number
    file.println(number);
    file.close();
    Serial.println("Wrote Sucess");
    Serial.println(number);
  }
  else{
    Serial.println("Cannot Open file to Write");
  }

  // we open again the file to read data.
  file = SD.open("arduino.txt",FILE_READ); //open file to read
  if(file){
    Serial.println("----Reading SD Card----");
    char character;
    while((character = file.read()) != -1 ){
      Serial.print(character);
    }
    file.close();
    Serial.println("------end--------");
  }
  else{
    Serial.println("Cannot Open to READ");
  }
  
  delay(15000); //wait 5000ms or 5 second
}

