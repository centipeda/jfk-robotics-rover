/*
 * Basic wireless control for the scooter-rover.
 * Connect to 192.168.4.1 for control menu.
 */

#include <ESP8266WiFi.h>
#include <SabertoothSimplified.h>

const char* ssid = "Rover Control"; // username/password
const char* password = "swordfish"; // for wi-fi server

WiFiServer server(80); // Port 80

SabertoothSimplified rover;

void setup() {
  SabertoothTXPinSerial.begin(9600);
  // Serial.begin(9600);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  server.begin();
}

void loop() {
  // if someone connects
  WiFiClient client = server.available();
  if(!client)
    return;

  // wait until client is available
  while(!client.available())
    delay(1);

  // read the first line of the request
  String request = client.readStringUntil('\r');
  client.flush();

  // check request, switch rover motors based on request
  if(request.indexOf(/*request string here*/) != -1) {
    // do stuff
  }

  // prepare response to client
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>";
  // build response string here
  s += "</html>\n"; 

  // send response
  client.print(s);
}

