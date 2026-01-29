#include <LiquidCrystal.h>
#include <Servo.h>

// Inicializacion del LCD (RS, EN, D4, D5, D6, D7)
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
Servo myservo; //Se le da nombre a nuestro servo

void setup() {
  lcd.begin(16, 2);
  myservo.attach(45);
  myservo.write(0); //Se inicializa en 0 grados
  
  lcd.setCursor(0, 0);
  lcd.print("Prueba servo");
  delay(1500);
  lcd.clear();
}

void loop() {
  int valorBotones = analogRead(A0);

  lcd.setCursor(0, 0);
  lcd.print("girar 90g der");
  lcd.setCursor(0, 1);
  lcd.print("girar 0g izq");
  if (valorBotones < 10){
    myservo.attach(90);
    }
  else if (valorBotones < 550 && valorBotones > 500){
    myservo.attach(0);
    }
  }