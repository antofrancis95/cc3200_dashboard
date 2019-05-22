/*
 Basic MQTT example 
 
  - connects to an MQTT server
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic"
*/

#include <Wire.h>
#include <BMA222.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "Adafruit_TMP006.h"

Adafruit_TMP006 tmp006(0x41);


BMA222 mySensor;
int8_t datax;
int8_t datay;
int8_t dataz;
int temprature;
char xdata[10];
char ydata[10];
char zdata[10];
char tdata[10];
char Accl_read[50];

// your network name also called SSID
char ssid[] = "Nokia 7 plus";
// your network password
char password[] = "asdfghjk";
// MQTTServer to use
char server[] = "192.168.43.68";

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received message for topic ");
  Serial.print(topic);
  Serial.print("with length ");
  Serial.println(length);
  Serial.println("Message:");
  Serial.write(payload, length);
  Serial.println();
}

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

void setup()
{
  Serial.begin(115200);

   mySensor.begin();
  uint8_t chipID = mySensor.chipID();
  // Start Ethernet with the build in MAC Address
  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to Network named: ");
  // print the network name (SSID);
  Serial.println(ssid); 
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    // print dots while we wait to connect
    Serial.print(".");
    delay(300);
  }
  
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an ip address");
  
  while (WiFi.localIP() == INADDR_NONE) {
    // print dots while we wait for an ip addresss
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nIP Address obtained");
  // We are connected and have an IP address.
  // Print the WiFi status.
  printWifiStatus();
}

void loop()
{
  // Reconnect if the connection was lost
  if (!client.connected()) {
    Serial.println("Disconnected. Reconnecting....");

    if(!client.connect("energiaClient")) {
      Serial.println("Connection failed");
    } else {
      Serial.println("Connection success");
      if(client.subscribe("inTopic")) {
        Serial.println("Subscription successfull");
      }
    }
  }

  
  datax = mySensor.readXData();
  Serial.print("X: ");
  Serial.print(datax);
  dtostrf(datax, 5, 1, xdata);

  datay = mySensor.readYData();
  Serial.print(" Y: ");
  Serial.print(datay);
  dtostrf(datay, 5, 1, ydata);

  dataz = mySensor.readZData();
  Serial.print(" Z: ");
  Serial.println(dataz);
  dtostrf(dataz, 5, 1, zdata);

  temprature = tmp006.readObjTempC();
  dtostrf(temprature, 5, 1, tdata);
  
  delay(25);

  strcpy(Accl_read,"X axis:");
  strcat(Accl_read,xdata);
  strcat(Accl_read," Y axis:");
  strcat(Accl_read,ydata);
  strcat(Accl_read," Z axis:");
  strcat(Accl_read,zdata);

  client.publish("cc/temprature",tdata);
  client.publish("cc/accleration",Accl_read); 
  client.publish("cc/xaxis",xdata);
  client.publish("cc/yaxis",ydata);
  client.publish("cc/zaxis",zdata);
  
//  if(client.publish("cc/accelerometercc/accelerometer",Accl_read)){
//    Serial.println("Publish success");
//  } else {
//    Serial.println("Publish failed");
//  }
 
  // Check if any message were received
  // on the topic we subsrcived to
  client.poll();
  delay(1000);
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
