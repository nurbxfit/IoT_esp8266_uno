#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"
#include <PubSubClient.h>

#define RX 10
#define TX 11

IPAddress server(192,168,43,67);


WiFiEspClient espClient; //init the Ethernet Client Object
PubSubClient client(espClient); 
SoftwareSerial soft(RX,TX); // create virtual Serial for esp8266 module

char AP[] = "mYs"; //change wifi ESSID
char PASS[]= "1234amir"; //change Wifi Password
char mqtt_server[] = "192.168.43.67"; //Raspberry Pi IP address
char mqtt_port[] = "1883";
int status = WL_IDLE_STATUS; //wireless status flags

void setup(){
  //initialize serial for debugging
  Serial.begin(9600); 
  soft.begin(115200); //initialize serial for module

  WiFi.init(&soft); //initialize esp module
  wifiConnect(); // connect to wifi


  //connect to MQTT Server 
  client.setServer(mqtt_server, mqtt_port);

  //PubSub client callback
  client.setCallback(callback);

  //setup pin

}

void wifiConnect(){

  if(WiFi.status() == WL_NO_SHIELD){
    Serial.println("Module Not properly Connected to Uno");
    while(true); //dont continue;
  }

  //if wifi shield pressent, try connect to wifi
  while(status != WL_CONNECTED){
    Serial.print("Attempting to Connect to Wifi:");
    Serial.println(AP);
    
    //connect to WIFI
    status = WiFi.begin(AP,PASS);
  }

  Serial.print("You Are Connected to: ");
  Serial.println(AP);
  Serial.println("IP address:"+ WiFi.localIP());
}

//callback function
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();


  //check which song played
   // Do the thing if your song plays
  if ((char)payload[0]=='1'){
    Serial.println("Something Send");
  }
}



void loop(){
  //try to connect to server if disconnected
  if(!client.connected()){
    reconnect();
  }
  client.loop();
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect, just a name to identify the client
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("command","hello world");
      // ... and resubscribe
      client.subscribe("presence");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
