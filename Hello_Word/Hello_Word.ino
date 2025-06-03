// C++ code
//
void setup()
{Serial.begin(9600); // Confugura a porta serial do Arduino

}

void loop()
{
  Serial.print("Hello, Word! "); // Escreve na porta serial
 
  delay(1000); // Wait for 1000 millisecond(s)
}