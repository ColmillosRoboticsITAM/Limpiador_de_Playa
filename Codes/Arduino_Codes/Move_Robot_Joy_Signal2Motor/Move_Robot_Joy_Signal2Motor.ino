/* Move_Robot controlado por joystick
 Programa de prueba para el movimiento de un robot diferencial utilizando ESP32 y un Cytron de 2 canales
 el cual realiza en una rutina infinita con los 4 movimientos en el robot básico y detenido */

 #include <PS4Controller.h>
 
 // Pines de conexión de los motores
#define LMot_dir 16     // Dir1 Cytron
#define LMot_pwm 4      // PWM1 Cytron
#define RMot_dir 2      // Dir2 Cytron 
#define RMot_pwm 15     // PWM2 Cytron

 // Declaración de variables
 int LWheel = 0;
 int RWheel = 0;

void setup() {
  // Configuración de pines como salida o entrada
  pinMode(LMot_pwm, OUTPUT);
  pinMode(RMot_pwm, OUTPUT);

  // Se envío un alto por el pin de PWm para trabajar en modo Anti-Phase PWM
  digitalWrite(LMot_pwm,HIGH);    
  digitalWrite(RMot_pwm,HIGH);
  
  
  // Inicializando PS4 y Serial
  PS4.begin();
  Serial.begin(115200);
}

void loop() {
  // Ejemplo de uso de la función moverRobot
  if (PS4.isConnected()) {
     
    LWheel = PS4.LStickY();         // Leyendo valor de Joystick izquierdo al moverlo arriba/abajo 
    LWheel = map(LWheel, -127, 127, 255, 0);     // Escalando
    RWheel = PS4.RStickY();         // Leyendo valor de Joystick izquierdo al moverlo izquierda/derecha
    RWheel = map(RWheel, -127, 127, 255, 0);     // Escalando
    
    moveRobot(LWheel, RWheel);

  delay(10);         // Esperar 2 segundos
  }
}

void moveRobot(int pwmIzquierdo, int pwmDerecho) {
  pwmIzquierdo = constrain(pwmIzquierdo, 0, 255);    // Restringuiendo límites para rueda izquierda
  pwmDerecho = constrain(pwmDerecho, 0, 255);    // Restringuiendo límites para rueda derecha

  // Aplicar señales PWM a los motores
  analogWrite(LMot_dir, pwmIzquierdo);
  analogWrite(RMot_dir, pwmDerecho);

  Serial.print(LWheel);  // Imprimiendo Dato de Movimiento Lineal del Robot
  Serial.print(" / ");   
  Serial.print(RWheel);  // Imprimiendo Dato de Movimiento Angular del Robot
  Serial.print(" /---/ ");
  Serial.print(pwmIzquierdo);  // Imprimiendo Dato para rueda izquierda
  Serial.print(" / ");  
  Serial.println(pwmDerecho);  //Print the angle to the Serial Monitor
}