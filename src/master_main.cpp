#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>

#define RS485_PIN 7

#define TRIG_PIN 8

#define MASTER_SE_TX_PIN 11
#define MASTER_SE_RX_PIN 10

#define MASTER_DIFF_TX_PIN 12
#define MASTER_DIFF_RX_PIN 13

QueueHandle_t trigQueue;

// SoftwareSerial SoftSerial(MASTER_SE_RX_PIN, MASTER_SE_TX_PIN);  // RX, TX
SoftwareSerial SoftSerial(MASTER_DIFF_RX_PIN, MASTER_DIFF_TX_PIN);  // RX, TX

char msg[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

bool isRisingEdge(bool current, bool last) {
    return current == HIGH && last == LOW;
}

bool isFallingEdge(bool current, bool last) {
    return current == LOW && last == HIGH;
}

void updateTrigState(void *pvParameters) {
    (void) pvParameters;

    while (1) {
        bool currentTrigState = digitalRead(TRIG_PIN);
        xQueueSend(trigQueue, &currentTrigState, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void writeUART(void *pvParameters) {
    bool current, last = LOW;

    while (1) {
        xQueueReceive(trigQueue, &current, portMAX_DELAY);

        if (isRisingEdge(current, last)) {
            vTaskDelay(pdMS_TO_TICKS(50)); // small stabilization delay
            SoftSerial.print(msg);
            Serial.println("Sent RISING EDGE message");
        }
        last = current;
    }
}

void setup() {
    Serial.begin(9600);
    SoftSerial.begin(9600);

    trigQueue = xQueueCreate(10, sizeof(bool));

    pinMode(TRIG_PIN, INPUT);

    xTaskCreate(
        updateTrigState,
        "Trigger State Update Task",
        128,
        NULL,
        2,
        NULL
    );

    xTaskCreate(
        writeUART,
        "SE UART Write Task",
        128,
        NULL,
        1,
        NULL
    );

    vTaskStartScheduler();
}

void loop() {}