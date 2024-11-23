#include <CAN.h>  // Inclui a biblioteca CAN

const int sensorPin = 13;  // Pino do sensor indutivo
volatile unsigned long pulseCount = 0;  // Contador de pulsos
unsigned long lastTime = 0;
const unsigned long interval = 1000;  // Intervalo de tempo para cálculo do RPM (1 segundo)

void IRAM_ATTR handleInterrupt() {
  pulseCount++;  // Incrementa o contador de pulsos na interrupção
}

void setup() {
  Serial.begin(115200);  // Inicia a comunicação serial
  while (!Serial);

  Serial.println("Inicializando o Transmissor CAN");

  pinMode(sensorPin, INPUT_PULLUP);  // Configura o pino do sensor como entrada com pull-up
  attachInterrupt(digitalPinToInterrupt(sensorPin), handleInterrupt, FALLING);  // Configura a interrupção na borda de descida

  // Inicia o barramento CAN a 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Falha ao iniciar o controlador CAN");
    while (1);
  }
}

void loop() {
  unsigned long currentTime = millis();

  // Calcula o RPM a cada segundo
  if (currentTime - lastTime >= interval) {
    detachInterrupt(digitalPinToInterrupt(sensorPin));  // Desabilita a interrupção temporariamente

    // Calcula o RPM
    unsigned long rpm = (pulseCount * 60) / (interval / 1000);
   
    // Converte o RPM para string
    String rpmStr = String(rpm);

    // Exibe o RPM no monitor serial
    Serial.print("RPM: ");
    Serial.println(rpmStr);

    // Envia o RPM via CAN
    Serial.println("Enviando RPM via CAN...");

    CAN.beginPacket(0x15);  // ID 18 em hexadecimal
    CAN.write((rpm >> 8) & 0xFF);  // Envia os 8 bits mais significativos do RPM
    CAN.write(rpm & 0xFF);  // Envia os 8 bits menos significativos do RPM
    CAN.endPacket();  // Encerra o pacote para envio

    Serial.println("RPM enviado.");

    pulseCount = 0;  // Reseta o contador de pulsos
    lastTime = currentTime;  // Atualiza o tempo

    attachInterrupt(digitalPinToInterrupt(sensorPin), handleInterrupt, FALLING);  // Reabilita a interrupção
  }
}
