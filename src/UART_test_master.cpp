#include <Arduino.h>
#include <SoftwareSerial.h>

char msg[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

#define MASTER_SE_TX_PIN 11
#define MASTER_SE_RX_PIN 10

#define MASTER_DIFF_TX_PIN 12
#define MASTER_DIFF_RX_PIN 13

// SoftwareSerial softSerial(MASTER_SE_RX_PIN, MASTER_SE_TX_PIN);  // RX, TX
SoftwareSerial softSerial(MASTER_DIFF_RX_PIN, MASTER_DIFF_TX_PIN);  // RX, TX

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