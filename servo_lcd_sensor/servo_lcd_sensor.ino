#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
Servo myservo;

const int pingPin = 43; 
int seleccion = 0; // 0 = Abrir, 1 = Cerrar

void setup() {
  lcd.begin(16, 2);
  myservo.attach(45);
  myservo.write(0); 
  
  lcd.setCursor(0, 0);
  lcd.print("Menu de Control");
  delay(1500);
  lcd.clear();
}

void loop() {
  int valorBotones = analogRead(A0);
  long duration, cm;

  // --- Lectura del Sensor Ping))) ---
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);
  cm = microsecacm(duration);

  if (cm <= 10) {
    // --- Lógica de Navegación del Menú ---
    if (valorBotones < 60) {         // Derecha
      seleccion = 0; // Seleccionar Abrir
    } else if (valorBotones < 600 && valorBotones > 400) { // Izquierda
      seleccion = 1; // Seleccionar Cerrar
    }

    // --- Mostrar Menú en LCD ---
    lcd.setCursor(0, 0);
    lcd.print("OBJETO DETECTADO");
    
    lcd.setCursor(0, 1);
    // Dibujamos el cursor ">" según la selección
    if (seleccion == 0) {
      lcd.print(">ABRIR  Cerrar  ");
    } else {
      lcd.print(" Abrir >CERRAR  ");
    }

    // --- Ejecutar con SELECT ---
    if (valorBotones < 800 && valorBotones > 600) { // Botón SELECT
      if (seleccion == 0) {
        myservo.write(90);
        mensajeEjecutado("Abriendo...");
      } else {
        myservo.write(0);
        mensajeEjecutado("Cerrando...");
      }
    }
  } 
  else {
    // Pantalla de espera cuando no hay nada cerca
    lcd.setCursor(0, 0);
    lcd.print("Buscando...     ");
    lcd.setCursor(0, 1);
    lcd.print("Dist: ");
    lcd.print(cm);
    lcd.print(" cm      ");
    myservo.write(0);
  }

  delay(100); 
}

// Función auxiliar para mostrar un mensaje rápido al pulsar SELECT
void mensajeEjecutado(String texto) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("EJECUTANDO:");
  lcd.setCursor(0, 1);
  lcd.print(texto);
  delay(1000); // Pausa para que el usuario lo vea
  lcd.clear();
}

long microsecacm(long microseconds) {
  return microseconds / 29 / 2;
}