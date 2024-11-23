#include <CAN.h>  // Inclui a biblioteca CAN

// Definições e variáveis para o RPM DO MOTOR
const int rpmPin = 13;  // Pino do sensor indutivo
volatile unsigned long pulseCount = 0;  // Contador de pulsos
unsigned long lastTime = 0;
const unsigned long interval = 1000;  // Intervalo de tempo para cálculo do RPM (1 segundo)

void setup() {
  Serial.begin(115200);  // Inicia a comunicação serial
  while (!Serial);

  initializeCAN();  // Configura a comunicação CAN
  initializeSensors();  // Configura o sensor indutivo
}

void loop() {
  processRPM();  // Processa o cálculo e envio do RPM
}

// Função de interrupção para contar pulsos do sensor indutivo
void IRAM_ATTR handleInterrupt() {
  pulseCount++;
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
    
    sendRPMViaCAN(rpm);  // Envia o RPM via CAN
    
    pulseCount = 0;  // Reseta o contador de pulsos
    lastTime = currentTime;  // Atualiza o tempo
    
    attachInterrupt(digitalPinToInterrupt(rpmPin), handleInterrupt, FALLING);  // Reabilita a interrupção
  }
}

// Envia o RPM via CAN
void sendRPMViaCAN(unsigned long rpm) {
  CAN.beginPacket(0x15);  // ID em hexadecimal
  CAN.write((rpm >> 8) & 0xFF);  // Envia os 8 bits mais significativos
  CAN.write(rpm & 0xFF);  // Envia os 8 bits menos significativos
  CAN.endPacket();  // Encerra o pacote
  Serial.println("RPM enviado via CAN.");
}
