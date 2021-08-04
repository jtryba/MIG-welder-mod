// DEFINE PINS
#define DELAY_POT_PIN A0
#define TRIGGER_PIN 2 // 10k to 5v (active low)
#define GAS_RELAY_PIN 3
#define FEED_RELAY_PIN 4
#define EXTRA_RELAY_PIN 5
#define TRIGGER_HOLD_PIN 6 // 10k to 5v (active low)
//

#define RELAY_ON LOW
#define RELAY_OFF HIGH

#define TRIG_UP HIGH
#define TRIG_DOWN LOW

#define TRIG_HOLD_ON LOW

volatile int delay_time = 100;

volatile bool trig_state = TRIG_UP;
volatile bool trig_last_state = TRIG_UP;

void setup() {
  pinMode(DELAY_POT_PIN, INPUT);
  pinMode(TRIGGER_PIN, INPUT);
  pinMode(TRIGGER_HOLD_PIN, INPUT_PULLUP);
  
  pinMode(GAS_RELAY_PIN, OUTPUT);
  pinMode(FEED_RELAY_PIN, OUTPUT);
  pinMode(EXTRA_RELAY_PIN, OUTPUT);

  digitalWrite(FEED_RELAY_PIN, RELAY_OFF);
  digitalWrite(GAS_RELAY_PIN, RELAY_OFF);
  digitalWrite(EXTRA_RELAY_PIN, RELAY_OFF);
  
  ReadPot();

  attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), StartWeld, FALLING);
}

void StartWeld() { // active low trigger
	if (TriggerHold())
		trig_state = !trig_state;
	else
	{
		detachInterrupt(digitalPinToInterrupt(TRIGGER_PIN));
		trig_state = TRIG_DOWN;
		attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), EndWeld, RISING);
	}
}

void EndWeld() {
  detachInterrupt(digitalPinToInterrupt(TRIGGER_PIN));
  trig_state = TRIG_UP;
  attachInterrupt(digitalPinToInterrupt(TRIGGER_PIN), StartWeld, FALLING);
}

void ReadPot() {
  delay_time = analogRead(DELAY_POT_PIN);
}

bool TriggerHold() {
  return digitalRead(TRIGGER_HOLD_PIN) == TRIG_HOLD_ON;
}

bool TriggerPressed() {
  return trig_state == TRIG_DOWN;
}

void loop() {
    ReadPot();
    if (trig_state != trig_last_state) {
      trig_last_state = trig_state;
      if (TriggerPressed()) {
        digitalWrite(GAS_RELAY_PIN, RELAY_ON);
        delay(delay_time);
        if (TriggerPressed()) {
          digitalWrite(FEED_RELAY_PIN, RELAY_ON);
        } else {
          digitalWrite(GAS_RELAY_PIN, RELAY_OFF);
        }
      } else {
        digitalWrite(FEED_RELAY_PIN, RELAY_OFF);
        delay(delay_time*2);
        digitalWrite(GAS_RELAY_PIN, RELAY_OFF);
      }
    }
}
