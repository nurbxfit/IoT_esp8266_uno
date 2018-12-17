#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"
#include <PubSubClient.h>
#include <Servo.h>

//define pin for virtual serial
#define RX 6 
#define TX 7 

//create object instance
WiFiEspClient espClient; //init the esp ethernet client object
PubSubClient client(espClient); //create pubsub object for mqtt
SoftwareSerial soft(RX,TX); //create fake serial for esp module

Servo myservo; //Servo object

//variable for WIFI access point
const char * SSID = "mYs"; //change wifi ESSID to your AP ID
const char * PASS = "password1234"; //change to AP password

//variable for MQTT server
const char * mqtt_server = "192.168.43.67"; 	//Raspberry Pi IP address (server)
int mqtt_port = 1883; 				//MQTT server listening port
const char * mqtt_topic = "songID"; //change topic for different device

int status = WL_IDLE_STATUS; //wifi status flags


char song = '1'; //just another variable

int outPin = 12;
int servoPin =13;
int angle = 0;

//boolean to tells if the pin was toggled or not
boolean toggle = true;


void setup(){
    //init Serial for Debugging
    Serial.begin(115200);
    soft.begin(9600); 

    WiFi.init(&soft);//init esp module
    wifiConnect(); //function to connect to wifi

    //connect to MQTT server
    client.setServer(mqtt_server,mqtt_port);

    //set callback function for PubSub mqtt
    client.setCallback(callback);
    delay(5000); //wait 5 second

    //setup pin for output to relay

  if (song=='1') {//sun, storm, and forest
    pinMode(outPin, OUTPUT);
    
    myservo.attach(servoPin);
    myservo.write(angle);
  }   
}

void wifiConnect(){
    //check if our module is up and connected
    if(WiFi.status()==WL_NO_SHIELD){
        Serial.println("Shield not Connected");
        //if not connected, we don't continue
        while(true);
    }
    
    //try to joint to network
    while(status != WL_CONNECTED){
        //while not connected
        Serial.print("Attempting connect to:");
        Serial.println(SSID);

        //connect to AP
        status = WiFi.begin(SSID,PASS);
    }

    //if connected, show network info
    Serial.println("Connected to Network");
    Serial.print("IP address:");
    Serial.println(WiFi.localIP());
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

}

//define callback function
void callback(char* topic, byte* payload, unsigned int length){
    Serial.println("Inside Callback");
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    //char *stat="OFF";
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
        
    }
    //Serial.print("Stat:");
    Serial.println();

    // Do the thing if your song plays
    if (song=='7') {
      servoHold();
    }
    if (song=='1') {
      pinToggle(outPin);//sun;
    }
  }

//callback function to connect to MQTT
void connectMqtt(){
    Serial.println("Trying to Connect to Mqtt");
    while(!client.connected())
    {
        Serial.println("Connecting to Server");
        //delete "PiCrust" if had problem.
        if(client.connect("PiCrust")){
            Serial.println("Connected to PiCrust");
        }else{
            Serial.println("Fail to connect");
            Serial.print("Error State:");
            Serial.println(client.state());
            delay(2000);
        }
    }

    client.subscribe(mqtt_topic); //subscribe to topic
    Serial.print("Subscribing to:");
    Serial.println(mqtt_topic);
}

void loop(){

    if(!client.connected()){
        connectMqtt(); //connect to MQTT if not connected
    }
    client.loop();
    
}



void pinToggle(int pin) {//toggle pin high/low whenever song is heard
  digitalWrite(pin, !digitalRead(pin));
}


void pinHold(int holdTime, int pin) {//hold pin low for some time when song is heard, then go back high
  digitalWrite(pin, LOW);
  delay(holdTime);
  digitalWrite(pin, HIGH);
}

// my servo not functioning well
void servoHold() {//when song is heard move servo to some position then move it back to neutral
int pos = angle + 90;
 myservo.write(pos);
 delay(1000);
 myservo.write(pos-90);
}
