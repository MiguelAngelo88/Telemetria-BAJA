const int sensorPin = 4; // Pino do sensor indutivo
volatile unsigned long pulseCount = 0; // Contador de pulsos
unsigned long lastTime = 0;
const unsigned long interval = 1000; // Intervalo de tempo para cálculo do RPM (1 segundo)


void IRAM_ATTR handleInterrupt() {
  pulseCount++; // Incrementa o contador de pulsos na interrupção
}


void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT_PULLUP); // Configura o pino do sensor como entrada com pull-up
  attachInterrupt(digitalPinToInterrupt(sensorPin), handleInterrupt, FALLING); // Configura a interrupção na borda de descida
}


void loop() {
  unsigned long currentTime = millis();
 
  // Calcula o RPM a cada segundo
  if (currentTime - lastTime >= interval) {
    detachInterrupt(digitalPinToInterrupt(sensorPin)); // Desabilita a interrupção temporariamente
   
    // Calcula o RPM
    unsigned long rpm = (pulseCount * 60) / (interval / 1000);
   
    Serial.print("RPM: ");
    Serial.println(rpm);
   
    pulseCount = 0; // Reseta o contador de pulsos
    lastTime = currentTime; // Atualiza o tempo


 attachInterrupt(digitalPinToInterrupt(sensorPin), handleInterrupt, FALLING); // Reabilita a interrupção
  }
}
