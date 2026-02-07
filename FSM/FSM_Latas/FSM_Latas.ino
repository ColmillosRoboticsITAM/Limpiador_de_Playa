/* =========================================================
   FSM Mantis -- Latas
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
  EVITAR_MAR,
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

// -------------------- ENCODERS --------------------
volatile long ticksL = 0;
volatile long ticksR = 0;

// -------------------- PID --------------------
float kp = 1.3;
float ki = 0.0;
float kd = 0.15;
float errPrev = 0;
float integral = 0;

// -------------------- EVITAR MAR --------------------
unsigned long evitarStartTime = 0;
bool evitarActivo = false;

// -------------------- TIMER RECOGER LATA --------------------
unsigned long recogerStartTime = 0;
bool timerRecogerCorriendo = false;

// =================================================
// INTERRUPTS
// =================================================
void IRAM_ATTR isr_LA() { ticksL++; }
void IRAM_ATTR isr_RA() { ticksR++; }

// =================================================
// ULTRASONICOS
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
// SERIAL – VISION
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
// PID GIRO
// =================================================
float PID_Giro(float ang) {
  float err = ang;
  integral += err;
  float deriv = err - errPrev;
  errPrev = err;
  return (kp * err + ki * integral + kd * deriv);
}

// =================================================
// MOTORES
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
// BLINKER ORIGINAL
// =================================================
void ledBlink(int pin, int t) {
  static unsigned long last[3] = {0,0,0};
  static bool state[3] = {0,0,0};

  int i = (pin == LED_A) ? 0 : (pin == LED_B) ? 1 : 2;

  if (millis() - last[i] > (unsigned long)t) {
    last[i] = millis();
    state[i] = !state[i];
    digitalWrite(pin, state[i]);
  }
}

// =================================================
// EVITAR MAR
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
// SETUP
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

  pinMode(USL_TRIG, OUTPUT);
  pinMode(USC_TRIG, OUTPUT);
  pinMode(USD_TRIG, OUTPUT);

  pinMode(USL_ECHO, INPUT);
  pinMode(USC_ECHO, INPUT);
  pinMode(USD_ECHO, INPUT);

  attachInterrupt(L_EncA, isr_LA, RISING);
  attachInterrupt(R_EncA, isr_RA, RISING);

  moveRobot(0,0);
}

// =================================================
// LOOP
// =================================================
void loop() {

  leerSerial();

  // Evitar mar en cualquier estado
  if (flag_mar && estadoActual != EVITAR_MAR) {
    estadoAnterior = estadoActual;
    estadoActual = EVITAR_MAR;
  }

  switch (estadoActual) {

    case INICIO:
      digitalWrite(LED_A, HIGH);
      digitalWrite(LED_B, LOW);
      digitalWrite(LED_C, LOW);
      moveRobot(0,0);
      estadoActual = BUSCAR_LATA;
      break;

    case BUSCAR_LATA:
      digitalWrite(LED_B, HIGH);
      digitalWrite(LED_A, LOW);
      digitalWrite(LED_C, LOW);
      moveRobot(0, 0.25);
      if (ang_lata != 540) estadoActual = GIRAR_A_LATA;
      break;

    case GIRAR_A_LATA: {
      // Si se pierde la lata → regresa a buscar
      if (ang_lata == 540) {
        estadoActual = BUSCAR_LATA;
        break;
      }

      digitalWrite(LED_B, LOW);
      digitalWrite(LED_A, LOW);
      digitalWrite(LED_C, LOW);

      float g = PID_Giro(ang_lata);
      moveRobot(0, g / 255.0);

      if (abs(ang_lata) < 8)
        estadoActual = IR_A_LATA;
    }
    break;

    case IR_A_LATA: {
      // Si se pierde la lata → regresa a buscar
      if (ang_lata == 540) {
        estadoActual = BUSCAR_LATA;
        break;
      }

      digitalWrite(LED_A, LOW);
      digitalWrite(LED_C, HIGH);
      digitalWrite(LED_B, LOW);

      float g = PID_Giro(ang_lata);
      moveRobot(0.35, g / 255.0);

      if (flag_lata) {
        // ===== TIMER =====
        timerRecogerCorriendo = true;
        recogerStartTime = millis();

        estadoActual = RECOGER_LATA;
      }
    }
    break;

    case RECOGER_LATA: {
      digitalWrite(LED_A, HIGH);
      digitalWrite(LED_C, LOW);
      digitalWrite(LED_B, LOW);
      moveRobot(0.30, 0.0);

      // fin de timer
      if (timerRecogerCorriendo && (millis() - recogerStartTime >= 2000)) {
        timerRecogerCorriendo = false;
        moveRobot(0, 0);
        estadoActual = BUSCAR_LATA;
      }
    }
    break;

    case EVITAR_MAR: {
      ejecutarEvitar();
      break;
    }
  }
}
