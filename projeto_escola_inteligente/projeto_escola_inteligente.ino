//bibliotecas
#include <DHT11.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPINO A2 //definimos qual será o pino
#define DHTTYPE DHT11 //Variavel do tipo DHT

DHT dht(DHTPINO, DHT11);//Inicializa o objeto DHT com o pino e o tipo


//Variaveis globais sao as funcoes acenderLEDAoDetectarPresenca
const int PIR = 2;  // pino Digital que o PIR está plugado
const int ledAmarelo = 13;

//Variaveis globais sao as funcoes verificarVazamentoDeGas
const int MQ135 = A0; // pino Analogico do mq-135
const int buzzer = 12; // pino Digital
/*
void acenderLEDAoDetectarPresenca() {
int estadoPIR = digitalRead(PIR);  // Lê o pino digital 2
}
*/
void alarme_dois_tons() {
int freqAlta = 2000;
int freqBaixa = 800;
int duracaoTom = 250;

tone(buzzer, freqAlta, duracaoTom);
delay(duracaoTom);
tone(buzzer, freqBaixa, duracaoTom);
delay(duracaoTom);
}
//INDENTAÇÃO (ATALHO) - SHIFT + ALT + F/
// HIGH - VALE O VALOR DE 1
//LOW - VALE O VALOR DE 0
/*
  if (estadoPIR == HIGH) {
digitalWrite(ledAmarelo, HIGH); //Fala ao programa para o pino 13 "ACENDER"
Serial.println("LED ligado");
 }
  else {
digitalWrite(ledAmarelo, LOW); //Fala ao programa para o pino 13 "DESLIGAR"
Serial.println("LED apagado");
 }
}
*/


void verificarVazamentoDeGas(){
int estadoMQ135 = analogRead(MQ135);
//if/ - verificar a intensidade do valor do estadoMQ135 >= 600
//sim - ativar o alarme
//nao - desativar o alarme
if (estadoMQ135 >= 300){
 alarme_dois_tons();
}
else{
  noTone(buzzer);
}


Serial.println(estadoMQ135);


}

void verificarTeperaturaEUmidade(){
  float umidade = dht.readHumidity(); // Lê a umidade
  float temperatura = dht.readTemperature();// Lê a temperatura em Celsius
  
  Serial.println("Umidade: " + String(umidade) + "%");
  Serial.println("Temperatura: " + String(temperatura) + "C");
  delay(7000);
}


void setup() {
Serial.begin(9600);

//Fala ao programa que o pino 13 será uma saída/output
pinMode(ledAmarelo, OUTPUT);
pinMode(MQ135, INPUT);
pinMode(buzzer, OUTPUT);

//Inicializa o sensor DHT
dht.begin();

Serial.println("Calibrando os sensores");
delay(10000);
Serial.println("Sensores calibrados!!!;)Pode testar hehe");
}

void loop() {
// As instruções no loop será somente chamada de funções
// acenderLEDAoDetectarPresenca();
//verificarVazamentoDeGas();
verificarTeperaturaEUmidade();
}