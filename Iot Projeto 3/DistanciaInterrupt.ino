#define TRIGGER 27
#define ECHO 26

// Configurações do Sensor
int max_dist = 100;      // cm
float v_sound = 0.0343;  // cm/us (velocidade do som)
unsigned long wait_trigger = 10; // us
unsigned long max_wait;

// Timers
unsigned long previousMicros = 0;
unsigned long previousMillis = 0;

// Variáveis de Interrupção
unsigned long echoStart = 0;
unsigned long echoEnd = 0;
bool newData = false;

int state = 0;

void echo() {
  if (digitalRead(ECHO) == HIGH) {
    echoStart = micros();
  } else {
    echoEnd = micros();
    newData = true;
  }
}

void setup() {
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  digitalWrite(TRIGGER, LOW);
  Serial.begin(115200);

  // Calcula o tempo máximo de espera baseado na distância máxima
  max_wait = (2 * max_dist) / v_sound;

  // Configura interrupção na borda de subida e descida (CHANGE)
  attachInterrupt(digitalPinToInterrupt(ECHO), echo, CHANGE);
}

void loop() {
  unsigned long currentMicros = micros();
  unsigned long currentMillis = millis();

  switch(state) {

    // 0: Dispara o Trigger
    case 0:
      digitalWrite(TRIGGER, HIGH);
      previousMicros = currentMicros;
      state = 1;
      break;

    // 1: Mantém o Trigger em HIGH por 10us
    case 1:
      if (currentMicros - previousMicros >= wait_trigger) {
        digitalWrite(TRIGGER, LOW);
        previousMicros = currentMicros; // Reinicia timer para o timeout do Echo
        state = 2;
      }
      break;

    // 2: Aguarda o Echo 
    case 2:
      if (newData) {
        unsigned long start = echoStart;
        unsigned long end = echoEnd;
        newData = false;

        float distancia = (end - start) * v_sound / 2;
        
        Serial.print("Distancia: ");
        Serial.print(distancia);
        Serial.println(" cm");

        previousMillis = currentMillis; 
        state = 3; 
      } 
      else if (currentMicros - previousMicros > max_wait) {
        Serial.println("Distancia: 0 cm (Timeout)");
        previousMillis = currentMillis;
        state = 3;
      }
      break;

    // 3: Delay para leitura
    case 3:
      if (currentMillis - previousMillis >= 60) {
        state = 0;
      }
      break;
  }
}
