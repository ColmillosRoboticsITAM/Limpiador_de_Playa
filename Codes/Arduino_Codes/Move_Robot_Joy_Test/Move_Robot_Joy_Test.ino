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
#define Intake_dir 5      // DIR Cytron 1 Channel
#define Intake_pwm 17     // PWM Cytron 1 Channel

// Pines de señal para los servomotores

 // Declaración de variables
 int velRob = 0;
 int angRob = 0;

 int velIntake = 0;

// ---------------------------- Setup -----------------------------------------
void setup() {
  // Configuración de pines como salida o entrada
  pinMode(LMot_dir, OUTPUT);
  pinMode(RMot_dir, OUTPUT);
  pinMode(Intake_dir, OUTPUT);
  
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
    
    velIntake = PS4.RStickY();         // Leyendo valor de Joystick derecho al moverlo arriba/abajo 
    velIntake = map(velIntake, -127, 127, -255, 255);     // Escalando
    
    moveRobot(velRob, angRob);

    moveIntake(velIntake);

  delay(10);         // Esperar 2 segundos
  }
  else{
    // Los motores se quedan detenidos
    analogWrite(LMot_pwm, 0);
    analogWrite(RMot_pwm, 0);

    analogWrite(Intake_pwm, 0);
  }

  
}


// ---------------------------- Functions -----------------------------------------
void moveRobot(int velocidadPWM, int giroPWM) {
  // Variables para calcular las señales PWM de cada motor
  int pwmIzquierdo = 0;
  int pwmDerecho = 0;

  pwmIzquierdo = velocidadPWM + giroPWM;       // Calculando velocidad de la rueda izquierda
  pwmIzquierdo = constrain(pwmIzquierdo, -255, 255);    // Restringuiendo límites para rueda izquierda
  pwmDerecho = velocidadPWM - giroPWM;       // Calculando velocidad de la rueda derecha
  pwmDerecho = constrain(pwmDerecho, -255, 255);    // Restringuiendo límites para rueda derecha

  // Condiciones para establecer la dirección del giro
  if (pwmIzquierdo < -20 && pwmDerecho > 20) {
    // Giro a la izquierda
    digitalWrite(LMot_dir,HIGH);
    digitalWrite(RMot_dir,LOW);
  } 
  else if (pwmIzquierdo > 20 && pwmDerecho < -20) {
    // Giro a la derecha
    digitalWrite(LMot_dir,LOW);
    digitalWrite(RMot_dir,HIGH);
  }
  else if (pwmIzquierdo > 20 && pwmDerecho > 20){
    // Movimiento recto y hacia adelante
    digitalWrite(LMot_dir,LOW);
    digitalWrite(RMot_dir,LOW);
  }
  else if (pwmIzquierdo < -20 && pwmDerecho < -20){
    // Movimiento recto y hacia atrás
    digitalWrite(LMot_dir,HIGH);
    digitalWrite(RMot_dir,HIGH);
  }

  if(pwmIzquierdo > -20 && pwmIzquierdo < 20){
    // Aplicar señales PWM a los motores
    analogWrite(LMot_pwm, 0);
    
    Serial.println("detenidoLEFT");
  }
  else{
    // Aplicar señales PWM a los motores
    analogWrite(LMot_pwm, abs(pwmIzquierdo));
  }

  if(pwmDerecho > -20 && pwmDerecho < 20){
    analogWrite(RMot_pwm, 0);
    Serial.println("detenidoRIGHT");
  }
  else{
    // Aplicar señales PWM a los motores
    analogWrite(RMot_pwm, abs(pwmDerecho));
  }

  Serial.print(velRob);  // Imprimiendo Dato de Movimiento Lineal del Robot
  Serial.print(" / ");   
  Serial.print(angRob);  // Imprimiendo Dato de Movimiento Angular del Robot
  Serial.print("  / --- /  ");
  Serial.print(pwmIzquierdo);  // Imprimiendo Dato para rueda izquierda
  Serial.print(" / ");  
  Serial.println(pwmDerecho);  // Imprimiendo Dato para rueda izquierda
}

void moveIntake(int velocidadPWM) {
  // Condiciones para establecer la dirección del giro
  if (velocidadPWM > 0) {
    // Mete latas
    digitalWrite(Intake_dir,HIGH);
  } 
  else {
    // Saca latas
    digitalWrite(Intake_dir,LOW);
  } 
  velocidadPWM = constrain(velocidadPWM, -255, 255);    // Restringuiendo límites para motor
  analogWrite(Intake_pwm, abs(velocidadPWM));          // Aplicar señal PWM al motor

  Serial.println(velocidadPWM);  // Muestra el valor que se manda al motor de la barredora*/
}