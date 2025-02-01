#include <CAN.h>  // Inclui a biblioteca CAN
#include <Adafruit_MLX90614.h>

// Definições e variáveis para o RPM DO MOTOR
#define  rpmPin  13  // Pino do sensor indutivo
volatile unsigned long pulseCount = 0;  // Contador de pulsos
unsigned long lastTime = 0;
const unsigned long interval = 1000;  // Intervalo de tempo para cálculo do RPM (1 segundo)

// Definições e variáveis para a TEMPERATURA DA CVT
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// --- Definições e variáveis para o NÍVEL DE COMBUSTÍVEL ---
const int sensor1Pin = 15; // Sensor inferior
const int sensor2Pin = 16; // Sensor intermediário
const int sensor3Pin = 17; // Sensor superior
const unsigned long debounceDelay = 50; // Tempo de debounce em milissegundos
struct SensorData {
  int pin;
  bool state;                  // Estado estável do sensor
  bool lastReading;            // Última leitura bruta
  unsigned long lastDebounceTime; // Último tempo em que ocorreu mudança
};
SensorData sensor1 = {sensor1Pin, LOW, LOW, 0};
SensorData sensor2 = {sensor2Pin, LOW, LOW, 0};
SensorData sensor3 = {sensor3Pin, LOW, LOW, 0};

void setup() {
  Serial.begin(115200);  // Inicia a comunicação serial
  while (!Serial);

  initializeCAN();  // Configura a comunicação CAN
  initializeSensors();  // Configura o sensor indutivo
}

void loop() {

  processRPM();  // Processa o cálculo e envio do RPM
  processTempCVT(); // Processa o cálculo e envio da Temp da CVT
  processNivelCombustivel(); // Processa o cálculo e envio do nível do combustível

  delay(500);
}

void IRAM_ATTR handleInterrupt() {
  pulseCount++; // Incrementa o contador de pulsos na interrupção
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
  pinMode(rpmPin, INPUT_PULLUP);  
  attachInterrupt(digitalPinToInterrupt(rpmPin), handleInterrupt, FALLING);
  mlx.begin();

  pinMode(sensor1.pin, INPUT_PULLUP);
  pinMode(sensor2.pin, INPUT_PULLUP);
  pinMode(sensor3.pin, INPUT_PULLUP);

  sensor1.state = digitalRead(sensor1.pin);
  sensor2.state = digitalRead(sensor2.pin);
  sensor3.state = digitalRead(sensor3.pin);

  sensor1.lastReading = sensor1.state;
  sensor2.lastReading = sensor2.state;
  sensor3.lastReading = sensor3.state;

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
    CAN.beginPacket(0x18);  // ID em hexadecimal
    CAN.write((rpm >> 8) & 0xFF);  // Envia os 8 bits mais significativos
    CAN.write(rpm & 0xFF);  // Envia os 8 bits menos significativos
    CAN.endPacket();  // Encerra o pacote
    Serial.println("RPM enviado via CAN.");
    
    pulseCount = 0;  // Reseta o contador de pulsos
    lastTime = currentTime;  // Atualiza o tempo
    
    attachInterrupt(digitalPinToInterrupt(rpmPin), handleInterrupt, FALLING);  // Reabilita a interrupção
  }
}

void processTempCVT() {
    float tempCVT = mlx.readObjectTempC(); // Lê a temperatura do sensor

    Serial.print("Temperatura da CVT: ");
    Serial.print(tempCVT);
    Serial.println(" *C");

    // Converte a temperatura para um formato adequado para envio via CAN
    int16_t tempInt = static_cast<int16_t>(tempCVT * 100); // Multiplica por 100 para manter duas casas decimais

    // Envia a temperatura via CAN
    CAN.beginPacket(0x19);  // ID da mensagem CAN para temperatura da CVT
    CAN.write((tempInt >> 8) & 0xFF);  // Envia os 8 bits mais significativos
    CAN.write(tempInt & 0xFF);  // Envia os 8 bits menos significativos
    CAN.endPacket();  // Encerra o pacote

    Serial.println("Temperatura da CVT enviada via CAN.");
}

void updateSensor(SensorData &sensor) {
  unsigned long currentTime = millis();
  bool reading = digitalRead(sensor.pin);

  if (reading != sensor.lastReading) {
    sensor.lastDebounceTime = currentTime;
  }

  if ((currentTime - sensor.lastDebounceTime) > debounceDelay) {
    sensor.state = reading;
  }

  sensor.lastReading = reading;
}

String getFuelLevel(bool s1, bool s2, bool s3) {
  if (!s1 && !s2 && !s3) {
    return "Tanque vazio";
  } else if (s1 && !s2 && !s3) {
    return "Nível muito baixo";
  } else if (s1 && s2 && !s3) {
    return "Nível médio";
  } else if (s1 && s2 && s3) {
    return "Tanque cheio ou nível alto";
  } else {
    return "Estado indefinido";
  }
}

// Função para processar o nível de combustível
void processNivelCombustivel() {
  updateSensor(sensor1);
  updateSensor(sensor2);
  updateSensor(sensor3);

  String fuelLevel = getFuelLevel(sensor1.state, sensor2.state, sensor3.state);
  
  Serial.print("Nivel de Combustível: ");
  Serial.println(fuelLevel);

  // Envia o nível de combustível via CAN
  uint8_t fuelLevelCode = 0;
  if (fuelLevel == "Tanque vazio") fuelLevelCode = 0;
  else if (fuelLevel == "Nível muito baixo") fuelLevelCode = 1;
  else if (fuelLevel == "Nível médio") fuelLevelCode = 2;
  else if (fuelLevel == "Tanque cheio ou nível alto") fuelLevelCode = 3;

  // Envia o código do nível de combustível via CAN
  CAN.beginPacket(0x20);  // ID da mensagem CAN para nível de combustível
  CAN.write(fuelLevelCode);  // Envia o código do nível de combustível
  CAN.endPacket();  // Encerra o pacote

  Serial.println("Nivel de Combustível enviado via CAN.");
}
