/* Joystick_LStick
 Programa que lee el stick izquierdo del control PS4 y muestra los valores de -127 a 127 y se mapean de -200 a 200
 de manera que se obtengan las velocidades de cada una de las ruedas.*/
 
 #include <PS4Controller.h>

int velRob = 0;
int angRob = 0;

int LSpeed = 0; 
int RSpeed = 0; 

void setup() {
  Serial.begin(115200);
  PS4.begin();

  Serial.println("Ready.");
}

void loop() {
  if (PS4.isConnected()) {
     
    velRob = PS4.LStickY();         // Leyendo valor de Joystick izquierdo al moverlo arriba/abajo 
    velRob = map(velRob, -127, 127, -200, 200);     // Escalando
    angRob = PS4.LStickX();         // Leyendo valor de Joystick izquierdo al moverlo izquierda/derecha
    angRob = map(angRob, -127, 127, -200, 200);     // Escalando
        
    LSpeed = velRob - angRob;       // Calculando velocidad de la rueda izquierda
    LSpeed = constrain(LSpeed, -255, 255);    // Restringuiendo límites para rueda izquierda
    RSpeed = velRob + angRob;       // Calculando velocidad de la rueda derecha
    RSpeed = constrain(LSpeed, -255, 255);    // Restringuiendo límites para rueda derecha

    Serial.print(velRob);  // Imprimiendo Dato de Movimiento Lineal del Robot
    Serial.print(" / ");   
    Serial.print(angRob);  // Imprimiendo Dato de Movimiento Angular del Robot
    Serial.print(" /---/ ");  
    Serial.print(LSpeed);  // Imprimiendo Dato para rueda izquierda
    Serial.print(" / ");  
    Serial.println(RSpeed);  //Print the angle to the Serial Monitor
   
    delay(10);
  }
}