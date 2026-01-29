#include <LiquidCrystal.h>

// Inicializacion del LCD (RS, EN, D4, D5, D6, D7)
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

const int pingPin = 43;

void setup(){
    lcd.begin(16,2);
    lcd.setCursor(0,0);
    lcd.print("prueba ultrasonido");
    delay(1500);
    lcd.clear();
}

void loop(){
    int botones = analogRead(A0);
    long duration, cm;
    //lectura ultrasonido
    pinMode(pingPin, OUTPUT);
    digitalWrite(pingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(pingPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(pingPin, LOW);
    pinMode(pingPin, INPUT);
    duration = pulseIn(pingPin, HIGH);
    cm = microsecacm(duration);

    //Muestra en pantalla
    lcd.setCursor(0,0);
    lcd.print("Distancia");
    lcd.setCursor(0,1);
    lcd.print("Dist: ");
    lcd.print(cm);
    lcd.print(" cm      ");

    delay(100);
    
}
long microsecacm(long microseconds) {
  return microseconds / 29 / 2;
}