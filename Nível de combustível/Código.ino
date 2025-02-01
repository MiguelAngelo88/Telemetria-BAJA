// --- Configurações de pinos e debounce ---
const int sensor1Pin = 15; // Sensor inferior
const int sensor2Pin = 16; // Sensor intermediário
const int sensor3Pin = 17; // Sensor superior

const unsigned long debounceDelay = 50; // Tempo de debounce em milissegundos

// Estruturas para armazenar estado e tempo de debounce de cada sensor
struct SensorData {
  int pin;
  bool state;                  // Estado estável do sensor
  bool lastReading;            // Última leitura bruta
  unsigned long lastDebounceTime; // Último tempo em que ocorreu mudança
};

// Inicializa os sensores com os pinos respectivos
SensorData sensor1 = {sensor1Pin, LOW, LOW, 0};
SensorData sensor2 = {sensor2Pin, LOW, LOW, 0};
SensorData sensor3 = {sensor3Pin, LOW, LOW, 0};

// --- Função para inicializar os sensores ---
void initSensors() {
  // Configure os pinos como entrada com resistor de pull-up (ajuste conforme a documentação do sensor)
  pinMode(sensor1.pin, INPUT_PULLUP);
  pinMode(sensor2.pin, INPUT_PULLUP);
  pinMode(sensor3.pin, INPUT_PULLUP);

  // Inicializa os estados com as leituras atuais
  sensor1.state = digitalRead(sensor1.pin);
  sensor2.state = digitalRead(sensor2.pin);
  sensor3.state = digitalRead(sensor3.pin);

  sensor1.lastReading = sensor1.state;
  sensor2.lastReading = sensor2.state;
  sensor3.lastReading = sensor3.state;
}

// --- Função para realizar a leitura com debounce de um sensor ---
// A função recebe uma referência da estrutura SensorData e atualiza seu estado.
void updateSensor(SensorData &sensor) {
  unsigned long currentTime = millis();
  bool reading = digitalRead(sensor.pin);

  // Se houve mudança na leitura, reinicia o contador de debounce
  if (reading != sensor.lastReading) {
    sensor.lastDebounceTime = currentTime;
  }
  
  // Se o tempo desde a última mudança é maior que o debounceDelay, atualiza o estado
  if ((currentTime - sensor.lastDebounceTime) > debounceDelay) {
    sensor.state = reading;
  }
  
  // Armazena a leitura atual para a próxima verificação
  sensor.lastReading = reading;
}

// --- Função para interpretar o nível de combustível com base nos estados dos sensores ---
String getFuelLevel(bool s1, bool s2, bool s3) {
  /*
     Interpretação dos estados:
       - Nenhum sensor ativo: Tanque vazio
       - Apenas sensor 1 ativo: Nível muito baixo
       - Sensores 1 e 2 ativos: Nível médio
       - Todos os sensores ativos: Tanque cheio ou nível alto
       - Outros estados: Estado indefinido (possível falha ou ruído)
  */
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

// --- Função para imprimir o estado dos sensores e o nível de combustível ---
void printFuelStatus() {
  String fuelLevel = getFuelLevel(sensor1.state, sensor2.state, sensor3.state);

  Serial.print("Sensor 1: ");
  Serial.print(sensor1.state ? "Ativo " : "Inativo ");
  Serial.print("| Sensor 2: ");
  Serial.print(sensor2.state ? "Ativo " : "Inativo ");
  Serial.print("| Sensor 3: ");
  Serial.print(sensor3.state ? "Ativo " : "Inativo ");
  Serial.print(" => Nivel de Combustivel: ");
  Serial.println(fuelLevel);
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Aguarda a inicialização do monitor serial

  // Inicializa os sensores
  initSensors();
  
  Serial.println("Sistema de medicao de nivel de combustivel iniciado...");
}

void loop() {
  // Atualiza o estado de cada sensor com debounce
  updateSensor(sensor1);
  updateSensor(sensor2);
  updateSensor(sensor3);
  
  // Exibe o status atual no Monitor Serial
  printFuelStatus();
  
  // Aguarda um curto intervalo para a próxima leitura (ajuste conforme necessário)
  delay(200);
}

