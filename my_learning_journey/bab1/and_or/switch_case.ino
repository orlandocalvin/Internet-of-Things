const int ledM = 10;
const int ledH = 11;
const int ledK = 12;
const int pbKiri = 5;
const int pbKanan = 6;

int i = 0;
int hbKiri;
int hbKanan;

void setup() {
  // Serial.begin(9600);
  pinMode(ledM, OUTPUT);
  pinMode(ledH, OUTPUT);
  pinMode(ledK, OUTPUT);
  pinMode(pbKiri, INPUT);
  pinMode(pbKanan, INPUT);
}

void loop() {
  hbKiri = digitalRead(pbKiri);
  hbKanan = digitalRead(pbKanan);
  
  if(hbKiri == 1) {
  	i--;
    delay(300);
  } else if(hbKanan == 1) {
  	i++;
    delay(300);
  }
  
  switch(i) {
    case -1 :
    	digitalWrite(ledM, LOW);
    	digitalWrite(ledH, LOW);
    	digitalWrite(ledK, HIGH);
    	i = 3;
    	break;
  	case 1 :
    	digitalWrite(ledM, HIGH);
    	digitalWrite(ledH, LOW);
    	digitalWrite(ledK, LOW);
    	break;
    case 2 :
    	digitalWrite(ledM, LOW);
    	digitalWrite(ledH, HIGH);
    	digitalWrite(ledK, LOW);
    	break;
    case 3 :
    	digitalWrite(ledM, LOW);
    	digitalWrite(ledH, LOW);
    	digitalWrite(ledK, HIGH);
    	break;
    default :
    	digitalWrite(ledM, LOW);
    	digitalWrite(ledH, LOW);
    	digitalWrite(ledK, LOW);
    	i = 0;
    	break;
  }
  // Serial.println(i);
}




























