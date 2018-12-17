#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"
#include <PubSubClient.h>

#define RX 6
#define TX 7

WiFiEspClient espClient; //init the Ethernet Client Object
PubSubClient client(espClient); 
SoftwareSerial soft(RX,TX); // create virtual Serial for esp8266 module

//variables for wifi
const char* AP= "mYs"; //change wifi ESSID
const char* PASS= "1234amir"; //change Wifi Password

//variables for MQTT server
const char* mqtt_server = "192.168.43.67"; //Raspberry Pi IP address
int mqtt_port = 1883;
const char* mqttUser = "mcprggpc"; //change your MQTT username
const char* mqttPass = "changetoyourpassword"; //change to your MQTT password

int status = WL_IDLE_STATUS; //wireless status flags


void setup(){
  //init serial for Debugging
  Serial.begin(115200);
  soft.begin(9600); //init serial for wifi module

  WiFi.init(&soft); //init esp module
  wifiConnect(); //Connect to wifi

  //Connect to MQTT Server
  client.setServer(mqtt_server,mqtt_port);

  //PubSub client callback
  client.setCallback(callback);
  delay(5000);
}

void wifiConnect(){
  //check if wifi shield connected
  if(WiFi.status() == WL_NO_SHIELD){
    Serial.println("shield not Present");
    //dont continue, (loop forever)
    while(true);
  }

  //if shield connected, then attempt to join the LAN
  while(status != WL_CONNECTED){
    Serial.print("Attempt to Connect to:");
    Serial.println(AP);
    //try to connect to wifi
    status = WiFi.begin(AP,PASS);
  }

  //if connected, print the network information
  Serial.println("Connected to Network");
  Serial.print("IP address:");
  Serial.println(WiFi.localIP());
  Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

}

void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}

void reconnect(){
  //reconnect to MQTT if disconnected
  Serial.println("Inside Reconnect Function");
  while(!client.connected())
  {
    Serial.println("Reconnecting to MQTT..");

    //try to connect to mqtt account
    if(client.connect("Pi")){
      Serial.println("Connected to Raspberry Pi");

        //if connected, then we send some data.
        Serial.println("Trying to Send Data Hello");
        delay(2000);
        client.publish("esptest", "Hello from ESP8266");
        client.subscribe("esptest");
        Serial.println("Sucess");
    }
    else{
      Serial.print("Failed with state:");
      Serial.println(client.state());
      delay(2000);
    }
  }


}

void loop(){
  if(!client.connected())
  {
    reconnect();
  }
  client.loop();
}

