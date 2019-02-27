/*
 Basic ESP8266 MQTT example for iandc

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <PubSubClient.h>


const char* ssid1 = "";    // WIFI number 1 
const char* password1 = "";
const char* ssid2 = "";// WIFI number 2  
const char* password2 = "";
const char* mqtt_server = ""; // config server MQTT 
const char* subTopic  = "";// config  sub server MQTT 
const char* pubTopic = "";// config  pub server MQTT 
const char* userMQTT ="";//config  user server MQTT
const char* passwordMQTT = "";//config  password server MQTT
int portMQTT = 0; //  config  port  server MQTT
const char* deviceMQTT = "";  // config device for mqtt
String pathupload = "";  // config path to upload program  from server 
int setminrandom = 0;///// config mix for random
int setmaxrandom = 0;//config mix for random
long lastMsg = 0;
int LED = 13;

WiFiClient espClient;
PubSubClient client(espClient);
ESP8266WiFiMulti WiFiMulti;

void setup_wifi() {

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid1, password1);
  WiFiMulti.addAP(ssid2, password2);
  
  // We start by connecting to a WiFi network delay  8 sec ready to wifi
    delay(8000);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String loadmqtt = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
   loadmqtt += ((char)payload[i]);
  }

  // Switch on the LED if an 1 was received as first character
  if (loadmqtt == "UPDATE") {
    ESPhttpUpdate.setLedPin(LED, LOW);

    t_httpUpdate_return ret = ESPhttpUpdate.update(espClient, pathupload);
    //t_httpUpdate_return ret = ESPhttpUpdate.update(client, "server", 80, "file.bin");

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
          }
  }
  else if (loadmqtt == "TEST")
  {
    client.publish(pubTopic, "hello world");
  }
  else {  
    delay(10);
  }

}

void reconnect() {  //Reconnetwifi When MQTT loss connect 
  // Loop until we're reconnected
  while (!client.connected()) {
    if (client.connect(deviceMQTT,userMQTT,passwordMQTT)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(pubTopic, "hello world");
      // ... and resubscribe
      client.subscribe(subTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void RECONNECTED_WIFI()   // Reconnetwifi When wifi loss connect 
{
  
   if (WiFi.status() != WL_CONNECTED) 
   {
    delay(1000);
    Serial.println(" don't connect wifi ");
   }
   else
   ESP.restart();
  
  
}

void setup() {
  delay(10);
  pinMode(LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server,portMQTT);
  client.setCallback(callback);
  randomSeed(micros());
}

void loop() {
  long now = millis();
  
  if(WiFiMulti.run() != WL_CONNECTED)
  {
    RECONNECTED_WIFI();
    digitalWrite(LED, LOW);   
  }
  else if (!client.connected()) 
  {
    reconnect();
    digitalWrite(LED, LOW);   
  }
  else if  (now - lastMsg > random(setminrandom, setmaxrandom)) 
  {
    lastMsg = now;                                       
  }
  else
  {
     delay(1);
  }
  client.loop();
  }

