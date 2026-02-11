/* =========================================================
   FSM Mantis -- Ultrasonicos
   ========================================================= */

// -------------------- ULTRASONICOS --------------------
#define USL_TRIG 16
#define USL_ECHO 4
#define USC_TRIG 5
#define USC_ECHO 12
#define USD_TRIG 21
#define USD_ECHO 19

// -------------------- LEDs --------------------
#define LED_A 2
#define LED_B 15
#define LED_C 13

// -------------------- ENCODERS ----------------
#define L_EncA 35
#define L_EncB 34
#define R_EncA 39
#define R_EncB 36

// -------------------- MOTORES -----------------
#define LMot_dir 32
#define LMot_pwm 33
#define RMot_dir 25
#define RMot_pwm 26

// -------------------- FSM ---------------------
enum EstadoRobot {
  INICIO,
  BUSCAR_LATA,
  GIRAR_A_LATA,
  IR_A_LATA,
  RECOGER_LATA,
  BUSCAR_CONT,
  GIRAR_A_CONT,
  IR_A_CONT,
  ENTREGAR,
  EVITAR_MAR,
  EVITAR_OBSTACULO,
};

EstadoRobot estadoActual = INICIO;
EstadoRobot estadoAnterior = INICIO;

// -------------------- VISION --------------------
float ang_lata = 540;
bool flag_lata = false;
bool flag_mar = false;
bool hayCont = false;
float ang_cont = 540;
bool flag_cont = false;

// -------------------- PID --------------------
float kp = 1.3;
float ki = 0.0;
float kd = 0.15;
float errPrev = 0;
float integral = 0;

// -------------------- TIMERS --------------------
unsigned long evitarStartTime = 0;
bool evitarActivo = false;

unsigned long recogerStartTime = 0;
bool timerRecoger = false;

unsigned long entregarStartTime = 0;
bool timerEntregar = false;

// -------------------- OBS --------------------
unsigned long obstStartTime = 0;
bool obstActivo = false;
int obstDir = 0;


// =================================================
void IRAM_ATTR isr_LA() {}
void IRAM_ATTR isr_RA() {}

// =================================================
void leerSerial() {
  if (!Serial.available()) return;

  ang_lata  = Serial.parseFloat();
  flag_lata = Serial.parseInt();
  flag_mar  = Serial.parseInt();
  hayCont   = Serial.parseInt();
  ang_cont  = Serial.parseFloat();
  flag_cont = Serial.parseInt();
}

// =================================================
float PID_Giro(float ang) {
  float err = ang;
  integral += err;
  float deriv = err - errPrev;
  errPrev = err;
  return kp * err + ki * integral + kd * deriv;
}

// =================================================
void moveRobot(float v, float w) {
  int L = constrain((v - w) * 255, -255, 255);
  int R = constrain((v + w) * 255, -255, 255);

  digitalWrite(LMot_dir, L >= 0 ? LOW : HIGH);
  digitalWrite(RMot_dir, R >= 0 ? LOW : HIGH);

  analogWrite(LMot_pwm, abs(L));
  analogWrite(RMot_pwm, abs(R));
}

// =================================================
void ledBlink(int pin, int t) {
  static unsigned long last[3];
  static bool state[3];

  int i = (pin == LED_A) ? 0 : (pin == LED_B) ? 1 : 2;

  if (millis() - last[i] > t) {
    last[i] = millis();
    state[i] = !state[i];
    digitalWrite(pin, state[i]);
  }
}

// =================================================
long leerDistancia(int trig, int echo) {

  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long d = pulseIn(echo, HIGH, 25000);
  if (d == 0) return 999;
  return d / 58;
}

// =================================================
void ejecutarEvitar() {

  ledBlink(LED_C, 80);

  if (!evitarActivo) {
    evitarActivo = true;
    evitarStartTime = millis();
  }

  unsigned long t = millis() - evitarStartTime;

  if (t < 350) moveRobot(-0.3, 0);
  else if (t < 900) moveRobot(0, 0.8);
  else {
    evitarActivo = false;
    estadoActual = estadoAnterior;
  }
}

// =================================================
void ejecutarObstaculo() {

  ledBlink(LED_A,80);

  if(!obstActivo){

    obstActivo=true;
    obstStartTime=millis();

    long dL = leerDistancia(USL_TRIG,USL_ECHO);
    long dC = leerDistancia(USC_TRIG,USC_ECHO);
    long dR = leerDistancia(USD_TRIG,USD_ECHO);

    if(dL<20) obstDir=1;
    else if(dR<20) obstDir=-1;
    else obstDir=random(0,2)?1:-1;
  }

  unsigned long t = millis()-obstStartTime;

  if(t<400) moveRobot(-0.35,0);
  else if(t<1200) moveRobot(0,obstDir*0.8);
  else{
    obstActivo=false;
    estadoActual=estadoAnterior;
  }
}

// =================================================
void setup() {

  Serial.begin(115200);

  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_C, OUTPUT);

  pinMode(LMot_dir, OUTPUT);
  pinMode(LMot_pwm, OUTPUT);
  pinMode(RMot_dir, OUTPUT);
  pinMode(RMot_pwm, OUTPUT);

  attachInterrupt(L_EncA, isr_LA, RISING);
  attachInterrupt(R_EncA, isr_RA, RISING);

  moveRobot(0,0);
}

// =================================================
void loop() {

  leerSerial();
  
  long dL = leerDistancia(USL_TRIG,USL_ECHO);
  long dC = leerDistancia(USC_TRIG,USC_ECHO);
  long dR = leerDistancia(USD_TRIG,USD_ECHO);

  if (flag_mar && estadoActual != EVITAR_MAR) {
    estadoAnterior = estadoActual;
    estadoActual = EVITAR_MAR;
  }

  if ((dL<20 || dC<20 || dR<20) && estadoActual != EVITAR_OBSTACULO) {
    estadoAnterior = estadoActual;
    estadoActual = EVITAR_OBSTACULO;
  }

  switch (estadoActual) {

    case INICIO:
      estadoActual = BUSCAR_LATA;
      break;

    case BUSCAR_LATA:
      ledBlink(LED_B,200);
      moveRobot(0,0.25);
      if (ang_lata != 540) estadoActual = GIRAR_A_LATA;
      break;

    case GIRAR_A_LATA: {

      if (ang_lata == 540) { estadoActual = BUSCAR_LATA; break; }

      float g = PID_Giro(ang_lata);
      moveRobot(0,g/255.0);

      if (abs(ang_lata)<8) estadoActual = IR_A_LATA;
    } break;

    case IR_A_LATA: {

      if (ang_lata == 540) { estadoActual = BUSCAR_LATA; break; }

      float g = PID_Giro(ang_lata);
      moveRobot(0.35,g/255.0);

      if(flag_lata){
        timerRecoger=true;
        recogerStartTime=millis();
        estadoActual=RECOGER_LATA;
      }

    } break;

    case RECOGER_LATA:{

      moveRobot(0.3,0);

      if(timerRecoger && millis()-recogerStartTime>2000){
        timerRecoger=false;
        estadoActual=BUSCAR_CONT;
      }

    } break;

    case BUSCAR_CONT:{

      ledBlink(LED_B,300);
      moveRobot(0,0.25);

      if(hayCont) estadoActual=GIRAR_A_CONT;

    } break;

    case GIRAR_A_CONT: {

      if(!hayCont){ estadoActual=BUSCAR_CONT; break;}

      float g=PID_Giro(ang_cont);
      moveRobot(0,g/255.0);

      if(abs(ang_cont)<8) estadoActual=IR_A_CONT;

    } break;

    case IR_A_CONT: {

      if(!hayCont){ estadoActual=BUSCAR_CONT; break;}

      float g=PID_Giro(ang_cont);
      moveRobot(0.35,g/255.0);

      if(flag_cont){
        timerEntregar=true;
        entregarStartTime=millis();
        estadoActual=ENTREGAR;
      }

    } break;

    case ENTREGAR:{

      moveRobot(0,0);
      ledBlink(LED_C,150);

      if(timerEntregar && millis()-entregarStartTime>1500){
        timerEntregar=false;
        estadoActual=BUSCAR_LATA;
      }

    } break;

    case EVITAR_MAR:{
      ejecutarEvitar();
    } break;

    case EVITAR_OBSTACULO:{
    ejecutarObstaculo();
    } break;

  }
}

