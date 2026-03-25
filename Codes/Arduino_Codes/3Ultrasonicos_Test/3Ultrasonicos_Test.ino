#define trigPin_Gen 13    // Pin General para enviar la señal a los 3 ultrasónicos

#define echoPin_L 12      // Ultrasonico Izquierdo
#define echoPin_F 27      // Ultrasonico Frontal
#define echoPin_R 25      // Ultrasonico Derecho

// Variables
long duration_L;
long duration_F;
long duration_R;

int dist_L;
int dist_F;
int dist_R;

//--------------------setup----------------
void setup() {
  pinMode(trigPin_Gen, OUTPUT);   // Se define el pin trigger General como SALIDA
  pinMode(echoPin_L, INPUT);    // Se define el pin echo Izquierdo como entrada
  pinMode(echoPin_F, INPUT);    // Se define el pin echo Frontal como entrada
  pinMode(echoPin_R, INPUT);    // Se define el pin echo Derecho como entrada
  
  Serial.begin(115200);       // Se inicializa la comunicación Serial del la 
                              //   ESP32 a 115,200 baudios
}

void loop() {
  Obstacle();
  
  Serial.print("L: ");
  Serial.print(dist_L);
  Serial.print("  F: ");
  Serial.print(dist_F);
  Serial.print("  R: ");
  Serial.println(dist_R);
}

//----------funciones------------

int Obstacle(){
  digitalWrite(trigPin_Gen, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin_Gen, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_Gen, LOW);
  
  duration_L = pulseIn(echoPin_L, HIGH);
  dist_L = duration_L * 0.0343 / 2;

  duration_F = pulseIn(echoPin_F, HIGH);
  dist_F = duration_F * 0.0343 / 2;

  duration_R = pulseIn(echoPin_R, HIGH);
  dist_R = duration_R * 0.0343 / 2;

  
  
  return dist_L;
  return dist_F;
  return dist_R;
}

