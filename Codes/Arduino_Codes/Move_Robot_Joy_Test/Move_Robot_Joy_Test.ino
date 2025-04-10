/* Move_Robot controlado por joystick
 Programa de prueba para el movimiento de un robot diferencial utilizando ESP32 y un Cytron de 2 canales
 el cual moviendo sólo el stick izquierdo mueve al robot*/

 #include <PS4Controller.h>
 
 // Pines de conexión de los motores de tracción
#define LMot_dir 16     // DIR1 Cytron 2 Channel
#define LMot_pwm 4      // PWM1 Cytron 2 Channel
#define RMot_dir 2      // DIR2 Cytron 2 Channel
#define RMot_pwm 15     // PWM2 Cytron 2 Channel

// Pines de conexión de los motores de la barredora
#define Sweeper_dir 5      // DIR Cytron 1 Channel
#define Sweeper_pwm 17     // PWM Cytron 1 Channel

 // Declaración de variables
 int velRob = 0;
 int angRob = 0;

 int velSweeper = 0;


// ---------------------------- Setup -----------------------------------------
void setup() {
  // Configuración de pines como salida o entrada
  pinMode(LMot_dir, OUTPUT);
  pinMode(RMot_dir, OUTPUT);
  pinMode(Sweeper_dir, OUTPUT);
  
  // Inicializando PS4 y Serial
  PS4.begin();
  Serial.begin(115200);
}


// ---------------------------- Loop -----------------------------------------
void loop() {
  // Ejemplo de uso de la función moveRobot
  if (PS4.isConnected()) {
     
    velRob = PS4.LStickY();         // Leyendo valor de Joystick izquierdo al moverlo arriba/abajo 
    velRob = map(velRob, -127, 127, -200, 200);     // Escalando
    angRob = PS4.LStickX();         // Leyendo valor de Joystick izquierdo al moverlo izquierda/derecha
    angRob = map(angRob, -127, 127, -200, 200);     // Escalando
    
    velSweeper = PS4.RStickY();         // Leyendo valor de Joystick derecho al moverlo arriba/abajo 
    velSweeper = map(velSweeper, -127, 127, -255, 255);     // Escalando
    
    moveRobot(velRob, angRob);

    moveSweeper(velSweeper);

  delay(10);         // Esperar 2 segundos
  }
  else{
    // Los motores se quedan detenidos
    analogWrite(LMot_pwm, 0);
    analogWrite(RMot_pwm, 0);

    analogWrite(Sweeper_pwm, 0);
  }

  
}


// ---------------------------- Functions -----------------------------------------
void moveRobot(int velocidadPWM, int direccion) {
  // Variables para calcular las señales PWM de cada motor
  int pwmIzquierdo = 0;
  int pwmDerecho = 0;

  pwmIzquierdo = velocidadPWM + direccion;       // Calculando velocidad de la rueda izquierda
  pwmIzquierdo = constrain(pwmIzquierdo, -255, 255);    // Restringuiendo límites para rueda izquierda
  pwmDerecho = velocidadPWM - direccion;       // Calculando velocidad de la rueda derecha
  pwmDerecho = constrain(pwmDerecho, -255, 255);    // Restringuiendo límites para rueda derecha

  // Condiciones para establecer la dirección del giro
  if (pwmIzquierdo > 10 && pwmDerecho < -10) {
    // Giro a la izquierda
    digitalWrite(LMot_dir,HIGH);
    digitalWrite(RMot_dir,LOW);
  } 
  else if (pwmIzquierdo < -10 && pwmDerecho > 10) {
    // Giro a la derecha
    digitalWrite(LMot_dir,LOW);
    digitalWrite(RMot_dir,HIGH);
  }
  else if (pwmIzquierdo > 10 && pwmDerecho > 10){
    // Movimiento recto y hacia adelante
    digitalWrite(LMot_dir,LOW);
    digitalWrite(RMot_dir,LOW);
  }
  else if (pwmIzquierdo < -10 && pwmDerecho < -10){
    // Movimiento recto y hacia atrás
    digitalWrite(LMot_dir,HIGH);
    digitalWrite(RMot_dir,HIGH);
  }
    
  // Aplicar señales PWM a los motores
  analogWrite(LMot_pwm, abs(pwmIzquierdo));
  analogWrite(RMot_pwm, abs(pwmDerecho));

  /*Serial.print(velocidadPWM);  // Imprimiendo Dato de Movimiento Lineal del Robot
  Serial.print(" / ");   
  Serial.print(direccion);  // Imprimiendo Dato de Movimiento Angular del Robot
  Serial.print(" /---/ ");
  Serial.print(pwmIzquierdo);  // Imprimiendo Dato para rueda izquierda
  Serial.print(" / ");  
  Serial.println(pwmDerecho);  // Imprimiendo Dato para rueda izquierda*/
}

void moveSweeper(int velocidadPWM) {
  // Condiciones para establecer la dirección del giro
  if (velocidadPWM > 0) {
    // Mete latas
    digitalWrite(Sweeper_dir,HIGH);
  } 
  else {
    // Saca latas
    digitalWrite(Sweeper_dir,LOW);
  } 
  velocidadPWM = constrain(velocidadPWM, -255, 255);    // Restringuiendo límites para motor
  analogWrite(Sweeper_pwm, abs(velocidadPWM));          // Aplicar señal PWM al motor

  Serial.println(velocidadPWM);  // Muestra el valor que se manda al motor de la barredora*/
}