volatile unsigned long last_time = 0;
volatile float T = 0, vel_ms = 0, vel_kmh = 0;
const float alpha = 0.2; // Fator de suavização 0 < alpha < 1


int n = 1; // Numero de pontos detectáveis pelo sensor
const float wheelRadius = 0.27; // Raio da roda em metros


#define sensorPin 3


void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(sensorPin), calc_T, FALLING);
}


void loop() {
  calc_vel();
  Serial.println(vel_kmh);  
}


void calc_vel(){
  // Verifica se T não é zero para evitar divisão por zero
  if (T != 0) {
    // Calcula a velocidade em m/s
    vel_ms = (2 * PI * wheelRadius) / (n * (T / 1000000.0));
    // Filtragem exponencial
    static float vel_ms_filtered = 0; // Valor inicial
    vel_ms_filtered = alpha * vel_ms + (1 - alpha) * vel_ms_filtered;
    // Calcula a velocidade em km/h
    vel_kmh = vel_ms_filtered * 3.6;
  }
}


void calc_T(){
  detachInterrupt(digitalPinToInterrupt(sensorPin));
  T = micros() - last_time;
  last_time = micros();
  attachInterrupt(digitalPinToInterrupt(sensorPin), calc_T, FALLING);
}
