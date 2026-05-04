#define TRIGGER 26
#define ECHO 27

int max_dist = 100; // cm
float v_sound = 0.0343; // cm/us

unsigned long max_wait;
unsigned long wait = 10; // us

unsigned long previousMicros = 0;
unsigned long previousMillis = 0;

// Variáveis usadas na interrupção precisam ser volatile
volatile unsigned long echoStart = 0;
volatile unsigned long echoEnd = 0;
volatile bool newData = false;

int state = 0;

void echo() {
  if (digitalRead(ECHO) == HIGH) {
    // Borda de subida
    echoStart = micros();
  } else {
    // Borda de descida
      echoEnd = micros();
      newData = true;
  }
}

void setup() {
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  Serial.begin(115200);

  max_wait = (2 * max_dist) / v_sound;

  // Configura interrupção
  attachInterrupt(digitalPinToInterrupt(ECHO), echo, CHANGE);
}

void loop() {

  unsigned long currentMicros = micros();
  unsigned long currentMillis = millis();

  switch(state) {

    // Inicia trigger
    case 0:
      digitalWrite(TRIGGER, HIGH);
      previousMicros = currentMicros;
      state = 1;
      break;

    // Mantém HIGH por 10us
    case 1:
      if (currentMicros - previousMicros >= wait) {
        digitalWrite(TRIGGER, LOW);
        previousMicros = currentMicros;
        state = 2;
      }
      break;

    // Espera a interrupção
    case 2:
      if (newData){
        Serial.print("Distância: ");
        Serial.print((echoEnd - echoStart) * v_sound / 2);
        Serial.println(" cm");
        newData = false;
        state = 0;
      }else if (currentMicros - previousMicros > max_wait) {
        Serial.println("Distância: 0 cm (Timeout)");
        previousMillis = currentMillis;
        state = 0;
      }
      break;
  }
}
