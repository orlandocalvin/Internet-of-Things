const int ledK = 3;
const int ledH = 4;
const int ledM = 5;

void setup() {
  pinMode(ledK, OUTPUT);
  pinMode(ledH, OUTPUT);
  pinMode(ledM, OUTPUT);
}

void loop() {
  kedip(3,ledM,500);
  kedip(2,ledH,500);
  kedip(1,ledK,500);
}

void kedip(int k, int led, int d) {
  int i = 1;
  while(i<=k) {
    digitalWrite(led, HIGH);
    delay(d);
    digitalWrite(led, LOW);
    delay(d);
    i++;
  }
  
  /* Function with For Loop
  for(int i=1;i<=k;i++) {
    digitalWrite(led, HIGH);
    delay(d);
    digitalWrite(led, LOW);
    delay(d);
  }*/
}










