/* ESP8266 + MQTT Humidity and Temperature Node
 * Can also receive commands; adjust messageReceived() function 
 * See MakeUseOf.com for full build guide and instructions
 * Author: James Bruce, 2015
 */

#include <MQTTClient.h>
#include <ESP8266WiFi.h>
//#include <DHT.h>

const char* ssid = "GEARTRACKS";
const char* password = "Raspberry";
 

char* subscribeTopic = "geartracks/flutter/12345678";  // "openhab/parentsbedroom/incoming"; // subscribe to this topic; anything sent here will be passed into the messageReceived function
char* statusTopic = "geartracks/flutter/status"; //topic to publish temperatures readings to
char* alarmTopic = "geartracks/flutter/alarm"; // publish humidity readings
const char* server = "192.168.1.151"; // server or URL of MQTT broker
String clientName = "flutter-12345678-"; // just a name used to talk to MQTT broker
long interval = 60000; //(ms) - 60 seconds between reports
unsigned long resetPeriod = 864000000; // 1 day - this is the period after which we restart the CPU, to deal with odd memory leak errors

//#define DHTTYPE DHT11 // DHT11 or DHT22
//#define DHTPIN  2 


unsigned long prevTime;
//DHT dht(DHTPIN, DHTTYPE,11); 
float h, t;  

WiFiClient wifiClient;
MQTTClient client;

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

void setup() {
  Serial.begin(115200);
  // dht.begin();   
  client.begin(server,wifiClient);
  Serial.print("Connecting to ");
  Serial.println(ssid);  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Generate client name based on MAC address and last 8 bits of microsecond counter
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += macToStr(mac);
  clientName += "-";
  clientName += String(micros() & 0xff, 16);

  Serial.print("Connecting to ");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(clientName);
  
  if (client.connect((char*) clientName.c_str())) {
    Serial.println("Connected to MQTT broker");
    Serial.print("Subscribed to: ");
    Serial.println(subscribeTopic);
    client.subscribe(subscribeTopic);
    // submit a welcome JSON string to Q
    String Payload = "{\"message\":\"Flutter_12345678 signs on ... hello world\"}";
    client.publish(statusTopic, Payload);
    Serial.println(Payload);
  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
    abort();
  }

  prevTime = 0;
}

void loop() {
  static int counter = 0;
  
  if(prevTime + interval < millis() || prevTime == 0){
    prevTime = millis();
    Serial.println("checking again");
    Serial.println(prevTime);

    // read from sensors
    // h = dht.readHumidity(); // change to battery voltage
    // t = dht.readTemperature();

    
    // h = h*1.23;
    // t = t*1.1;
    
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("No sensor readings!");
    }
    else if(!client.connected()){
      Serial.println("Connection to broker lost; retrying");
    }
    else{
      // Json string//
      String tPayload = "{\"sensorID\":\"" + clientName + "\"}";
      //char* hPayload = "test alarm"; //f2s(h,0);
      
      Serial.println(t);
      //Serial.println(h);
      
      Serial.println(tPayload);
      //Serial.println(hPayload);

      client.publish(statusTopic, tPayload);
      //client.publish(alarmTopic, hPayload);

      Serial.println("published data");
    }
    
  }
  
  client.loop();

  // reset after a day to avoid memory leaks 
  if(millis()>resetPeriod){
    ESP.restart();
  }
}


/* float to string
 * f is the float to turn into a string
 * p is the precision (number of decimals)
 * return a string representation of the float.
 */
char *f2s(float f, int p){
  char * pBuff;                         // use to remember which part of the buffer to use for dtostrf
  const int iSize = 10;                 // number of buffers, one for each float before wrapping around
  static char sBuff[iSize][20];         // space for 20 characters including NULL terminator for each float
  static int iCount = 0;                // keep a tab of next place in sBuff to use
  pBuff = sBuff[iCount];                // use this buffer
  if(iCount >= iSize -1){               // check for wrap
    iCount = 0;                         // if wrapping start again and reset
  }
  else{
    iCount++;                           // advance the counter
  }
  return dtostrf(f, 0, p, pBuff);       // call the library function
}

void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
  Serial.print("incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.print(payload);
  Serial.println();
}
