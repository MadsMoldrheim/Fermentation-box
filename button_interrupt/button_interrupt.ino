int output = 5;
int input = 3;

void setup() {
  Serial.begin(9600);
  pinMode (output, OUTPUT);
  digitalWrite (output, LOW);
  //pinMode (input, INPUT);
  attachInterrupt(digitalPinToInterrupt(input), buttonpress, RISING);

}

void loop() {
  delay (500);
  digitalWrite(output, LOW);
  delay (500);

}

void buttonpress() {
  digitalWrite(output, HIGH);
}
