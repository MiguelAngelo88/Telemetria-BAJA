/*
  Telemetria Veicular - Módulo Frontal
  Autor: Miguel Ângelo de Lacerda Silva
  Data: 2025
  Descrição: Processamento da Velocidade, Nível da Bateria e Temperatura do freio, e
    envio via barramento CAN
  Hardware: Heltec Wifi LoRa 32(V2)
*/

#include <CAN.h>  // Inclui a biblioteca CAN
#include "max6675.h" //Sensor da temp do freio

// Definições e variáveis para a Velocidade
const int  wheelSensorPin         = 13;  // Pino do sensor indutivo
volatile unsigned long pulseCount = 0;  // Contador de pulsos
unsigned long lastTime            = 0;
const unsigned long interval      = 1000;  // Intervalo de tempo para cálculo do RPM (1 segundo)
const float wheelRadius           = 0.27; // Raio da roda em metros 

// Definições e variáveis para a TEMPERATURA DO FREIO
const int thermoDO  = 19;
const int thermoCS  = 23;
const int thermoCLK = 5;
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);//declara o objeto MAX6675

// Definições e variáveis para a SENSOR DE TENSÃO DA BATERIA
const int batteryPin     = 36;
const float REF_VOLTAGE  = 3.3;
const float ADC_RESOLUTION = 4095.0;
const float R1             = 30000.0; // resistor values in voltage sensor (in ohms)
const float R2             = 7500.0;  // resistor values in voltage sensor (in ohms)

/*Definição dos IDs da variáveis CAN*/
const uint8_t CAN_ID_VELOCIDADE = 0x15;
const uint8_t CAN_ID_BATERIA = 0x17;
const uint8_t CAN_ID_FREIO = 0x16;

void setup() {
  pinMode(15, INPUT); // Alta impedância inicialmente para garantir que fique em HIGH
  delay(300); // Aguarda o boot completo e estabilização

  Serial.begin(115200);  // Inicia a comunicação serial

  initializeSensors();
  initializeCAN();  // Configura a comunicação CAN
  
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

void initializeCAN() {
  CAN.setPins(15, 4); // Define os pinos para RX e TX do CAN
  Serial.println("Tentando inicializar o controlador CAN...");
  
  // Tenta inicializar o CAN repetidamente até obter sucesso
  while (!CAN.begin(500E3)) {
    Serial.println("Falha ao iniciar o controlador CAN. Tentando novamente em 1 segundo...");
    delay(1000); // Aguarda 1 segundo antes de tentar novamente
  }
  
  Serial.println("Controlador CAN inicializado com sucesso!");
}

// Inicializa os sensores e configura a interrupções
void initializeSensors() {
  //RPM
  pinMode(wheelSensorPin, INPUT_PULLUP);  
  attachInterrupt(digitalPinToInterrupt(wheelSensorPin), handleWheelInterrupt, FALLING);
  Serial.println("Sensores inicializados com sucesso!");
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
    CAN.beginPacket(CAN_ID_VELOCIDADE);  // ID em hexadecimal
    CAN.write((speedInt >> 8) & 0xFF);  // Envia os 8 bits mais significativos
    CAN.write(speedInt & 0xFF);  // Envia os 8 bits menos significativos
    // Tenta encerrar o pacote
    if (!CAN.endPacket()) {
    Serial.println("Falha ao enviar pacote CAN de velocidade!");
    }
    Serial.println("Velocidade enviada via CAN.");
   
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
  CAN.beginPacket(CAN_ID_FREIO);  // ID em hexadecimal
  CAN.write((tempFreio >> 8) & 0xFF);  // Envia os 8 bits mais significativos
  CAN.write(tempFreio & 0xFF);        // Envia os 8 bits menos significativos
  // Tenta encerrar o pacote
  if (!CAN.endPacket()) {
  Serial.println("Falha ao enviar pacote CAN de freio!");
  } 
  Serial.println("Temperatura do freio enviada via CAN.");
}

void processBatteryLevel() {
  // read the analog input
  int adc_value = analogRead(batteryPin);

  // determine voltage at adc input
  float voltage_adc = ((float)adc_value * REF_VOLTAGE) / ADC_RESOLUTION;

  // calculate voltage at the sensor input
  float batteryVoltage = voltage_adc * (R1 + R2) / R2;

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
  CAN.beginPacket(CAN_ID_BATERIA);  // ID em hexadecimal
  CAN.write((batteryLevel >> 8) & 0xFF);  // Envia os 8 bits mais significativos
  CAN.write(batteryLevel & 0xFF);        // Envia os 8 bits menos significativos
  // Tenta encerrar o pacote
  if (!CAN.endPacket()) {
  Serial.println("Falha ao enviar pacote CAN de bateria!");
}
  Serial.println("Nivel de bateria enviado via CAN.");
}
