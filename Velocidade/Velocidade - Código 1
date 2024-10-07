const int wheelSensorPin = 4; // Pino do sensor indutivo
volatile unsigned long pulseCount = 0; // Contador de pulsos
unsigned long lastTime = 0;
const unsigned long interval = 1000; // Intervalo de tempo para cálculo da velocidade (1 segundo)
const float wheelRadius = 0.27; // Raio da roda em metros (exemplo)


void IRAM_ATTR handleWheelInterrupt() {
  pulseCount++; // Incrementa o contador de pulsos na interrupção
}


void setup() {
  Serial.begin(115200);
  pinMode(wheelSensorPin, INPUT_PULLUP); // Configura o pino do sensor como entrada com pull-up
  attachInterrupt(digitalPinToInterrupt(wheelSensorPin), handleWheelInterrupt, FALLING); // Configura a interrupção na borda de descida
}


void loop() {
  unsigned long currentTime = millis();
 
  // Calcula a velocidade a cada segundo
  if (currentTime - lastTime >= interval) {
    detachInterrupt(digitalPinToInterrupt(wheelSensorPin)); // Desabilita a interrupção temporariamente
   
    // Calcula o número de rotações
    float rotations = pulseCount; // Supondo 1 pulso por rotação
   
    // Calcula a velocidade angular (ω = 2π * rotações / tempo)
    float omega = (2.0 * PI * rotations) / (interval / 1000.0); // rad/s
   
    // Calcula a velocidade linear (v = ω * r)
    float speed = omega * wheelRadius; // m/s
   
    // Convertendo para km/h
    float speedKmh = speed * 3.6;
   
    Serial.print("Velocidade: ");
    Serial.print(speedKmh);
    Serial.println(" km/h");
   
    pulseCount = 0; // Reseta o contador de pulsos
    lastTime = currentTime; // Atualiza o tempo


    attachInterrupt(digitalPinToInterrupt(wheelSensorPin), handleWheelInterrupt, FALLING); // Reabilita a interrupção
  }
}
