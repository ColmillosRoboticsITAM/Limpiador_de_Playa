/* Move_Robot
 Programa de prueba para el movimiento de un robot diferencial utilizando ESP32 y un Cytron de 2 canales
 el cual realiza en una rutina infinita con los 4 movimientos en el robot básico y detenido */

// Pines de conexión de los motores
#define LMot_dir 16     // Dir1 Cytron
#define LMot_pwm 4      // PWM1 Cytron
#define RMot_dir 2      // Dir2 Cytron 
#define RMot_pwm 15     // PWM2 Cytron

// Configuración de PWM
const int resolucionPWM = 10;   // Resolución de 10 bits (0-1023)
const int frecuenciaPWM = 5000; // Frecuencia de 5 kHz

void setup() {
  // Configuración de pines como salida o entrada
  pinMode(LMot_dir, OUTPUT);
  pinMode(RMot_dir, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // Ejemplo de uso de la función moverRobot
  moveRobot(200, 0);  // Mover recto hacia adelante con velocidad PWM (50% de 1023)
  Serial.println("adelante");
  delay(2000);         // Esperar 2 segundos
  moveRobot(200, -1); // Girar hacia la izquierda
  Serial.println("izquierda");
  delay(2000);         // Esperar 2 segundos
  moveRobot(200, 1);  // Girar hacia la derecha
  Serial.println("derecha");
  delay(2000);         // Esperar 2 segundos
  moveRobot(-200, 0);  // Mover recto hacia atrás con velocidad PWM (50% de 1023)
  Serial.println("atrás");
  delay(2000);         // Esperar 2 segundos
  moveRobot(0, 0);    // Detenido
  Serial.println("Detenido");
  delay(2000);         // Esperar 2 segundos
}

void moveRobot(int velocidadPWM, int direccion) {
  // Condiciones para establecer la dirección del giro
  if (direccion == 0) {
    // Movimiento recto
    if (velocidadPWM > 0){
      digitalWrite(LMot_dir,LOW);
    digitalWrite(RMot_dir,LOW);
    }
    else{
      digitalWrite(LMot_dir,HIGH);
    digitalWrite(RMot_dir,HIGH);
    }
  } 
  else if (direccion == -1) {
    // Giro a la izquierda
    digitalWrite(LMot_dir,HIGH);
    digitalWrite(RMot_dir,LOW);
  } 
  else if (direccion == 1) {
    // Giro a la derecha
    digitalWrite(LMot_dir,LOW);
    digitalWrite(RMot_dir,HIGH);
  }

  // Aplicar señales PWM a los motores
  analogWrite(LMot_pwm, abs(velocidadPWM));
  analogWrite(RMot_pwm, abs(velocidadPWM));
}