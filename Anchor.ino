#include<SoftwareSerial.h>
#include<XBee.h>

const int anchorID = 1;

SoftwareSerial mySerial = SoftwareSerial(2, 3);
XBee xbee = XBee();
Rx16Response rx16 = Rx16Response();
int numChairs = 1;
int current = 0;

boolean r2 = false;
boolean r3 = false;
String r2Rssi = "";
String r3Rssi = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  xbee.setSerial(mySerial);
}

void loop() {
  // put your main code here, to run repeatedly:
  xbee.readPacket();
  if (xbee.getResponse().isAvailable()) {
    if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
      xbee.getResponse().getRx16Response(rx16);
      uint8_t str_rssi = rx16.getRssi();
      Serial.println("Received from " + (String) rx16.getRemoteAddress16() + ", rssi = " + str_rssi);
      String r = "";
      for (int i = 0; i < rx16.getDataLength(); i++) {
        r += (char) rx16.getData(i);
      }
      if (rx16.getRemoteAddress16() == 2) {
        r2 = true;
        r2Rssi = str_rssi;
      } else if (rx16.getRemoteAddress16() == 3) {
        r3 = true;
        r3Rssi = str_rssi;
      }
      if (r2 == true && r3 == true) {
        Serial.println(r2Rssi + " | " + r3Rssi);
        uint8_t payload[] = {'a', 'n', 'c', 'h', 'o', 'r', '1'};
        int mydel = 5000;
        for (int i = 1; i <= numChairs; i++) {
          int destID = i + 5;
          Serial.println("Sending to " + String(destID) + " " + String(mydel));
          Tx16Request tx16 = Tx16Request(destID, payload, sizeof(payload));
          xbee.send(tx16);
        }
        String req = "{\"r2Rssi\": " + r2Rssi + ", \"r3Rssi\": " +  r3Rssi + "}";
        int len = req.length();
        uint8_t payload1[len];
        for (int i = 0; i < len; i++) {
          payload1[i] = (uint8_t) req[i];
        }
        Serial.println("Sending to " + String(5) + " " + String(mydel));
        Tx16Request tx16 = Tx16Request(5, payload1, sizeof(payload1));
        xbee.send(tx16);
        delay(mydel);
        r2 = false;
        r3 = false;
      }
    }
  }
}















