#include <EtherCard.h>

byte Ethernet::buffer[700]; // tcp/ip send and receive buffer
BufferFiller bfill;

#define numberOfChannels 8
#define NETWORK_LED 13
#define MESSAGE_LED 11

// This would use digital2 to digital9 for channels
int RELAIS[numberOfChannels] = { 22,23,24,25,26,27,28,29 };
int BUTTON[numberOfChannels] = { 30,31,32,33,34,35,36,37 };
int BTN_LED[numberOfChannels] = { 2,3,4,5,6,7,8,9 };

float tempC;
int reading;
int tempPin = 12;

#include "ext_def.h"

void setup(){

  // Start serial communication
  Serial.begin(115200);

  Serial.println("Starting...");

  // Set analog ref voltage to 1.1 V
  analogReference(INTERNAL1V1);

  pinMode(NETWORK_LED, OUTPUT);
  pinMode(MESSAGE_LED, OUTPUT);

  digitalWrite(NETWORK_LED, LOW);
  digitalWrite(MESSAGE_LED, LOW);

  Serial.println("Set Modes...");
  for (byte i = 0; i < numberOfChannels; i++) {

    // Alle Relais ausschalten
    pinMode(RELAIS[i], OUTPUT);
    setRelais(i, LOW);

    // Set BUTTON pins as input
    pinMode(BUTTON[i], INPUT_PULLUP); // BUTTONs are inputs

    // Set BTN_LED pins as output
    pinMode(BTN_LED[i], OUTPUT); // BUTTONs are inputs

    setButton(i, LOW);
  }

  Serial.println("Initialize Ether...");
  if (ether.begin(sizeof Ethernet::buffer, mymac, 53) == 0) {
    Serial.println("Failed to access Ethernet controller");
    error();
  }
  else
  {
    digitalWrite(NETWORK_LED, HIGH);
    ether.staticSetup(myip, gwip, dnsip);

    ether.printIp("IP:  ", ether.myip);
    ether.printIp("GW:  ", ether.gwip);  
    ether.printIp("DNS: ", ether.dnsip);  
  }

  Serial.println("Setup finished...");
}

const char http_OK[] PROGMEM =
  "HTTP/1.0 200 OK\r\n"
  "Content-Type: text/html\r\n"
  "Pragma: no-cache\r\n\r\n";

const char http_Found[] PROGMEM =
  "HTTP/1.0 302 Found\r\n"
  "Content-Type: text/html\r\n"
  "Pragma: no-cache\r\n\r\n";

const char http_Unauthorized[] PROGMEM =
  "HTTP/1.0 401 Unauthorized\r\n"
  "Content-Type: text/html\r\n\r\n"
  "<h1>401 Unauthorized</h1>";

void homePage(){
  bfill.emit_p(
    PSTR(
      "$F"
      "Usage http://&lt;ip&gt?on=&lt;n&gt<br />"
      "Usage http://&lt;ip&gt?off=&lt;n&gt<br />"
      "Usage http://&lt;ip&gt?toggle=&lt;n&gt<br />"
      "Usage http://&lt;ip&gt?status=&lt;n&gt<br />"
    ),
    http_OK
  );
}

void statusPage(int status) {

  bfill.emit_p(
   PSTR(
     "$F"
     "$S"
    ),
    http_OK,
    (status == 1) ? "OFF" : "ON"
  ); 
}

void statusPage(String out) {

  char tempOut[out.length() + 1];
  out.toCharArray(tempOut, out.length() + 1);

  bfill.emit_p(
   PSTR(
     "$F"
     "$S"
    ),
    http_OK,
    tempOut
  ); 
}

void tempPage(double deg) {

Serial.println(String (deg));
  int temp = (int)deg;

  bfill.emit_p(
   PSTR(
     "$F"
     "TEMP: "
     "$S"
    ),
    http_OK,
    deg
  ); 
}

void loop(){

  readButtons();

  readTemp();

  // Copy recieved packets to data buffer
  word len = ether.packetReceive();

  // Parse recieved data. pos is offset to TCP payload
  word pos = ether.packetLoop(len); 

  if (pos) {

    delay(1);   // necessary for my system
    bfill = ether.tcpOffset();

    // Create pointer that points to Ethernet::buffer + pos
    char *data = (char *) Ethernet::buffer + pos;

    if (strncmp("GET /", data, 5) != 0) {

      // Unsupported HTTP request
      // 304 or 501 response would be more appropriate
      bfill.emit_p(http_Unauthorized);

    } else {

        // Move pointer forward 5 characters (the "GET /" string)
        data += 5;

        // Serial.println(data);

        if (data[0] == ' ') { //Check if the home page, i.e. no URL

          // How home
          homePage();

        } else if (!strncmp("?on=", data, 4)) { // Switch on relais

          data += 4;
          char tmpChannel = data[0];
          int channel = tmpChannel - '0';

          int state = setRelais(channel, HIGH);

          delay(50);
          statusPage(state);

        } else if (!strncmp("?off=", data, 5)) { // Switch off relais

          data += 5;
          char tmpChannel = data[0];
          int channel = tmpChannel - '0';

          int state = setRelais(channel, LOW);

          delay(50);
          statusPage(state);

        } else if (!strncmp("?toggle=", data, 8)) { // Toggle relais

          data += 8;
          char tmpChannel = data[0];
          int channel = tmpChannel - '0';

          int state = toggleRelais(channel);

          delay(50);
          statusPage(state);

        } else if (!strncmp("?status=", data, 8)) { // Get status of relais

          data += 8;
          char tmpChannel = data[0];
          int channel = tmpChannel - '0';

          int state = getRelais(channel);
          statusPage(state);

        } else if (!strncmp("?msg=1", data, 6)) { //Check if a url which changes the leds has been recieved

          digitalWrite(MESSAGE_LED, HIGH);

          bfill.emit_p(http_OK);

        } else if (!strncmp("?msg=0", data, 6)) { //Check if a url which changes the leds has been recieved

          digitalWrite(MESSAGE_LED, LOW);

          bfill.emit_p(http_OK);

        } else if (!strncmp("?cmd=getTemp", data, 11)) { //Check if a url which changes the leds has been recieved

          String temp = String(tempC);

          statusPage(temp);

        } else { //Otherwise, page isn't found
          // Page not found
          bfill.emit_p(http_Unauthorized);
        }
      }

      ether.httpServerReply(bfill.position());    // send http response
  }
}

void readButtons() {

  for (int i = 0; i < numberOfChannels; i++) {
    if (getButton(i) == LOW) {

      // Entprellen
      delay(50);
      if (getButton(i) == LOW) {

        buttonPressed(i);
        while (getButton(i) == LOW);
      }
    }
  }
}

void buttonPressed(int i) {

  Serial.println("Button pressed");

  int state = toggleRelais(i);

  fhemSetState(i, state);
}

extern void ethernet_callback(byte status, word off, word len) {
  Serial.println(F(">>>"));
  Ethernet::buffer[off+300] = 0;
  Serial.print((const char*) Ethernet::buffer + off);
  Serial.println(F("..."));
}

void fhemSetState(int i, bool state) {

  // Send notification to fhem
  ether.copyIp(ether.hisip, fhemIp);

  while (ether.packetLoop(ether.packetReceive()));

  char paramString[128];
  sprintf(paramString, "cmd=setstate%%20%s%i%%20%s", devicePrefix, (i+1), (state ? "on" : "off"));

  ether.browseUrl(PSTR("/fhem?"), paramString, website, authHeader, ethernet_callback);
  // http://myserver/fhem?cmd=setstate%20PM01_Msg%20on
}

bool getButton(int i) {
  return digitalRead(BUTTON[i]);
}

void setButton(int i, bool state) {
  digitalWrite(BTN_LED[i], state);
}

bool getRelais(int i) {
  return !digitalRead(RELAIS[i]);
}

bool setRelais(int i, bool value) {
  digitalWrite(RELAIS[i], !value);
  int state = !digitalRead(RELAIS[i]);

  setButton(i, state);
  return state;
}

bool toggleRelais(int i) {
  return setRelais(i, !getRelais(i));
}

void error() {
  while (1) {
      digitalWrite(MESSAGE_LED, HIGH);
      delay(1000);
      digitalWrite(MESSAGE_LED, LOW);
      delay(1000);
    }
}

void readTemp () {
  reading = analogRead(tempPin);
  tempC = reading / 9.31;
}

