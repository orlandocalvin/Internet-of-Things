#include <IRremote.h>

int pinIR = 2;
IRrecv irrecv(pinIR);
decode_results results;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
}

void loop() {
  if(irrecv.decode(&results)) {
    int value = results.value;
    Serial.println(value);
    irrecv.resume();
  } 
}