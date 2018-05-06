/*
  Projet d'apprentissage d'un objet connecté (IoT)  pour réaliser une sonde de température
  ESP8266 + DHT22 + LED + MQTT + Home-Assistant
  Projets DIY (http://www.projetsdiy.fr) - Mai 2016
  Licence : MIT
  https://diyprojects.io/esp8266-dht22-mqtt-make-iot-include-home-assistant/
  
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
#define wifi_ssid "ssid"
#define wifi_password "wifi_password"
 
#define mqtt_server "m12.cloudmqtt.com"
#define mqtt_user "username"     
#define mqtt_password "password"  
 
//Buffer to decode MQTT messages

char message_buff[100];
 
long lastMsg = 0;   
long lastRecu = 0;
int buttonState = 0;
bool debug = true;  //Display log message if True

int relayPin = 14;
const int mqttPort = 11760;
 
// Create abjects   
WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
  Serial.begin(115200);     
  pinMode(relayPin,OUTPUT); 
  setup_wifi();           //Connect to Wifi network
  client.setServer(mqtt_server, mqttPort);    // Configure MQTT connexion
  client.setCallback(callback);           // callback function to execute when a MQTT message   
}
 
//Connexion au réseau WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
 
  WiFi.begin(wifi_ssid, wifi_password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi OK ");
  Serial.print("=> ESP8266 IP address: ");
  Serial.print(WiFi.localIP());
}
 
//Reconnexion
void reconnect() {
 
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker ...");
    if (client.connect("ArduinoBubble", mqtt_user, mqtt_password)) {
      Serial.println("OK");
    } else {
      Serial.print("KO, error : ");
      Serial.print(client.state());
      Serial.println(" Wait 5 secondes before to retry");
      delay(5000);
    }
  }
}
 
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastRecu > 100 ) { // update buttons and LED's 10 times per second
    lastRecu = now;
    client.subscribe("homeassistant/switch1"); // listen 
  }



 
}
 
// MQTT callback function
// D'après http://m2mio.tumblr.com/post/30048662088/a-simple-example-arduino-mqtt-m2mio
void callback(char* topic, byte* payload, unsigned int length) {
 
  int i = 0;
  if ( debug ) {
    Serial.println("Message recu =>  topic: " + String(topic));
    Serial.print(" | longueur: " + String(length,DEC));
  }
  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  
  String msgString = String(message_buff);
  if ( debug ) {
    Serial.println("Payload: " + msgString);
  }
  
  if ( msgString == "ON" ) {
    digitalWrite(relayPin,HIGH); 
  } else {
    digitalWrite(relayPin,LOW);   
  }
}
