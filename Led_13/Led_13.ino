int ledInterno = 13;
void setup() {
pinMode(13, OUTPUT);
}

void loop() {
digitalWrite(ledInterno, HIGH);
delay(500);
digitalWrite(ledInterno, LOW);
delay(500);
}