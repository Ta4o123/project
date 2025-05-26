// Пины подключения
#define TRIG_PIN 3
#define ECHO_PIN 4
#define IR_SENSOR_PIN 8
#define PUMP_PIN 9
#define MOTOR_DRIVER_ENABLE_LEFT 5
#define MOTOR_DRIVER_INPUT1_LEFT 6
#define MOTOR_DRIVER_INPUT2_LEFT 7
#define MOTOR_DRIVER_ENABLE_RIGHT 10
#define MOTOR_DRIVER_INPUT1_RIGHT 11
#define MOTOR_DRIVER_INPUT2_RIGHT 12
#define RELAY_PIN 14
#define SWITCHER_PIN 15
#define RADIO_RECEIVER_PIN 13

// Функция для измерения расстояния с ультразвукового датчика
long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2; // Переводим в сантиметры
  return distance;
}

// Функция для проверки наличия сигнала от радио сенсора
bool isFlameDetected() {
  return digitalRead(RADIO_RECEIVER_PIN) == HIGH;
}

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(MOTOR_DRIVER_ENABLE_LEFT, OUTPUT);
  pinMode(MOTOR_DRIVER_INPUT1_LEFT, OUTPUT);
  pinMode(MOTOR_DRIVER_INPUT2_LEFT, OUTPUT);
  pinMode(MOTOR_DRIVER_ENABLE_RIGHT, OUTPUT);
  pinMode(MOTOR_DRIVER_INPUT1_RIGHT, OUTPUT);
  pinMode(MOTOR_DRIVER_INPUT2_RIGHT, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(SWITCHER_PIN, INPUT_PULLUP);
  pinMode(RADIO_TRANSMITTER_PIN, OUTPUT);
  pinMode(RADIO_RECEIVER_PIN, INPUT);

  digitalWrite(PUMP_PIN, LOW); // Отключаем насос при старте
  digitalWrite(MOTOR_DRIVER_ENABLE_LEFT, LOW);
  digitalWrite(MOTOR_DRIVER_INPUT1_LEFT, LOW);
  digitalWrite(MOTOR_DRIVER_INPUT2_LEFT, LOW);
  digitalWrite(MOTOR_DRIVER_ENABLE_RIGHT, LOW);
  digitalWrite(MOTOR_DRIVER_INPUT1_RIGHT, LOW);
  digitalWrite(MOTOR_DRIVER_INPUT2_RIGHT, LOW);
  digitalWrite(RELAY_PIN, LOW); // Отключаем реле при старте

  Serial.begin(9600);
}

void moveForward() {
  digitalWrite(MOTOR_DRIVER_INPUT1_LEFT, HIGH);
  digitalWrite(MOTOR_DRIVER_INPUT2_LEFT, LOW);
  digitalWrite(MOTOR_DRIVER_ENABLE_LEFT, HIGH);
  digitalWrite(MOTOR_DRIVER_INPUT1_RIGHT, HIGH);
  digitalWrite(MOTOR_DRIVER_INPUT2_RIGHT, LOW);
  digitalWrite(MOTOR_DRIVER_ENABLE_RIGHT, HIGH);
}

void moveBackward() {
  digitalWrite(MOTOR_DRIVER_INPUT1_LEFT, LOW);
  digitalWrite(MOTOR_DRIVER_INPUT2_LEFT, HIGH);
  digitalWrite(MOTOR_DRIVER_ENABLE_LEFT, HIGH);
  digitalWrite(MOTOR_DRIVER_INPUT1_RIGHT, LOW);
  digitalWrite(MOTOR_DRIVER_INPUT2_RIGHT, HIGH);
  digitalWrite(MOTOR_DRIVER_ENABLE_RIGHT, HIGH);
}

void stopMotors() {
  digitalWrite(MOTOR_DRIVER_ENABLE_LEFT, LOW);
  digitalWrite(MOTOR_DRIVER_ENABLE_RIGHT, LOW);
}

void loop() {
  // Проверяем состояние переключателя
  int switchState = digitalRead(SWITCHER_PIN);
  if (switchState == LOW) { // LOW означает, что переключатель включен
    Serial.println("Робот активирован.");

    // Считываем расстояние до препятствия
    long distance = getDistance();
    Serial.print("Расстояние: ");
    Serial.print(distance);
    Serial.println(" см");

    // Проверяем наличие огня с помощью радио сенсора
    if (isFlameDetected()) {
      Serial.println("Огонь обнаружен!");

      if (distance > 15) {
        Serial.println("Двигаемся к огню.");
        moveForward(); // Двигаемся к огню
      } else {
        Serial.println("Огонь в пределах досягаемости, начинаем тушение.");
        stopMotors(); // Останавливаемся перед тушением
        digitalWrite(PUMP_PIN, HIGH); // Включаем насос
        digitalWrite(RELAY_PIN, HIGH); // Включаем реле
        delay(5000); // Тушим огонь в течение 5 секунд
        digitalWrite(PUMP_PIN, LOW); // Выключаем насос
        digitalWrite(RELAY_PIN, LOW); // Выключаем реле
      }
    } else {
      digitalWrite(PUMP_PIN, LOW); // Выключаем насос
      digitalWrite(RELAY_PIN, LOW); // Выключаем реле

      if (distance < 20) { // Если препятствие близко
        Serial.println("Препятствие обнаружено! Двигаемся назад.");
        moveBackward();
        delay(500); // Двигаемся назад полсекунды
        stopMotors();
        delay(200);
      } else {
        Serial.println("Двигаемся вперед.");
        moveForward();
      }
    }
  } else {
    Serial.println("Робот отключен.");
    stopMotors();
    digitalWrite(PUMP_PIN, LOW);
    digitalWrite(RELAY_PIN, LOW);
  }

  delay(100); // Небольшая задержка для стабильности
}