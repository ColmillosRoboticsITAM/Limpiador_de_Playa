#include <ESP32Servo.h>

const int pinServo1 = 2;  // Pin del primer servomotor
const int pinServo2 = 4;  // Pin del segundo servomotor (ajústalo según el pin que estés usando)
Servo servoMotor1;
Servo servoMotor2;

void setup() {
  Serial.begin(115200);
  servoMotor1.attach(pinServo1);  // Conectar el primer servomotor
  servoMotor2.attach(pinServo2);  // Conectar el segundo servomotor
  Serial.println("Angulo de 0 a 80");
}

void loop() {
  if (Serial.available()) {
    int angulo = Serial.parseInt();
    
    if (angulo >= 0 && angulo <= 80) {
      // Establecer el ángulo para el primer servomotor
      servoMotor1.write(angulo);
      
      // Calcular el ángulo para el segundo servomotor (180 - ángulo)
      int anguloServo2 = 80 - angulo;
      servoMotor2.write(anguloServo2);
      
      Serial.print("Angulo del primer servo establecido a: ");
      Serial.println(angulo);
      Serial.print("Angulo del segundo servo establecido a: ");
      Serial.println(anguloServo2);
      
    } else {
      Serial.println("Error: Ingrese un angulo entre 0 y 80");
    }
  }
}

