
//static const uint8_t D0   = 16;
//static const uint8_t D1   = 5;
//static const uint8_t D2   = 4;
//static const uint8_t D3   = 0;
//static const uint8_t D4   = 2;
//static const uint8_t D5   = 14;
//static const uint8_t D6   = 12;
//static const uint8_t D7   = 13;
//static const uint8_t D8   = 15;
//static const uint8_t D9   = 3;
//static const uint8_t D10  = 1;

//boilerplate code sourced from Robojax.com 
/*
   
   written by Ashwin Kumar K
   on 08/09/20
*/

int Pin1 = D1;//IN1 is connected
int Pin2 = D2;//IN2 is connected
int Pin3 = D3;//IN3 is connected
int Pin4 = D4;//IN4 is connected


int pole1[] = {0, 0, 0, 0, 0, 1, 1, 1, 0}; //pole1, 8 step values
int pole2[] = {0, 0, 0, 1, 1, 1, 0, 0, 0}; //pole2, 8 step values
int pole3[] = {0, 1, 1, 1, 0, 0, 0, 0, 0}; //pole3, 8 step values
int pole4[] = {1, 1, 0, 0, 0, 0, 0, 1, 0}; //pole4, 8 step values

int poleStep = 0;
int  dirStatus = 3;// stores direction status 3= stop (do not change)

String buttonTitle1[] = {"CCW", "CW"}; //Watch Video for details (on Udemy)
String buttonTitle2[] = {"CCW", "CW"};
String argId[] = {"ccw", "cw"};

int rpm = 10;
int t = (1000000);
float starttime = millis();
float endtime = millis();
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "StepperMotor"
#define STAPSK  "12#qwaszx@"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);

void handleRoot() {

  // ESP32 Relay Motor Control
  String HTML = "<!DOCTYPE html>\
  <html>\
  <head>\
  \t\n<title>28BYJ-48 Motor Control</title>\
  \t\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
  \n<style>\
 \nhtml,body{\t\nwidth:100%\;\nheight:100%\;\nmargin:0}\n*{box-sizing:border-box}\n.colorAll{\n\tbackground-color:#90ee90}\n.colorBtn{\n\tbackground-color:#add8e6}\n.angleButtdon,a{\n\tfont-size:30px\;\nborder:1px solid #ccc\;\ndisplay:table-caption\;\npadding:7px 10px\;\ntext-decoration:none\;\ncursor:pointer\;\npadding:5px 6px 7px 10px}a{\n\tdisplay:block}\n.btn{\n\tmargin:5px\;\nborder:none\;\ndisplay:inline-block\;\nvertical-align:middle\;\ntext-align:center\;\nwhite-space:nowrap}\n";

  HTML += " </style>\n\n</head>\n\n<body> <center> \n<h1>28BYJ-48 Stepper Motor Control </h1>\n ";

  HTML += "<form action=\"/DataRPM\" method=\"POST\">Enter RPM : <input type=\"number\" name=\"RPM\" placeholder=\"RPM\"> <input type=\"submit\" value=\"Send RPM\"></form>Try '1 -20 RPM' Only.";
  HTML += "<form action=\"/DataRunTime\" method=\"POST\"></br> Enter Remaining time (in min)  :<input type=\"number\" name=\"RunTime\" placeholder=\"RunTime\"><input type=\"submit\" value=\"Send Time\"></form>Try '50 min' ...";


  if (dirStatus == 2) {
    HTML += "\n\t<h2><span style=\"background-color: #FFFF00\">Motor Running in CW</span></h2>\n";
  } else if (dirStatus == 1) {
    HTML += "\n\t<h2><span style=\"background-color: #FFFF00\">Motor Running in CCW</span></h2>\n";
  } else {
    HTML += "\n\t<h2><span style=\"background-color: #FFFF00\">Motor OFF</span></h2>\n";
  }
  if (dirStatus == 1) {
    HTML += "\t<div class=\"btn\">\n\t\t<a class=\"angleButton\" style=\"background-color:#f56464\"  href=\"/motor?";
    HTML += argId[0];
    HTML += "=off\">";
    HTML += buttonTitle1[0]; //motor ON title
  } else {
    HTML += "\t<div class=\"btn\">\n\t\t<a class=\"angleButton \" style=\"background-color:#90ee90\"  href=\"/motor?";
    HTML += argId[0];
    HTML += "=on\">";
    HTML += buttonTitle2[0]; //motor OFF title
  }
  HTML += "</a>\t\n\t</div>\n\n";

  if (dirStatus == 2) {
    HTML += "\t<div class=\"btn\">\n\t\t<a class=\"angleButton\" style=\"background-color:#f56464\"  href=\"/motor?";
    HTML += argId[1];
    HTML += "=off\">";
    HTML += buttonTitle1[1]; //motor ON title
  } else {
    HTML += "\t<div class=\"btn\">\n\t\t<a class=\"angleButton \" style=\"background-color:#90ee90\"  href=\"/motor?";
    HTML += argId[1];
    HTML += "=on\">";
    HTML += buttonTitle2[1]; //motor OFF title
  }
  HTML += "</a>\t\n\t</div>\n\n";


  HTML += "\t\n</body> </center> \n</html>\n";
  server.send(200, "text/html", HTML);

}

void handleNotFound() {

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);

}

void setup(void) {
  rpm = 15;
  rpm = 15;
  t = 60000000 / (2038 * rpm * 2);


  pinMode(Pin1, OUTPUT);//define pin for ULN2003 in1
  pinMode(Pin2, OUTPUT);//define pin for ULN2003 in2
  pinMode(Pin3, OUTPUT);//define pin for ULN2003 in3
  pinMode(Pin4, OUTPUT);//define pin for ULN2003 in4

  Serial.println(t);
  Serial.begin(115200);//initialize the serial monitor
  Serial.println("28BYJ-48 Stepper Motor Control");

  //28BYJ-48 Steper Motor Control
  //  WiFi.mode(WIFI_STA);
  //  WiFi.begin(ssid, password);
  boolean result = WiFi.softAP(ssid, password);
  if (result == true)
  {
    Serial.println("Ready");
  }
  else
  {
    Serial.println("Failed!");
  }
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Started SSID ");
  Serial.println(ssid);
  Serial.print("IP address: http://");
  Serial.println(WiFi.localIP());

  //multicast DNS
  if (MDNS.begin("ssid")) {
    Serial.println("MDNS responder started");
    Serial.print("access via http://");
    Serial.println(ssid);
  }

  server.on("/", handleRoot);
  server.on("/motor", HTTP_GET, motorControl);
  server.on("/DataRPM", HTTP_POST, handleRPM);
  server.on("/DataRunTime", HTTP_POST, handleTime);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
  if (millis() < endtime) {

    if (dirStatus == 1) {
      poleStep++;
      driveStepper(poleStep);
    } else if (dirStatus == 2) {
      poleStep--;
      driveStepper(poleStep);
    } else {
      driveStepper(8);
    }
    if (poleStep > 7) {
      poleStep = 0;
    }
    if (poleStep < 0) {
      poleStep = 7;
    }

  }


  delayMicroseconds(t);
  //delay(2);

}




void motorControl() {

 
  if (server.arg(argId[0]) == "on")
  {
    dirStatus = 1;// CCW

  } else if (server.arg(argId[0]) == "off") {
    dirStatus = 3;  // motor OFF

  } else if (server.arg(argId[1]) == "on") {
    dirStatus = 2;  // CW

  } else if (server.arg(argId[1]) == "off") {
    dirStatus = 3;  // motor OFF

  }


  handleRoot();
}//motorControl end

void handleRPM() {
  Serial.println("Entering handleRPM");
  Serial.println(server.arg("RPM"));
  if ( ! server.hasArg("RPM") || server.arg("RPM") == NULL) { // If the POST request doesn't have username and password data
    server.send(400, "text/plain", "400: Invalid Request Please fill both the arguments, Press back to continue ");         // The request is invalid, so send HTTP status 400
    return;
  }
  rpm = (server.arg("RPM")).toFloat();
  t = 60000000 / (2038 * rpm * 2);
  Serial.print("Setting delay = ");
  Serial.println(t);

  //
  //  if (server.arg("RPM") == "10" && server.arg("RunTime") == "1") { // If both the username and the password are correct
  //    server.send(200, "text/html", "<h1>Welcome, " + server.arg("username") + "!</h1><p>Login successful</p>");
  //    delay(2000);
  //  }

  handleRoot();

}


void handleTime() {
  Serial.println("Entering handleTime");
  Serial.println(server.arg("RunTime"));
  if (  ! server.hasArg("RunTime") || server.arg("RunTime") == NULL) { // If the POST request doesn't have username and password data
    server.send(400, "text/plain", "400: Invalid Request Please fill both the arguments, Press back to continue ");         // The request is invalid, so send HTTP status 400
    return;
  }
  float newt = (server.arg("RunTime")).toFloat();
  newt = newt * 60 * 1000;
  endtime = millis() + newt;
  //  rpm = (server.arg("RPM")).toInt();
  //  t = 60000000 / (2038 * rpm * 2);
  //  Serial.print("Setting delay = ");
  //  Serial.println(t);

  //
  //  if (server.arg("RPM") == "10" && server.arg("RunTime") == "1") { // If both the username and the password are correct
  //    server.send(200, "text/html", "<h1>Welcome, " + server.arg("username") + "!</h1><p>Login successful</p>");
  //    delay(2000);
  //  }

  handleRoot();

}

void driveStepper(int c)
{

  digitalWrite(Pin1, pole1[c]);
  digitalWrite(Pin2, pole2[c]);
  digitalWrite(Pin3, pole3[c]);
  digitalWrite(Pin4, pole4[c]);
}
