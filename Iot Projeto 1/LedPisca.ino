#define LED_ON 16
#define LED_BLK 17
#define BUTTON 15
#define P_INPUT 4
#define LDR_INPUT 5

// PWM config
int freq = 10000;
int res = 8;

int mode = 0;
int newState;
int oldState = HIGH;

int blkState = LOW;

unsigned long currentMillis;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  pinMode(P_INPUT, INPUT); 
  pinMode(LDR_INPUT, INPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED_BLK, OUTPUT);
  
  // PWM setup
  ledcAttach(LED_ON, freq, res);
}

void loop() {
  newState = digitalRead(BUTTON);

  // Verifica se o botão foi pressionado
  if (newState == LOW && oldState == HIGH){
    mode = !mode; // Inverte o modo atual
    Serial.print("Botão apertado!:");
    Serial.println(mode); 
    delay(200);   // Debounce
  }

  // Atualiza estado do botão
  oldState = newState;

  // leitura do ADC
  int adcValue;
  if (mode == 0) {
    adcValue = analogRead(P_INPUT);   // potenciometro
  } else {
    adcValue = analogRead(LDR_INPUT); // LDR
  }
  
  // converter ADC (0–4095) → PWM (0–255)
  int pwmValue = map(adcValue, 0, 4095, 0, 255);

  // converter ADC → intervalo (2000–100 ms)
  int interval = map(adcValue, 0, 4095, 2000, 100);

  // LED 0 (PWM)
  ledcWrite(LED_ON, pwmValue);
  
  // espera nao ocupada do LED 1 (Blink sem delay)
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    blkState = !blkState;
    digitalWrite(LED_BLK, blkState);
  }
}
