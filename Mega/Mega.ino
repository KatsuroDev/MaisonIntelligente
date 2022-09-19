/*
 * MEGA PASSERELLE
 * 
 * Alexis Millette 19-09-2022
 * Serial2 sur les PINS 16-17
 * HM-10 (BLE) connecté Serial3
 */

char charMaison = 0;
char charConsole = 0;
char charBluetooth = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial2.available()) {
    charMaison = Serial2.read();
    Serial.print("De la maison: ");
    Serial.println(charMaison);
  }
  if (Serial.available()) {
    charConsole = Serial.read();
    Serial.print("De la console: ");
    Serial.println(charConsole);
    Serial2.write(charConsole);
  }
  if (Serial3.available()) {
    charBluetooth = Serial3.read();
    Serial.print("De la console: ");
    Serial.println(charBluetooth);
    Serial2.write(charBluetooth);
  }
}
