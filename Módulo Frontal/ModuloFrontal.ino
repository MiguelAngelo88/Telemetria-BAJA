/*
  Telemetria Veicular - Módulo de Aquisição
  Autor: Miguel Ângelo de Lacerda Silva
  Data: 2025
  Descrição: Processamento da Velocidade, RPM do Motor, Nível da Bateria, 
             Temperatura do freio e Temperatura da CVT, 
             envio via barramento CAN
  Hardware: Heltec Wifi LoRa 32(V2)
*/

#include <CAN.h>          // Biblioteca CAN
#include "max6675.h"      // Sensor da temp do freio
#include <Wire.h>
#include <Adafruit_MLX90614.h> // Sensor IR da CVT
#include <esp_task_wdt.h>

#define WDT_TIMEOUT_S 15 // Timeout de 5 segundos

// ---- VELOCIDADE ----
const int wheelSensorPin = 13;
volatile unsigned long pulseCountVel = 0;
unsigned long lastVelocidadeTime = 0;
const unsigned long intervaloVelocidade = 1000;
const float wheelRadius = 0.27;

// ---- RPM DO MOTOR ----
const int rpmPin = 17; // GPIO17 para o sensor de RPM
volatile unsigned long pulseCountRPM = 0;
unsigned long lastRPMTime = 0;
const unsigned long intervaloRPM = 1000; // 1 segundo

// ---- TEMPERATURA DO FREIO ----
const int thermoDO  = 19;
const int thermoCS  = 23;
const int thermoCLK = 5;
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
unsigned long lastFreioTime = 0;
const unsigned long intervaloFreio = 1000;

// ---- TEMPERATURA DA CVT (MLX90614) ----
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
unsigned long lastCVTTime = 0;
const unsigned long intervaloCVT = 1000;
bool mlxPresent = false;


// ---- NÍVEL DA BATERIA ----
const int batteryPin = 36;
const float REF_VOLTAGE = 3.3;
const float ADC_RESOLUTION = 4095.0;
const float R1 = 30000.0;
const float R2 = 7500.0;
unsigned long lastBateriaTime = 0;
const unsigned long intervaloBateria = 2000;
// Filtro IIR
float filtroBateria = 0.0f;
bool filtroInicializado = false;

// ---- IDs CAN ----
const uint8_t CAN_ID_VELOCIDADE = 0x15;
const uint8_t CAN_ID_FREIO      = 0x16;
const uint8_t CAN_ID_BATERIA    = 0x17;
const uint8_t CAN_ID_RPM        = 0x18;
const uint8_t CAN_ID_CVT        = 0x19;

void setup() {
  //pinMode(15, INPUT);
  delay(300);

  Serial.begin(115200);
  Serial.println("Inicializando Task Watchdog Timer...");
  esp_task_wdt_init(WDT_TIMEOUT_S, true); 
  esp_task_wdt_add(NULL); 
  Serial.println("Task Watchdog Timer inicializado.");

  initializeSensors();
  initializeCAN();  
}

void loop() {
  esp_task_wdt_reset();

  unsigned long currentMillis = millis();

  if (currentMillis - lastVelocidadeTime >= intervaloVelocidade) {
    processVelocity();
    lastVelocidadeTime = currentMillis;
  }

  if (currentMillis - lastRPMTime >= intervaloRPM) {
    processRPM();
    lastRPMTime = currentMillis;
  }

  if (currentMillis - lastFreioTime >= intervaloFreio) {
    processTempFreio();
    lastFreioTime = currentMillis;
  }

  if (currentMillis - lastCVTTime >= intervaloCVT) {
    processTempCVT();
    lastCVTTime = currentMillis;
  }

  if (currentMillis - lastBateriaTime >= intervaloBateria) {
    processBatteryLevel();
    lastBateriaTime = currentMillis;
  }
}

// ---- Interrupções ----
void IRAM_ATTR handleWheelInterrupt() {
  pulseCountVel++;
}
void IRAM_ATTR handleRPMInterrupt() {
  pulseCountRPM++;
}

// ---- Inicializações ----
void initializeCAN() {
  CAN.setPins(15, 4); 
  Serial.println("Tentando inicializar o controlador CAN...");
  while (!CAN.begin(500E3)) {
    Serial.println("Falha ao iniciar o controlador CAN. Tentando novamente em 1 segundo...");
    delay(1000);
  }
  Serial.println("Controlador CAN inicializado com sucesso!");
}

void initializeSensors() {
  pinMode(wheelSensorPin, INPUT_PULLUP);  
  attachInterrupt(digitalPinToInterrupt(wheelSensorPin), handleWheelInterrupt, FALLING);

  pinMode(rpmPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(rpmPin), handleRPMInterrupt, FALLING);

  if (!mlx.begin()) {
  Serial.println("MLX falhou. Sensor desativado.");
  mlxPresent = false;
  } else {
    Serial.println("MLX90614 inicializado com sucesso!");
    mlxPresent = true;
  }

  Serial.println("Sensores inicializados.");
}

// ---- Processamentos ----
void processVelocity() {
  //detachInterrupt(digitalPinToInterrupt(wheelSensorPin));


   
  float rotations = pulseCountVel;
  float omega = (2.0 * PI * rotations) / (intervaloVelocidade / 1000.0);
  float speed = omega * wheelRadius;
  float speedKmh = speed * 3.6;
  int speedInt = (int)speedKmh;
   
  Serial.print("Velocidade: ");
  Serial.print(speedKmh);
  Serial.println(" km/h");

  CAN.beginPacket(CAN_ID_VELOCIDADE);
  CAN.write((speedInt >> 8) & 0xFF);
  CAN.write(speedInt & 0xFF);
  if (!CAN.endPacket()) {
    Serial.println("Falha ao enviar CAN de velocidade!");
  }
  pulseCountVel = 0;
  //attachInterrupt(digitalPinToInterrupt(wheelSensorPin), handleWheelInterrupt, FALLING);
}

void processRPM() {
  //detachInterrupt(digitalPinToInterrupt(rpmPin));



  unsigned long rpm = (pulseCountRPM * 60) / (intervaloRPM / 1000);

  Serial.print("RPM: ");
  Serial.println(rpm);

  CAN.beginPacket(CAN_ID_RPM);
  CAN.write((rpm >> 8) & 0xFF);
  CAN.write(rpm & 0xFF);
  if (!CAN.endPacket()) {
    Serial.println("Falha ao enviar CAN de RPM!");
  }

  pulseCountRPM = 0;
  //attachInterrupt(digitalPinToInterrupt(rpmPin), handleRPMInterrupt, FALLING);
}

void processTempFreio() {
  int tempFreio = thermocouple.readCelsius();
  
  Serial.print("Temp Freio: ");
  Serial.print(tempFreio);
  Serial.println(" °C");
  
  CAN.beginPacket(CAN_ID_FREIO);
  CAN.write((tempFreio >> 8) & 0xFF);
  CAN.write(tempFreio & 0xFF);
  if (!CAN.endPacket()) {
    Serial.println("Falha ao enviar CAN de freio!");
  }
}

void processTempCVT() {
 if (mlxPresent) {

  float tempCVT = mlx.readObjectTempC();

  Serial.print("Temp CVT: ");
  Serial.print(tempCVT);
  Serial.println(" °C");

  int16_t tempInt = (uint16_t)tempCVT;

  CAN.beginPacket(CAN_ID_CVT);
  CAN.write((tempInt >> 8) & 0xFF);
  CAN.write(tempInt & 0xFF);
  if (!CAN.endPacket()) {
    Serial.println("Falha ao enviar CAN de CVT!");
  }
 }
}

float leituraADC_oversampling(int pin) {
  const int N = 64;
  uint32_t soma = 0;

  for (int i = 0; i < N; i++) {
    soma += analogRead(pin);
  }

  return (float)soma / N;
}

float filtrarIIR(float entrada) {
  const float alpha = 0.12f;

  if (!filtroInicializado) {
    filtroBateria = entrada;
    filtroInicializado = true;
  }

  filtroBateria += alpha * (entrada - filtroBateria);
  return filtroBateria;
}

float lerTensaoBateria() {
  float adc_raw = leituraADC_oversampling(batteryPin);

  float tensao_adc = (adc_raw / ADC_RESOLUTION) * REF_VOLTAGE;
  float tensao_bat = tensao_adc * ((R1 + R2) / R2);

  return filtrarIIR(tensao_bat);
}

void processBatteryLevel() {
  float batteryVoltage = lerTensaoBateria();

  Serial.print("Bateria (filtrada): ");
  Serial.print(batteryVoltage, 3);
  Serial.println(" V");

  // Converte o float para 4 bytes
  union {
    float value;
    uint8_t bytes[4];
  } data;

  data.value = batteryVoltage;

  // Envia no CAN
  CAN.beginPacket(CAN_ID_BATERIA);
  CAN.write(data.bytes, 4);   // envia os 4 bytes do float
  if (!CAN.endPacket()) {
    Serial.println("Falha ao enviar CAN de bateria!");
  }
}
