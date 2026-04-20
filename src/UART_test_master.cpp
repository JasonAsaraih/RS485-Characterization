#include <Arduino.h>
#include <SoftwareSerial.h>

char msg[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

SoftwareSerial softSerial(10, 11);  // RX, TX

bool sent = false;

void setup() {
    Serial.begin(9600);
    softSerial.begin(9600);

    pinMode(8, INPUT);  // listen for trigger
}

void loop() {
    if (digitalRead(8) == HIGH && !sent) {
        delay(50); // small stabilization delay

        softSerial.print(msg);

        Serial.println("Sent message");

        sent = true;  // only send once
    }
}