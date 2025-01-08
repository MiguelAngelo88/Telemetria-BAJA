#include <CAN.h>  // Inclui a biblioteca CAN
#include "max6675.h" //Sensor da temp do freio

//Configuração do watchdog
hw_timer_t *wdTimer = NULL;

// Definições e variáveis para o RPM DO MOTOR
#define  rpmPin  13  // Pino do sensor indutivo
volatile unsigned long pulseCount = 0;  // Contador de pulsos
unsigned long lastTime = 0;
const unsigned long interval = 1000;  // Intervalo de tempo para cálculo do RPM (1 segundo)

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

  configureWatchdog();// Configura o watchdog
  initializeCAN();  // Configura a comunicação CAN
  initializeSensors();  // Configura o sensor indutivo
}

void loop() {
  timerWrite(wdTimer, 0);// reseta o timer do watchdog

  processRPM();  // Processa o cálculo e envio do RPM
  processTempFreio(); // Processa o cálculo e envio da Temp do freio
  processBatteryLevel(); // Processa o cálculo e envio do nível da bateria

  delay(500);
}

//Função que reinicia o ESP
void IRAM_ATTR resetModule(){
  ets_printf("(watchdog) reiniciar\n");
  //esp_restart_noos();//reinicia o chip
  esp_restart();
}

// Função de interrupção para contar pulsos do sensor indutivo
void IRAM_ATTR handleInterrupt() {
  pulseCount++;
}

//Configura o watchdog
void configureWatchdog(){
  wdTimer = timerBegin(0,80, true); //timerID 0, 80 MHz
  timerAttachInterrupt(wdTimer, &resetModule, true);
  timerAlarmWrite(wdTimer, 3000000, true); //3000000 us = 3s
  timerAlarmEnable(wdTimer);
}

// Inicializa a comunicação CAN
void initializeCAN() {
  Serial.println("Inicializando o Transmissor CAN");
  if (!CAN.begin(500E3)) {
    Serial.println("Falha ao iniciar o controlador CAN");
  }
}

// Inicializa os sensores e configura a interrupções
void initializeSensors() {
  //RPM
  pinMode(rpmPin, INPUT_PULLUP);  
  attachInterrupt(digitalPinToInterrupt(rpmPin), handleInterrupt, FALLING);
}

// Função principal do loop para calcular e enviar o RPM
void processRPM() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastTime >= interval) {
    detachInterrupt(digitalPinToInterrupt(rpmPin));  // Desabilita a interrupção temporariamente
    
    unsigned long rpm = (pulseCount * 60) / (interval / 1000); // Calcula o RPM
    
    Serial.print("RPM: ");
    Serial.println(rpm);
    
    // Envia o RPM via CAN
    CAN.beginPacket(0x15);  // ID em hexadecimal
    CAN.write((rpm >> 8) & 0xFF);  // Envia os 8 bits mais significativos
    CAN.write(rpm & 0xFF);  // Envia os 8 bits menos significativos
    CAN.endPacket();  // Encerra o pacote
    Serial.println("RPM enviado via CAN.");
    
    pulseCount = 0;  // Reseta o contador de pulsos
    lastTime = currentTime;  // Atualiza o tempo
    
    attachInterrupt(digitalPinToInterrupt(rpmPin), handleInterrupt, FALLING);  // Reabilita a interrupção
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
