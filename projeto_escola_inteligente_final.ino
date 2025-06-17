//bibliotecas
#include <DHT11.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>
//#include <Servo.h>
#include <ESP32Servo.h>
#include <PubSubClient.h>
#include <WiFi.h>

// --- WiFi & MQTT ---
const char* ssid = "ProjetosIoT_Esp32";         //sua rede wifi
const char* password = "senai@134";             //senha da sua rede wifi
const char* mqtt_server = "broker.hivemq.com";  //endereço do broker público
const int mqtt_port = 1883;                     //porta do broker público, geralmente 1883

//Tópicos
const char* topic_led2 = "babytechhome/quarto2/LED";
const char* topic_led = "babytechhome/quarto/LED";
const char* topic_umid = "babytechhome/quarto/umidade";
const char* topic_temp = "babytechhome/quarto/tmperatura";
const char* topic_porta = "babytechhome/sala/porta";
//const char* topic_buzzer = "babytchhome/quarto2/buzzer";

//Variaveis globais sao da funcao acenderLEDAoDeclararPresenca
const int PIR = 5;  //Pino Digital que o PIR está plugado
const int ledQuarto = 4;
const int ledQuarto2 = 14;
const int buzzer = 18;
const int rele = 15;
#define DHTPINO 23    //Definimos qual sera o pino pars o sensor
#define DHTIPO DHT11  //Variavel do tipo DHT
float temperatura;
float umidade;

DHT dht(DHTPINO, DHTIPO);  //inicializa o objeto DHT com o pino e o tipo
LiquidCrystal_I2C lcd(0x27, 20, 4);
Servo motorPorta;
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long ultimaLeitura = 0;

// --- Funções WiFi e MQTT ---
void conectarWiFi() {  //verifica conexão wifi para somente depois iniciar o sistema
  Serial.println("Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
}

void reconectarMQTT() {  //verifica e reconecta a conexão com o broker mqtt
  while (!client.connected()) {
    Serial.print("Reconectando MQTT...");
    if (client.connect("ESP32ClientTest")) {
      Serial.println("Conectado!");
      client.subscribe(topic_led);    //conecta ao topico do led assim que estabelecer ligação com o broker
      client.subscribe(topic_porta);  //conecta ao topico da porta assim que estabelecer ligação com o broker
    } else {
      Serial.print("Falha: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

/**
  Função para tratamento das mensagens de callback/retorno do broker de cada tópico subscrito (led, porta, etc.)

  char* é uma cadeia de caracteres em C como um vetor onde cada caractter/letra está em uma posição, 
  diferente de uma String em C++ que pode ser lida completamente
*/
void tratarMensagem(char* topic, byte* payload, unsigned int length) {  //
  String mensagem = "";
  for (int i = 0; i < length; i++) {  //concatena todas os char* para se ter o texto completo em String
    mensagem += (char)payload[i];
  }

  Serial.printf("Mensagem recebida [%s]: %s\n", topic, mensagem.c_str());

  //led - luz quarto
  if (strcmp(topic, topic_led) == 0) {  //tópico atual é o do led?
    if (mensagem == "ligar") {
      digitalWrite(ledQuarto, HIGH);
    } else if (mensagem == "desligar") {
      digitalWrite(ledQuarto, LOW);
    }
  }

  //led - luz quarto2
  if (strcmp(topic, topic_led) == 0) {  //tópico atual é o do led?
    if (mensagem == "ligar") {
      digitalWrite(ledQuarto2, HIGH);
    } else if (mensagem == "desligar") {
      digitalWrite(ledQuarto2, LOW);
    }
  }

  /*
    Verifica se o tópico recebido é o topico da porta
  é uma função da linguagem C que compara duas strings (topic e topic_porta)
  */
  //porta
  if (strcmp(topic, topic_porta) == 0) {  //tópico atual é o da porta?
    if (mensagem == "abrir") {
      delay(500);
      abrirPorta();
      delay(500);
    } else if (mensagem == "fechar") {
      delay(500);
      fecharPorta();
      delay(500);
    }
  }
}

/*void acenderLEDAoDetectarPresenca() {
  int estadoPIR = digitalRead(PIR);  //Le o pino Digital 2

  //INDENTACAO (ATALHO) - SHIFT + ALT + F
  // HIGH - VALE O VALOR DE 1
  // LOW - VALE O VALOR DE 0

  if (estadoPIR == HIGH) {
    digitalWrite(ledVerde, HIGH);//Fala ao programa para o pino 13 "ACENDER"
    Serial.println("LED ligado");
    
  } else {
    digitalWrite(ledVerde, LOW);//Fala ao programa para o pino 13 "ACENDER"
    Serial.println("LED apagado");
    
  }
  delay(1000);
}*/

void alarme_dois_tons() {
  int freqAlta = 2000;
  int freqBaixa = 800;
  int duracaoTom = 300;

  // Repete a sequência de tons por um período mais longo
  for (int i = 0; i < 2; i++) {
    tone(buzzer, freqAlta, duracaoTom);
    tone(buzzer, freqBaixa, duracaoTom);
    delay(500);
    tone(buzzer, freqAlta, duracaoTom);
    tone(buzzer, freqBaixa, duracaoTom);
  }
  noTone(buzzer);  // Desliga o buzzer ao final
}


/*void verificarVazamentoDeGas() {
 int estadoMQ135 = analogRead(MQ135);
 Serial.println("Concentracao de gas: " + String(estadoMQ135));
 
 //if - verificar a intensidade do valor do estadoMQ135 >= 600
 //sim - ativar o alarme
 //nao - desativar alarme - noTone(buzzer);

  if (estadoMQ135 >= 400) {
    alarme_dois_tons();
  }
  else{
    noTone(buzzer);
  }
  delay(1000);
}*/

void verificarTemperaturaEUmidade() {
  temperatura = dht.readTemperature();  //lê a temperatura do sensor
  umidade = dht.readHumidity();         // lê a umidade do sensor

  if (isnan(temperatura) || isnan(umidade)) {  //avisa no console se deu erro
    Serial.println("Erro ao ler DHT!");
    return;
  }

  Serial.printf("Temp: %.1f °C | Umid: %.1f %%\n", temperatura, umidade);  //mostra temperatura e umidade no console

  char tempStr[10], umidStr[10];
  dtostrf(temperatura, 4, 1, tempStr);  //converte o valor da temperatura do sensor que para string (ele vem float do sensor)
  dtostrf(umidade, 4, 1, umidStr);      //converte o valor da umidade do sensor que para string (ele vem float do sensor)
  client.publish(topic_temp, tempStr);  //publica a temperatura no tópico, lá no Broker Server
  client.publish(topic_umid, umidStr);  //publica a umidade no tópico, lá no Broker Server

  if (temperatura <= 21.00 || temperatura >= 24.00) {
    alarme_dois_tons();
  } else {
    noTone(buzzer);
  }

  lcd.clear();  //limpa o blacklight do lcd
  lcd.setCursor(0, 0);
  lcd.print("Umidade: " + String(umidade) + "%");
  lcd.setCursor(0, 1);
  lcd.print("Temperatura: " + String(temperatura) + "C");

  delay(2000);
}

void abrirPorta() {
  motorPorta.write(20);
  delay(5000);
}

void fecharPorta() {
  motorPorta.write(182);
  delay(5000);
}

/*void trancarPorta(){
  digitalWrite(rele, LOW);
  Serial.println("Porta trancada");
  delay(2000);
}

void destrancarPorta(){
  digitalWrite(rele, HIGH);
  Serial.println("Porta destrancada");
  delay(2000);
}*/

void setup() {
  Serial.begin(115200);

  //motor.attach(servoMotor);
  pinMode(buzzer, OUTPUT);
  //pinMode(rele, OUTPUT);
  pinMode(ledQuarto, OUTPUT);
  pinMode(ledQuarto2, OUTPUT);

  //
  //digitalWrite(rele, LOW);

  //
  motorPorta.write(182);
  motorPorta.attach(26);
  //
  dht.begin();

  lcd.init();       //lcd.begin();
  lcd.backlight();  //liga o lcd

  conectarWiFi();                            //conecta no wifi
  client.setServer(mqtt_server, mqtt_port);  //conecta no broker server
  client.setCallback(tratarMensagem);        //trata as mensagens recebidas do broker

  Serial.println("Calibrando os sensores");



  delay(1500);
  Serial.print(".");
  delay(1500);
  Serial.print(".");
  delay(1500);
  Serial.print(".");
  delay(1500);
  Serial.print(".");
  delay(1500);
  Serial.println(".");
  delay(1500);
  Serial.println("Sensores calibrados!!!");
  delay(500);
  Serial.println("  *TESTE LIBERADO*");
}

void loop() {
  if (!client.connected()) reconectarMQTT();  //se não tem conexão com o broker, tenta reconectar
  client.loop();                              //mantém a conexão com o broker serve sempre aberta

  //acenderLEDAoDetectarPresenca();

  //verificarVazamentoDeGas();

  abrirPorta();
  

  verificarTemperaturaEUmidade();

  delay(10000);

  fecharPorta();

  //destrancarPorta();

  //trancarPorta();

  if (millis() - ultimaLeitura > 5000) {
    ultimaLeitura = millis();        //contador de milisegundos
    verificarTemperaturaEUmidade();  //Dados do sensor de umidade e temperatura
  }
}