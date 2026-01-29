#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

const int rele = 50;

void setup() {
  lcd.begin(16, 2);
  pinMode(rele, OUTPUT);
  digitalWrite(rele, HIGH);//apagar el relay
  lcd.print("prueba motor");
  delay(1500);
  lcd.clear();
}

void loop() {
  int boton = analogRead(A0);
  lcd.setCursor(0, 0);
  lcd.print("der:prender");
  lcd.setCursor(0, 1);
  lcd.print("izq:apagar");
  if(boton <10 ){
    digitalWrite(rele, LOW);
  }else if(boton < 550 && boton > 500){
    digitalWrite(rele, HIGH);
  }
  delay(200);
}
