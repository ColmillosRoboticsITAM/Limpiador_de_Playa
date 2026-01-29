#include <PS4Controller.h>
#include <ESP32Servo.h>

// Pines de motores de tracción
#define LMot_dir 2
#define LMot_pwm 17
#define RMot_dir 16
#define RMot_pwm 4

// Pines del motor de barredora
#define Intake_dir 5
#define Intake_pwm 18

// Pines de servomotores del contenedor
#define Servo1_pin 14  
//#define Servo2_pin 19

Servo servo1;
Servo servo2;

// Variables globales
int velRob = 0;
int angRob = 0;
bool intakeActivo = false;
bool containerAbierto = false;
int anguloServo = 60;

void setup() {
  pinMode(LMot_dir, OUTPUT);
  pinMode(RMot_dir, OUTPUT);
  pinMode(Intake_dir, OUTPUT);

  PS4.begin();
  Serial.begin(115200);

  // Servos
  servo1.attach(Servo1_pin);
  //servo2.attach(Servo2_pin);

  // Posición inicial del contenedor
  moveContainer(false);
}

void loop() {
  if (PS4.isConnected()) {

    // Movimiento del robot
    velRob = map(PS4.LStickY(), -127, 127, -200, 200);
    angRob = map(PS4.LStickX(), -127, 127, -200, 200);
    moveRobot(velRob, angRob);

    // Toggle del intake con botón Círculo
    static bool circuloPrev = false;
    if (PS4.Circle() && !circuloPrev) {
      intakeActivo = !intakeActivo;
    }
    circuloPrev = PS4.Circle();
    moveIntake(intakeActivo);

    // Toggle del contenedor con botón Cuadrado
    static bool cuadradoPrev = false;
    if (PS4.Square() && !cuadradoPrev) {
      containerAbierto = !containerAbierto;
      moveContainer(containerAbierto);
    }
    cuadradoPrev = PS4.Square();

    delay(10);

    // Parada de emergencia con OPTIONS
    if (PS4.Options()) {
      analogWrite(LMot_pwm, 0);
      analogWrite(RMot_pwm, 0);
      analogWrite(Intake_pwm, 0);
    }

  } else {
    analogWrite(LMot_pwm, 0);
    analogWrite(RMot_pwm, 0);
    analogWrite(Intake_pwm, 0);
  }
}

// Movimiento del robot
void moveRobot(int velocidadPWM, int giroPWM) {
  int pwmIzquierdo = constrain(velocidadPWM + giroPWM, -255, 255);
  int pwmDerecho = constrain(velocidadPWM - giroPWM, -255, 255);

  // Dirección de motores
  if (pwmIzquierdo < -20 && pwmDerecho > 20) {
    digitalWrite(LMot_dir, HIGH);
    digitalWrite(RMot_dir, LOW);
  } else if (pwmIzquierdo > 20 && pwmDerecho < -20) {
    digitalWrite(LMot_dir, LOW);
    digitalWrite(RMot_dir, HIGH);
  } else if (pwmIzquierdo > 20 && pwmDerecho > 20) {
    digitalWrite(LMot_dir, LOW);
    digitalWrite(RMot_dir, LOW);
  } else if (pwmIzquierdo < -20 && pwmDerecho < -20) {
    digitalWrite(LMot_dir, HIGH);
    digitalWrite(RMot_dir, HIGH);
  }

  analogWrite(LMot_pwm, abs(pwmIzquierdo));
  analogWrite(RMot_pwm, abs(pwmDerecho));

  Serial.print(velRob); Serial.print(" / ");
  Serial.print(angRob); Serial.print(" / --- / ");
  Serial.print(pwmIzquierdo); Serial.print(" / ");
  Serial.println(pwmDerecho);
}

// Movimiento del intake
void moveIntake(bool activo) {
  if (activo) {
    digitalWrite(Intake_dir, HIGH);
    analogWrite(Intake_pwm, 255);
    Serial.println("Intake ACTIVADO");
  } else {
    analogWrite(Intake_pwm, 0);
    Serial.println("Intake APAGADO");
  }
}

// Movimiento del contenedor
void moveContainer(bool abierto) {
  if (abierto) {
    servo1.write(anguloServo);               // Ángulo de apertura
    //servo2.write(180 - anguloServo);               // Complemento
    Serial.println("Contenedor ABIERTO");
  } else {
    servo1.write(180 - anguloServo);               // Ángulo de cierre
    //servo2.write(anguloServo);               // Complemento
    Serial.println("Contenedor CERRADO");
  }
}

