#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>

// Full ASCII test message
char msg[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

#define RS485_PIN 7

#define TRIG_PIN 8

#define SLAVE_SE_TX_PIN 11
#define SLAVE_SE_RX_PIN 10

#define SLAVE_DIFF_TX_PIN 12
#define SLAVE_DIFF_RX_PIN 3

SoftwareSerial singledEndedSoftSerial(SLAVE_SE_RX_PIN, SLAVE_SE_TX_PIN);  // RX, TX
SoftwareSerial differentialSoftSerial(SLAVE_DIFF_RX_PIN, SLAVE_DIFF_TX_PIN);  // RX, TX

const int MSG_LEN = 95;

char buffer[MSG_LEN];
int index = 0;

int lcsLength(const char* a, const char* b, int n, int m) {
    int prev[MSG_LEN + 1];
    int curr[MSG_LEN + 1];

    // Initialize prev row
    for (int j = 0; j <= m; j++) {
        prev[j] = 0;
    }

    for (int i = 1; i <= n; i++) {
        curr[0] = 0;

        for (int j = 1; j <= m; j++) {
            if (a[i - 1] == b[j - 1]) {
                curr[j] = prev[j - 1] + 1;
            } else {
                curr[j] = max(prev[j], curr[j - 1]);
            }
        }

        // Copy current row into previous
        for (int j = 0; j <= m; j++) {
            prev[j] = curr[j];
        }
    }

    return prev[m];
}

void sendUART(void *pvParameters) {
    while(1){
        digitalWrite(TRIG_PIN, HIGH);
        memset(buffer, 0, MSG_LEN);

        singledEndedSoftSerial.listen();

        while (index < MSG_LEN) {
            if (singledEndedSoftSerial.available()) {
                buffer[index++] = singledEndedSoftSerial.read();
            }
        }
        index = 0;


        int lcs = lcsLength(msg, buffer, MSG_LEN, MSG_LEN);

        float accuracy = (lcs * 100.0) / MSG_LEN;
        int errors = MSG_LEN - lcs;

        Serial.print("SE Accuracy (%): ");
        Serial.print(accuracy);
        Serial.print("\t");

        memset(buffer, 0, MSG_LEN);
        differentialSoftSerial.listen();
        digitalWrite(TRIG_PIN, LOW);

        vTaskDelay(10);

        while (index < MSG_LEN) {
            if (differentialSoftSerial.available()) {
                buffer[index++] = differentialSoftSerial.read();
                Serial.println("available");

            }
        }
        index = 0;

        lcs = lcsLength(msg, buffer, MSG_LEN, MSG_LEN);

        accuracy = (lcs * 100.0) / MSG_LEN;
        errors = MSG_LEN - lcs;

        Serial.print("DIFF Accuracy (%): ");
        Serial.println(accuracy);

        digitalWrite(TRIG_PIN, LOW);
        vTaskDelay(10);

    }
}

void setup() {
    Serial.begin(9600);
    singledEndedSoftSerial.begin(9600);
    differentialSoftSerial.begin(9600);

    pinMode(TRIG_PIN, OUTPUT);

    digitalWrite(TRIG_PIN, LOW);

    pinMode(RS485_PIN, OUTPUT);
    digitalWrite(RS485_PIN, LOW);

    xTaskCreate(
        sendUART,
        "UART Read Task",
        256,
        NULL,
        1,
        NULL
    );

    vTaskStartScheduler();

}


void loop() {}

