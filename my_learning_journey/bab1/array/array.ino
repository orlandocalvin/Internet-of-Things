int led[5] = {2,3,4,5,6};

void setup() {
  for(int i=0;i<5;i++) {
    pinMode(led[i], OUTPUT);
  }
}

void loop() {
  for(int i=0;i<5;i++) {
    digitalWrite(led[i], HIGH);
    delay(500);
    digitalWrite(led[i], LOW);
    delay(500);
    if(i == 4) {
      for(int j=3;j>0;j--) {
        digitalWrite(led[j], HIGH);
    	delay(500);
    	digitalWrite(led[j], LOW);
    	delay(500);
      }
    }
  }
}