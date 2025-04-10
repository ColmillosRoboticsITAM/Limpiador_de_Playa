/* Move_Robot
 Programa de prueba para el movimiento de un robot diferencial utilizando ESP32 y un Cytron de 2 canales
 el cual realiza en una rutina infinita con los 4 movimientos en el robot básico y detenido */

// Pines de conexión de los motores
#define LMot_dir 16     // Dir1 Cytron
#define LMot_pwm 4      // PWM1 Cytron
#define RMot_dir 2      // Dir2 Cytron 
#define RMot_pwm 15     // PWM2 Cytron


void setup() {
  // Configuración de pines como salida o entrada
  pinMode(LMot_dir, OUTPUT);
  pinMode(RMot_dir, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // Ejemplo de uso de la función moverRobot
  moveRobot(200, 0);  // Mover recto hacia adelante con velocidad PWM (50% de 1023)
  Serial.println("Forward");
  delay(2000);         // Esperar 2 segundos
  moveRobot(200, -200); // Girar hacia la izquierda
  Serial.println("Left");
  delay(2000);         // Esperar 2 segundos
  moveRobot(200, 300);  // Girar hacia la derecha
  Serial.println("Right");
  delay(2000);         // Esperar 2 segundos
  moveRobot(-200, 0);  // Mover recto hacia atrás con velocidad PWM (50% de 1023)
  Serial.println("Back");
  delay(2000);         // Esperar 2 segundos
  moveRobot(0, 0);    // Detenido
  Serial.println("Stop");
  delay(2000);         // Esperar 2 segundos
}

void moveRobot(int velocidadPWM, int direccion) {
  // Variables para calcular las señales PWM de cada motor
  int pwmIzquierdo = 0;
  int pwmDerecho = 0;

  pwmIzquierdo = constrain(velocidadPWM, 0, 255);
  pwmDerecho = constrain(velocidadPWM, 0, 255);

  // Condiciones para establecer la dirección del giro
  if (direccion < 0) {
    // Giro a la izquierda
    digitalWrite(LMot_dir,HIGH);
    digitalWrite(RMot_dir,LOW);
  } 
  else if (direccion > 0) {
    // Giro a la derecha
    digitalWrite(LMot_dir,LOW);
    digitalWrite(RMot_dir,HIGH);
  }
  else {   // direccion == 0
    // Movimiento recto
    if (velocidadPWM > 0){
    digitalWrite(LMot_dir,LOW);
    digitalWrite(RMot_dir,LOW);
    }
    else {
    digitalWrite(LMot_dir,HIGH);
    digitalWrite(RMot_dir,HIGH);
    }
  } 
  

  // Aplicar señales PWM a los motores
  analogWrite(LMot_pwm, abs(pwmIzquierdo));
  analogWrite(RMot_pwm, abs(pwmDerecho));
}