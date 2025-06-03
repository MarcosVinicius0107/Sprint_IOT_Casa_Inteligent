const int PIR = 2;  // pino Digital que o PIR está plugado


void acenderLEDAoDetectarPresenca() {
  int estadoPIR = digitalRead(PIR);  // Lê o pino digital 2

  //INDENTAÇÃO (ATALHO) - SHIFT + ALT + F/
  // HIGH - VALE O VALOR DE 1
  //LOW - VALE O VALOR DE 0

  if (estadoPIR == HIGH) {
    Serial.println("LED ligado");
  } else {
    Serial.println("LED apagado");
  }
}

void setup() {
  Serial.begin(9600);

  Serial.println("Calibrando os sensores");
  delay(10000);
  Serial.println("Sensores calibrados!!!;)Pode testar hehe");
}

void loop() {
  // As instruções no loop será somente chamada de funções
  acenderLEDAoDetectarPresenca();
}
