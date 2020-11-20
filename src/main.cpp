#include <Arduino.h>

#include <WiFi.h>

// measure frequencey on PIN 15 (input) and toggle an output on PIN 25 whenver a change is detected
// every second PIN26 is toggled as well, which can be observed using the logic analyzer (16MHz 50M samples)

volatile int count = 0;
volatile int frequency;
hw_timer_t * timer = NULL;

#define INPUTPIN 15
#define OSCPIN1 25
#define OSCPIN2 26

portMUX_TYPE isrMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


void IRAM_ATTR isr() {
  portENTER_CRITICAL(&isrMux);
  count++;
  digitalWrite(OSCPIN1, 1);
  digitalWrite(OSCPIN1, 0);
  portEXIT_CRITICAL(&isrMux);
}

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  frequency=count;
  count=0;
  //digitalWrite(OSCPIN2, 1-digitalRead(OSCPIN2));x
  digitalWrite(OSCPIN2, 1);
  digitalWrite(OSCPIN2, 0);
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  WiFi.mode(WIFI_OFF);
  btStop();

  Serial.begin(115200);
  Serial.print("setup() running on core ");
  Serial.println(xPortGetCoreID());

  pinMode(INPUTPIN, INPUT_PULLUP);
  attachInterrupt(INPUTPIN, isr, RISING);

  pinMode(OSCPIN1, OUTPUT);
  pinMode(OSCPIN2, OUTPUT);

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);
}

void display(int line, String text, int val) {
  Serial.print(text);
  Serial.println(val);
}

void loop() {
  display(0, "frequency=", frequency);
  delay(1000);
}

