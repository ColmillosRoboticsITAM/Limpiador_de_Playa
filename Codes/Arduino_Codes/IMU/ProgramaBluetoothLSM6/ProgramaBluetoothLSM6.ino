#include <Wire.h>
#include <LSM6.h>
#include "BluetoothSerial.h"  
BluetoothSerial SerialBT;  

LSM6 imu;

// Motores
#define motor_pwm1  15
#define motor_pwm2  4
#define motor_dir1  16 
#define motor_dir2  2
const int Frecuencia = 115200;
const int Resolucion = 8;

// Variables globales
float Theta_deseada = 90;
float Theta = 0;  
float correction = 0;
float anterior_error = 0;  
float integral = 0;

// PID
float Kp = 0.5;
float Ki = 0.001;
float Kd = 0.3;

// Tiempo
unsigned long tiempo_anterior = 0;

void setup() {
    Serial.begin(115200);
    SerialBT.begin("ESP32_PID");
    Serial.println("Bluetooth listo. Empareja con ESP32_PID");
    SerialBT.println("Bluetooth listo. Empareja con ESP32_PID");

    Wire.begin();

    if (!imu.init()) {
        Serial.println("No se detectó el LSM6.");
        SerialBT.println("No se detectó el LSM6.");
        while (1);
    }
    imu.enableDefault();

    // Pines motores
    pinMode(motor_pwm1, OUTPUT);
    pinMode(motor_pwm2, OUTPUT);
    pinMode(motor_dir1, OUTPUT);
    pinMode(motor_dir2, OUTPUT);

    // PWM
    ledcAttach(motor_pwm1, Frecuencia, Resolucion);
    ledcAttach(motor_pwm2, Frecuencia, Resolucion);

    tiempo_anterior = millis();
}

void loop() {
    actualizarTheta();
    calcularPID();
    Correccion(correction);

    Serial.print("Theta: "); Serial.println(Theta);
    SerialBT.print("Theta: "); SerialBT.println(Theta);
    Serial.print("Error: "); Serial.println(Theta_deseada - Theta); 
    Serial.print("Corrección: "); Serial.println(correction); 
    SerialBT.print("Error: "); SerialBT.println(Theta_deseada - Theta); 
    SerialBT.print("Corrección: "); SerialBT.println(correction); 

    delay(50);
}

void actualizarTheta() {
    imu.readGyro();
    unsigned long tiempo_actual = millis();
    float dt = (tiempo_actual - tiempo_anterior) / 1000.0;
    tiempo_anterior = tiempo_actual;
 
    float gyroZ = imu.g.z * 0.035;  // escala de °/s 

    Theta += gyroZ * dt;

    if (Theta > 360) Theta -= 360;
    if (Theta < 0) Theta += 360;
}

void calcularPID() {
    float error = Theta_deseada - Theta;
    
    if (error > 180) error -= 360;
    if (error < -180) error += 360;

    integral += error;
    float derivada = error - anterior_error;
    correction = (Kp * error) + (Ki * integral) + (Kd * derivada);
    correction = constrain(correction, -30, 30);
    anterior_error = error;
}

void Correccion(float correction){
    if (correction > 2){
        Serial.println("MOTOR DERECHO");
        SerialBT.println("MOTOR DERECHO");
        Speed2(true, 100);
    }
    else if (correction < -2){
        Serial.println("MOTOR IZQUIERDO");
        SerialBT.println("MOTOR IZQUIERDO");
        Speed2(false,100); 
    }
    else {
        Serial.println("ADELANTE");
        SerialBT.println("ADELANTE");
        Speed(true,100); 
    }
}

void Speed(bool dir, int speed) {
    digitalWrite(motor_dir1, dir);
    digitalWrite(motor_dir2, dir);
    ledcWrite(motor_pwm1, speed);
    ledcWrite(motor_pwm2, speed);
}

void Speed2(bool dir, int speed) {
    digitalWrite(motor_dir1, !dir);
    digitalWrite(motor_dir2, dir);
    ledcWrite(motor_pwm1, speed);
    ledcWrite(motor_pwm2, speed);
}
