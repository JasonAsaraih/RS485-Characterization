#include <Arduino.h>
#include <SoftwareSerial.h>

// Full ASCII test message
char msg[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

SoftwareSerial softSerial(10, 11);

const int MSG_LEN = 95;

char buffer[MSG_LEN];
int index = 0;

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
    digitalWrite(8, HIGH);
}

// =====================
// LOOP
// =====================
void loop() {
    // Receive full message
    while (index < MSG_LEN) {
        if (softSerial.available()) {
            buffer[index++] = softSerial.read();
        }
    }

    Serial.println("Received:");

    for (int i = 0; i < MSG_LEN; i++) {
        Serial.write(buffer[i]);
    }
    Serial.println();

    // =====================
    // LCS ACCURACY
    // =====================
    int lcs = lcsLength(msg, buffer, MSG_LEN, MSG_LEN);

    float accuracy = (lcs * 100.0) / MSG_LEN;
    int errors = MSG_LEN - lcs;

    Serial.println("---------------------------------------------------");
    Serial.print("LCS Length: ");
    Serial.println(lcs);

    Serial.print("Accuracy (%): ");
    Serial.println(accuracy);

    Serial.print("Error Count: ");
    Serial.println(errors);
    Serial.println("---------------------------------------------------");

    // Stop master from resending
    digitalWrite(8, LOW);

    while (true); // halt forever
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