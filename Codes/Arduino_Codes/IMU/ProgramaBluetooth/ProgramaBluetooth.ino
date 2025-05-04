// Programa con Bluetooth
#include "BluetoothSerial.h" 

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

BluetoothSerial SerialBT;  

// Dirección I2C
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

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

// Parámetros PID
float Kp = 0.5;
float Ki = 0.001;
float Kd = 0.3;

void setup() {
    // Comunicación Serial y Bluetooth
    Serial.begin(115200);
    SerialBT.begin("ESP32"); // Nombre del Bluetooth
    Serial.println("Bluetooth listo. Empareja con ESP32");

    // Conexión de la IMU 
    if (!bno.begin()) {
        Serial.println("Error al detectar el BNO055");
        SerialBT.println("Error al detectar el BNO055");
        while (1);
    }
    bno.setExtCrystalUse(true);
    Serial.println("BNO055 listo.");
    SerialBT.println("BNO055 listo.");

    // Configuración de Pines 
    pinMode(motor_pwm1, OUTPUT);
    pinMode(motor_pwm2, OUTPUT);
    pinMode(motor_dir1, OUTPUT);
    pinMode(motor_dir2, OUTPUT);

    // PWM para motores
    ledcAttach(motor_pwm1, Frecuencia, Resolucion);
    ledcAttach(motor_pwm2, Frecuencia, Resolucion);
}

void loop() {
    Fun_IMU();
    Error();
    Correccion(correction);
    
    Serial.print("Error: "); Serial.println(Theta_deseada - Theta); 
    Serial.print("Corrección: "); Serial.println(correction); 
    
    SerialBT.print("Error: "); SerialBT.println(Theta_deseada - Theta); 
    SerialBT.print("Corrección: "); SerialBT.println(correction); 
    
    delay(100);
}

void Fun_IMU(){
    sensors_event_t event;
    bno.getEvent(&event);
    Theta = event.orientation.x;

    Serial.print("Theta: "); Serial.println(Theta);
    SerialBT.print("Theta: "); SerialBT.println(Theta);
}

void Error(){
    float error = Theta_deseada - Theta;
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
