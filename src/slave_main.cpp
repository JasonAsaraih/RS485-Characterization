#include <Arduino.h>
#include <SoftwareSerial.h>

// Full ASCII test message
char msg[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

#define SLAVE_SE_TX_PIN 11
#define SLAVE_SE_RX_PIN 10

#define SLAVE_DIFF_TX_PIN 12
#define SLAVE_DIFF_RX_PIN 5

// SoftwareSerial softSerial(SLAVE_SE_RX_PIN, SLAVE_SE_TX_PIN);  // RX, TX
SoftwareSerial softSerial(SLAVE_DIFF_RX_PIN, SLAVE_DIFF_TX_PIN);  // RX, TX

const int MSG_LEN = 95;

char buffer[MSG_LEN];
int index = 0;
unsigned long startTime = 0;
bool timeOut = false;
unsigned long counter = 0;


// =====================
// LCS FUNCTION (2-row DP, memory safe)
// =====================
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

// =====================
// SETUP
// =====================
void setup() {
    Serial.begin(9600);
    softSerial.begin(9600);

    pinMode(8, OUTPUT);

    // Trigger master immediately
    digitalWrite(8, LOW);
}

// =====================
// LOOP
// =====================
void loop() {
    index = 0;
    timeOut = false;
    memset(buffer, 0, sizeof(buffer));

    unsigned long startTime = millis();
    Serial.print(counter);
    counter++;
    digitalWrite(8, HIGH); // Trigger master to send
    
    while (index < MSG_LEN) {
        if (softSerial.available()) {
            int c = softSerial.read();
            if (c >= 0) {
                buffer[index++] = (char)c;
            }
        }

        if (millis() - startTime > 3000UL) {
            timeOut = true;
            break;
        }

        delayMicroseconds(5);
    }

    digitalWrite(8, LOW);

    if (!timeOut && index == MSG_LEN) {
        int lcs = lcsLength(msg, buffer, MSG_LEN, MSG_LEN);
        float accuracy = (lcs * 100.0) / MSG_LEN;

        Serial.print("Accuracy (%): ");
        Serial.println(accuracy);
    } else {
        Serial.println("ERROR: Timeout while waiting for message");
    }

    delay(10);
}

// #include <Arduino.h>
// #include <SoftwareSerial.h>

// char msg[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

// SoftwareSerial softSerial(10, 11);

// const int MSG_LEN = 95;

// char buffer[MSG_LEN];
// int index = 0;

// void setup() {
//     Serial.begin(9600);
//     softSerial.begin(9600);

//     pinMode(8, OUTPUT);

//     // Trigger master immediately on reset
//     digitalWrite(8, HIGH);
// }

// void loop() {
//     // Wait until full message received
//     while (index < MSG_LEN) {
//         if (softSerial.available()) {
//             buffer[index++] = softSerial.read();
//         }
//     }

//     int correctCount = 0;

//     Serial.println("Received:");

//     for (int i = 0; i < MSG_LEN; i++) {
//         Serial.write(buffer[i]);

//         if (buffer[i] == msg[i]) {
//             correctCount++;
//         }
//     }

//     Serial.println();

//     float accuracy = (correctCount * 100.0) / MSG_LEN;

//     Serial.print("Accuracy (%): ");
//     Serial.println(accuracy);

//     // Lower trigger so master doesn't resend
//     digitalWrite(8, LOW);

//     while (true); // stop forever
// }