/*
 * Basic wireless control for the scooter-rover.
 * Connect to 192.168.4.1 for control menu.
 */

#include <ESP8266WiFi.h>
#include <SabertoothSimplified.h>
#include <Servo.h>

const char* ssid = "Rover Control"; // username/password
const char* password = "swordfish"; // for wi-fi server

const int SERVO_PIN = 2; // GPIO_2, D4

WiFiServer server(80); // Port 80

SabertoothSimplified rover;
Servo tiller;

int currentSpeed = 0;

void setup() {
  // connect the servo
  tiller.attach(SERVO_PIN);
  
  // Activate Sabertooth's serial interface
  SabertoothTXPinSerial.begin(9600);

  // turn on the wi-fi server
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
  String motorSetting = "0";
  String servoSetting = "0";
  client.flush();

  // check request, switch rover motors based on request
  if(request.indexOf("/rev/full") != -1) {
    currentSpeed = accelTo(currentSpeed, -127, 25);
    motorSetting = "-127";
  }
  else if(request.indexOf("/rev/half") != -1) {
    currentSpeed = accelTo(currentSpeed, -64, 25);
    motorSetting = "-64";
  }
  else if(request.indexOf("/zero") != -1) {
    currentSpeed = accelTo(currentSpeed, 0, 10);
    motorSetting = "0";
  }
  else if(request.indexOf("/fwd/half") != -1) {
    currentSpeed = accelTo(currentSpeed, 64, 25);
    motorSetting = "64";
  }
  else if(request.indexOf("/fwd/full") != -1) {
    currentSpeed = accelTo(currentSpeed, 127, 25);
    motorSetting = "127";
  }
  else if(request.indexOf("/pwr/") != -1) {
    request += '\r'; // so we can find the end of the request
    int pos = request.indexOf('=');
    pos++;
    for(;request[pos] != '\r';pos++) { // HTML form sends a request of the form /pwr/someinthere
      motorSetting += request[pos];    // this searches for that int
    }
    currentSpeed = accelTo(currentSpeed, motorSetting.toInt(), 25);
  }
  else if(request.indexOf("/serv/min") != -1) {
    servoSetting = "0";
    tiller.write(0);
  }
  else if(request.indexOf("/serv/zero") != -1) {
    servoSetting = "90";
    tiller.write(90);
  }
  else if(request.indexOf("/serv/max") != -1) {
    servoSetting = "180";
    tiller.write(180);
  }
  

  // prepare response to client
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>";
  s += "Motor power at:";
  s += motorSetting;
  s += "<br>Servo heading at:";
  s += servoSetting;
  s += "<br><a href=\"/rev/full\"><h2>Reverse, Full Power</h2></a>";
  s += "<br><a href=\"/rev/half\"><h2>Reverse, Half Power</h2></a>";
  s += "<br><a href=\"/zero\"><h2>Disable Motor</h2></a>";
  s += "<br><a href=\"/fwd/half\"><h2>Forward, Half Power</h2></a>";
  s += "<br><a href=\"/fwd/full\"><h2>Forward, Full Power</h2></a>";
  s += "<br><a href=\"/serv/min\"><h2>Servo Minimum Angle</h2></a>";
  s += "<br><a href=\"/serv/zero\"><h2>Servo Zero</h2></a>";
  s += "<br><a href=\"/serv/max\"><h2>Servo Maximum Angle</h2></a>";
  s += "<br><form action=\"/pwr\">Set Power Level<br><input type=\"text\" name=\"power\"><br><input type=\"submit\" value=\"SET\"></form>";
  s += "</html>\n"; 

  // send response
  client.print(s);
}

int accelTo(int currentPower, int targetPower, int stepDelay) {
  if(targetPower == currentPower) { // return if no change is to be made
    return currentPower;
  }
  if(targetPower > currentPower) { // if power is to be upped
    for(currentPower; currentPower < targetPower; currentPower++) {
      rover.motor(1, currentPower);
      delay(stepDelay);
    }
    return currentPower;
  }
  else if(targetPower < currentPower) { // if power is to be lowered
    for(currentPower; currentPower > targetPower; currentPower--) {
      rover.motor(1, currentPower);
      delay(stepDelay);
    }
    return currentPower;
  }
}

