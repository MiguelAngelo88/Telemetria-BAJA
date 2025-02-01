#include <CAN.h>  // Inclui a biblioteca CAN
#include "max6675.h" //Sensor da temp do freio

// Definições e variáveis para a Velocidade
#define  wheelSensorPin  13  // Pino do sensor indutivo
volatile unsigned long pulseCount = 0;  // Contador de pulsos
unsigned long lastTime = 0;
const unsigned long interval = 1000;  // Intervalo de tempo para cálculo do RPM (1 segundo)
const float wheelRadius = 0.27; // Raio da roda em metros 

// Definições e variáveis para a TEMPERATURA DO FREIO
#define thermoDO  19
#define thermoCS  23
#define thermoCLK  5
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);//declara o objeto MAX6675

// Definições e variáveis para a SENSOR DE TENSÃO DA BATERIA
#define batteryPin 12

void setup() {
  Serial.begin(115200);  // Inicia a comunicação serial
  while (!Serial);

  initializeCAN();  // Configura a comunicação CAN
  initializeSensors();  // Configura o sensor indutivo
}

void loop() {

  processVelocity();  // Processa o cálculo e envio do RPM
  processTempFreio(); // Processa o cálculo e envio da Temp do freio
  processBatteryLevel(); // Processa o cálculo e envio do nível da bateria

  delay(500);
}

// Função de interrupção para contar pulsos do sensor indutivo
void IRAM_ATTR handleWheelInterrupt() {
  pulseCount++;
}

// Inicializa a comunicação CAN
void initializeCAN() {
  CAN.setPins(15, 4);
  Serial.println("Inicializando o Transmissor CAN");
  if (!CAN.begin(500E3)) {
    Serial.println("Falha ao iniciar o controlador CAN");
  }
}

// Inicializa os sensores e configura a interrupções
void initializeSensors() {
  //RPM
  pinMode(wheelSensorPin, INPUT_PULLUP);  
  attachInterrupt(digitalPinToInterrupt(wheelSensorPin), handleWheelInterrupt, FALLING);
}

void processVelocity() {
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
    int speedInt = (int)speedKmh; // Armazena apenas a parte inteira da velocidade
   
    Serial.print("Velocidade: ");
    Serial.print(speedKmh);
    Serial.println(" km/h");

    // Envia o RPM via CAN
    CAN.beginPacket(0x15);  // ID em hexadecimal
    CAN.write((speedInt >> 8) & 0xFF);  // Envia os 8 bits mais significativos
    CAN.write(speedInt & 0xFF);  // Envia os 8 bits menos significativos
    CAN.endPacket();  // Encerra o pacote
    Serial.println("Velocidade enviado via CAN.");
   
    pulseCount = 0; // Reseta o contador de pulsos
    lastTime = currentTime; // Atualiza o tempo


    attachInterrupt(digitalPinToInterrupt(wheelSensorPin), handleWheelInterrupt, FALLING); // Reabilita a interrupção
  }
}

void processTempFreio() {
  // Lê a temperatura em graus Celsius
  int tempFreio = thermocouple.readCelsius();
  
  // Imprime a temperatura no monitor serial
  Serial.print("Temperatura do Freio: ");
  Serial.print(tempFreio);
  Serial.println(" °C");
  
  // Envia a temperatura do freio via CAN
  CAN.beginPacket(0x16);  // ID em hexadecimal
  CAN.write((tempFreio >> 8) & 0xFF);  // Envia os 8 bits mais significativos
  CAN.write(tempFreio & 0xFF);        // Envia os 8 bits menos significativos
  CAN.endPacket();  // Encerra o pacote
  
  Serial.println("Temperatura do freio enviada via CAN.");
}

void processBatteryLevel() {
  // Leitura da tensão da bateria
  float batteryVoltage = analogRead(batteryPin) * 5 * 5.0 / 1023;

  // Vetores com os limites das tensões e seus respectivos níveis de carga
  const float voltageLevels[] = {12.60, 12.50, 12.42, 12.32, 12.20, 12.06, 11.90, 11.75, 11.58, 11.31, 10.50};
  const int chargeLevels[] = {100, 90, 80, 70, 60, 50, 40, 30, 20, 10, 0};

  // Determinação do nível de carga
  int batteryLevel = 0;
  for (int i = 0; i < sizeof(voltageLevels) / sizeof(voltageLevels[0]); i++) {
    if (batteryVoltage >= voltageLevels[i]) {
      batteryLevel = chargeLevels[i];
      break;  // Sai do loop ao encontrar o nível correspondente
    }
  }

  // Envio do nível da bateria via CAN
  CAN.beginPacket(0x17);  // ID em hexadecimal
  CAN.write((batteryLevel >> 8) & 0xFF);  // Envia os 8 bits mais significativos
  CAN.write(batteryLevel & 0xFF);        // Envia os 8 bits menos significativos
  CAN.endPacket();  // Encerra o pacote
}
