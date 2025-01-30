int ledM = 3;
const int ledH = 4;
int ledB = 5;
int dUp = 500;
int dDown = 25;
int lampu = 0;

void setup() {
   Serial.begin(9600);
  pinMode(ledM, OUTPUT);
  pinMode(ledH, OUTPUT);
  pinMode(ledB, OUTPUT);
}

void loop() {
  /* For Loop
  	for(int i=0;i<9;i++) {
    digitalWrite(ledM, HIGH);
    delay(500);
    digitalWrite(ledM, LOW);
    delay(500);
  }*/
  
  /* While Loop
  int i = 0;
  while(i<5) {
    digitalWrite(ledM, HIGH);
    delay(500);
    digitalWrite(ledM, LOW);
    delay(500);
    i++;
  }*/
  
  if(lampu == 0) {
    lampu = ledM;
  } else if (lampu == ledM) {
    lampu = ledB;
  } else if (lampu == ledB) {
    lampu = ledM;
  }
  
  // LED Merah
  for(int i=0;i<20;i++) {
    digitalWrite(lampu, HIGH);
    delay(dUp-(i*(dUp*0.05)));
    digitalWrite(lampu, LOW);
    delay(dUp-(i*(dUp*0.05)));
    // Serial.println(dUp-(i*(dUp*0.05)));
  }
  
  // LED Hijau
  for(int i=0;i<20;i++) {
    digitalWrite(ledH, HIGH);
    delay(dDown+(i*dDown));
    digitalWrite(ledH, LOW);
    delay(dDown+(i*(dUp*0.05)));
    // Serial.println(dDown+(i*dDown));
  }
  
  // LED Biru
  /*for(int i=0;i<20;i++) {
    digitalWrite(ledB, HIGH);
    delay(dUp-(i*(dUp*0.05)));
    digitalWrite(ledB, LOW);
    delay(dUp-(i*(dUp*0.05)));
    // Serial.println(dUp-(i*(dUp*0.05)));
  }*/
}