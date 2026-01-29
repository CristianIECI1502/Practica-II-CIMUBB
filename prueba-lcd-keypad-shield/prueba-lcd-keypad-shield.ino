#include <LiquidCrystal.h>

// Inicializacion del LCD (RS, EN, D4, D5, D6, D7)
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

const int pinBotones = A0;
int valorADC = 0;

void setup() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibracion");
  lcd.setCursor(0, 1);
  lcd.print("Botones LCD");
  delay(2000);
  lcd.clear();
}

void loop() {
  valorADC = analogRead(pinBotones);

  lcd.setCursor(0, 0);
  lcd.print("Valor ADC:");
  lcd.setCursor(0, 1);
  lcd.print(valorADC);
  lcd.print("     ");

  delay(200);
}