#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
Servo myservo;

const int rele = 50;
const int pingPin = 43;

int opcion = 0;      // 0 = 250cc, 1 = 500cc
int estado = 0;      // 0:Menu, 1:Pregunta, 2:Confirmar, 3:Llenando, 4:Listo, 6:Limpieza
int seleccion = 0;   // 0 = SI, 1 = NO

unsigned long tiempoReferencia = 0; 
unsigned long duracionLlenado = 0;
bool botonPresionado = false;

// VARIABLES PARA EL MENÚ OCULTO
unsigned long tiempoInicioAbajo = 0; // Para contar los 5 segundos
const unsigned long TIEMPO_ACTIVACION = 5000; // 5 segundos

void setup() {
  lcd.begin(16, 2);
  pinMode(rele, OUTPUT);
  digitalWrite(rele, HIGH); 
  myservo.attach(45);
  myservo.write(0);
  lcd.print("Dispenser V3.0");
  delay(1500);
  lcd.clear();
}

bool botonSelect(int valor) {
  if (valor > 700 && valor < 800 && !botonPresionado) {
    botonPresionado = true;
    return true;
  }
  if (valor > 800) botonPresionado = false;
  return false;
}

void loop() {
  int boton = analogRead(A0);
  unsigned long currentMillis = millis();
  
  // --- Lectura Sensor Ultrasónico ---
  long duration, cm;
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);
  cm = duration / 29 / 2;

  // SEGURIDAD ESTRICTA (NO SE TOCA): 
  // Si no hay vaso, se apaga todo y se sale del loop (return).
  if (cm > 5) {
    digitalWrite(rele, HIGH);
    myservo.write(0); 
    if (estado != 0) {
      estado = 0;
      lcd.clear();
    }
    lcd.setCursor(0, 0);
    lcd.print("PONGA UN VASO   ");
    lcd.setCursor(0, 1);
    lcd.print("Dist: "); lcd.print(cm); lcd.print("cm      ");
    delay(120);
    
    // Reseteamos el contador del botón abajo por seguridad si quitan el vaso
    tiempoInicioAbajo = 0; 
    return; // Se detiene el código aquí si no hay vaso
  }

  // --- LÓGICA DE ESTADOS ---
  switch (estado) {
    case 0: // ===== MENU PRINCIPAL =====
      lcd.setCursor(0, 0);
      lcd.print("Seleccione:     ");
      lcd.setCursor(0, 1);
      if (opcion == 0) lcd.print(">250cc   500cc  ");
      else             lcd.print(" 250cc  >500cc  ");

      // --- LÓGICA OCULTA: BOTÓN ABAJO (5 SEGUNDOS) ---
      // El rango del botón ABAJO suele ser 200-400 en el shield estándar
      if (boton > 200 && boton < 400) {
        if (tiempoInicioAbajo == 0) {
          tiempoInicioAbajo = currentMillis; // Empezamos a cronometrar
        }
        
        // Si se mantiene presionado más de 5 segundos...
        if (currentMillis - tiempoInicioAbajo > TIEMPO_ACTIVACION) {
          estado = 6; // Entramos a limpieza
          lcd.clear();
          tiempoInicioAbajo = 0; // Reset
        }
      } 
      else {
        // Si suelta el botón o presiona otro, el contador se reinicia
        tiempoInicioAbajo = 0;
        
        // --- Navegación Normal del Menú ---
        if (boton < 60) opcion = 1; // Derecha
        else if (boton > 400 && boton < 600) opcion = 0; // Izquierda
        else if (botonSelect(boton)) { // Select
          estado = 1;
          tiempoReferencia = currentMillis;
          lcd.clear();
        }
      }
      break;

    case 1: // ===== PREGUNTA =====
      lcd.setCursor(0, 0);
      lcd.print("Desea llenar    ");
      lcd.setCursor(0, 1);
      lcd.print(opcion == 0 ? "250cc?          " : "500cc?          ");
      
      if (currentMillis - tiempoReferencia >= 1000) {
        estado = 2;
        lcd.clear();
      }
      break;

    case 2: // ===== CONFIRMAR =====
      lcd.setCursor(0, 0);
      lcd.print("Confirmar?      ");
      lcd.setCursor(0, 1);
      if (seleccion == 0) lcd.print(">SI     NO      ");
      else                lcd.print(" SI    >NO      ");

      if (boton < 60) seleccion = 1;
      else if (boton > 400 && boton < 600) seleccion = 0;
      else if (botonSelect(boton)) {
        if (seleccion == 0) {
          estado = 3;
          myservo.write(90);
          duracionLlenado = (opcion == 0) ? 8000 : 16000;
          tiempoReferencia = currentMillis;
          digitalWrite(rele, LOW);
          lcd.clear();
        } else {
          estado = 0;
          lcd.clear();
        }
      }
      break;

    case 3: // ===== LLENANDO =====
      lcd.setCursor(0, 0);
      lcd.print("Llenando...     ");
      lcd.setCursor(0, 1);
      lcd.print((duracionLlenado - (currentMillis - tiempoReferencia)) / 1000);
      lcd.print(" seg restantes ");

      if (currentMillis - tiempoReferencia >= duracionLlenado) {
        digitalWrite(rele, HIGH);
        myservo.write(0);
        estado = 4;
        tiempoReferencia = currentMillis;
        lcd.clear();
      }
      break;

    case 4: // ===== LISTO =====
      lcd.setCursor(0, 0);
      lcd.print("    LISTO!!     ");
      if (currentMillis - tiempoReferencia >= 1500) {
        estado = 0;
        lcd.clear();
      }
      break;

    case 6: // ===== MODO LIMPIEZA OCULTO =====
      // OJO: Como mantuvimos la seguridad arriba, ESTE MODO 
      // SOLO FUNCIONA SI HAY UN OBJETO FRENTE AL SENSOR.
      
      lcd.setCursor(0, 0);
      lcd.print("MODO LIMPIEZA   ");
      lcd.setCursor(0, 1);
      lcd.print("ABA:Limp ARR:Sal");

      if (boton > 200 && boton < 400) { // Botón ABAJO presionado
        digitalWrite(rele, LOW); // Activa Bomba
        myservo.write(90);       // Abre Servo
        lcd.setCursor(0, 1);
        lcd.print(">> SURTIENDO << ");
      } 
      else if (boton > 60 && boton < 200) { // Botón ARRIBA presionado
        // Salir del modo limpieza
        digitalWrite(rele, HIGH);
        myservo.write(0);
        estado = 0;
        lcd.clear();
        lcd.print("Saliendo...");
        delay(1000);
        lcd.clear();
      } 
      else {
        // Si no aprieta nada
        digitalWrite(rele, HIGH);
        myservo.write(0);
        lcd.setCursor(0, 1);
        lcd.print("ABA:Limp ARR:Sal");
      }
      break;
  }
}