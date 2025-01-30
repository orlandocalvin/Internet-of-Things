const int red = 2, green = 3, blue = 4;

void setup() {
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
}

void loop() {
  analogWrite(red, 255);
  analogWrite(green, 0);
  analogWrite(blue, 0);
  // ^ Red ^ //
  delay(1000);

  RGB_color(255, 255, 255); // White
  delay(1000);
  
  RGB_color(0, 0, 255); // Blue
  delay(1000);
}

void RGB_color(int redValue, int greenValue, int blueValue) {
  analogWrite(red, redValue);
  analogWrite(green, greenValue);
  analogWrite(blue, blueValue);
}
