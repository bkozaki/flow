/*
  WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi module (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 9.

 If the IP address of your board is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * Board with NINA module (Arduino MKR WiFi 1010, MKR VIDOR 4000 and UNO WiFi Rev.2)
 * LED attached to pin 9

 created 25 Nov 2012
 by Tom Igoe
 */
#include <SPI.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

const int redLED = 2;
const int blueLED = 3;
const int greenLED = 5;
 
int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  Serial.begin(9600);      // initialize serial communication
  pinMode(redLED, OUTPUT);      // set the LED pin mode
  pinMode(blueLED, OUTPUT); 
  pinMode(greenLED, OUTPUT); 
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
}


void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            showWebPage(client);
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        performRequest(currentLine);
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

void showWebPage(WiFiClient client) {
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  // the content of the HTTP response follows the header:
  client.println("<h1>Arduino Remote Control</h1>");
  client.println("<table border=1 style='text-align:center'>");
  client.println("<tr><th>Component</th><th>Status</th><th>Control</th></tr>");

  // Red LED
  client.print("<tr><td>Red LED</td><td>");
  if (digitalRead(redLED)) {
    client.print("<font style='color:green;'>ON</font>");
  } else {
    client.print("<font style='color:red;'>OFF</font>");
  }
  client.println("</td><td><a href='/redLED/on'>ON</a> / <a href='/redLED/off'>OFF</a></td></tr>");

  // Yellow LED
  client.print("<tr><td>Yellow LED</td><td>");
  if (digitalRead(blueLED)) {
    client.print("<font style='color:green;'>ON</font>");
  } else {
    client.print("<font style='color:red;'>OFF</font>");
  }
  client.println("</td><td><a href='/yellowLED/on'>ON</a> / <a href='/yellowLED/off'>OFF</a></td></tr>");

  // Green LED
  client.print("<tr><td>Green LED</td><td>");
  if (digitalRead(greenLED)) {
    client.print("<font style='color:green;'>ON</font>");
  } else {
    client.print("<font style='color:red;'>OFF</font>");
  }
  client.println("</td><td><a href='/greenLED/on'>ON</a> / <a href='/greenLED/off'>OFF</a></td></tr>");

  client.println("</table>");

  // The HTTP response ends with another blank line
  client.println();
}

void performRequest(String line) {
  if (line.endsWith("GET /redLED/on")) {
    digitalWrite(redLED, HIGH);
  } else if (line.endsWith("GET /redLED/off")) {
    digitalWrite(redLED, LOW);
  } else if (line.endsWith("GET /yellowLED/on")) {
    digitalWrite(blueLED, HIGH);
  } else if (line.endsWith("GET /yellowLED/off")) {
    digitalWrite(blueLED, LOW);
  } else if (line.endsWith("GET /greenLED/on")) {
    digitalWrite(greenLED, HIGH);
  } else if (line.endsWith("GET /greenLED/off")) {
    digitalWrite(greenLED, LOW);
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
