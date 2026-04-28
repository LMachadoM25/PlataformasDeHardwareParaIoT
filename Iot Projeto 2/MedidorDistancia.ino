#define TRIGGER 27
#define ECHO 26

int max_dist = 400; // cm
float v_sound = 0.0343; // cm/us

unsigned long max_wait;
unsigned long wait = 100000; // us

unsigned long previousMicros = 0;
unsigned long previousMillis = 0; // Para o delay de segurança
unsigned long echoStart = 0;
unsigned long echoEnd = 0;

int state = -1; // Começa no descanso

void setup() {
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  Serial.begin(115200);

  max_wait = (2 * max_dist) / v_sound;
}

void loop() {

  unsigned long currentMicros = micros();
  unsigned long currentMillis = millis();

  switch(state) {
      
    // -1: Pausa de 60ms obrigatória para o sensor não travar
    case -1:
      if (currentMillis - previousMillis >= 60) {
        state = 0;
      }
      break;

    // 0 - Inicia o trigger
    case 0:
      digitalWrite(TRIGGER, HIGH);
      previousMicros = currentMicros;
      state = 1;
      break;

    // 1 - Mantem HIGH por 10us
    case 1:
      if (currentMicros - previousMicros >= wait) {
        digitalWrite(TRIGGER, LOW);
        previousMicros = currentMicros;
        state = 2;
      }
      break;

    // 2 - Espera o ECHO subir
    case 2:
      if (digitalRead(ECHO) == HIGH) {
        echoStart = currentMicros;
        state = 3;
      } else if (currentMicros - previousMicros > max_wait) {
        Serial.println("Distância: 0 cm (Timeout)");
        previousMillis = currentMillis; // Volta pro descanso
        state = -1;
      }
      break;

    // 3 - Espera o ECHO descer
    case 3:
      if (digitalRead(ECHO) == LOW) {
        echoEnd = currentMicros;

        unsigned long duration = echoEnd - echoStart;
        float distance = (duration * v_sound) / 2;

        Serial.print("Distância: ");
        Serial.print(distance);
        Serial.println(" cm");

        previousMillis = currentMillis; // Volta pro descanso
        state = -1;
      } else if (currentMicros - echoStart > max_wait) {
         previousMillis = currentMillis;
         state = -1;
      }
      break;
  }
}
