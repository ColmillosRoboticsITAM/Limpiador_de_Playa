// Librerías necesarias para utilizar la IMU BNO055
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

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
float Kp = 0.5;  // Ajusta según sea necesario
float Ki = 0.001; // Ajusta para eliminar error constante
float Kd = 0.3;  // Ajusta para respuesta rápida

void setup() {
    // Inicia la comunicación Serial 
    Serial.begin(115200);

    // Conexión de la IMU 
    if (!bno.begin()) {
        Serial.println("Error al detectar el BNO055");
        while (1);
    }
    bno.setExtCrystalUse(true);
    Serial.println("BNO055 listo.");

    // Configuración de Pines 
    pinMode(motor_pwm1, OUTPUT);
    pinMode(motor_pwm2, OUTPUT);
    pinMode(motor_dir1, OUTPUT);
    pinMode(motor_dir2, OUTPUT);

    // Configuración de PWM para los motores 
    ledcAttach(motor_pwm1, Frecuencia, Resolucion);
    ledcAttach(motor_pwm2, Frecuencia, Resolucion);
}

void loop() {
    Fun_IMU();
    Error();
    Correccion(correction);
    
    Serial.print("Error: "); Serial.println(Theta_deseada - Theta); 
    Serial.print("Corrección: "); Serial.println(correction); 
    delay(100);
}

// Función de la IMU 
void Fun_IMU(){
    // Estructura que almacena los datos del sensor
    sensors_event_t event;
    bno.getEvent(&event);

    // Obtener la Theta (Z)
    Theta = event.orientation.z;

    // Imprimir Valores
    Serial.print("Theta: "); Serial.println(Theta);
}

// Función para calcular el error y aplicar PID
void Error(){
    // Error en General 
    float error = Theta_deseada - Theta;

    // Cálculo de PID
    integral += error;  // Acumula el error
    float derivada = error - anterior_error;  // Diferencia con el error anterior
    correction = (Kp * error) + (Ki * integral) + (Kd * derivada);

    // Limitar la corrección para evitar cambios bruscos
    correction = constrain(correction, -30, 30);

    // Guardar el error anterior
    anterior_error = error;
}

// Control del movimiento
void Correccion(float correction){
    if (correction > 2){
        Serial.println("MOTOR DERECHO");
        Speed2(true, 100);
    }
    else if (correction < -2){
        Serial.println("MOTOR IZQUIERDO");
        Speed2(false,100); 
    }
    else {
        Serial.println("ADELANTE");
        Speed(true,100); 
    }
}

void Speed(bool dir, int speed) {
    if (dir) {
        digitalWrite(motor_dir1, HIGH);
        ledcWrite(motor_pwm1, speed);
        digitalWrite(motor_dir2, HIGH);
        ledcWrite(motor_pwm2, speed);
    } else {
        digitalWrite(motor_dir1, LOW);
        ledcWrite(motor_pwm1, speed);
        digitalWrite(motor_dir2, LOW);
        ledcWrite(motor_pwm2, speed);
    }
}

void Speed2(bool dir, int speed) {
    if (dir) {
        digitalWrite(motor_dir1, LOW);
        ledcWrite(motor_pwm1, speed);
        digitalWrite(motor_dir2, HIGH);
        ledcWrite(motor_pwm2, speed);
    } else {
        digitalWrite(motor_dir1, HIGH);
        ledcWrite(motor_pwm1, speed);
        digitalWrite(motor_dir2, LOW);
        ledcWrite(motor_pwm2, speed);
    }
}
