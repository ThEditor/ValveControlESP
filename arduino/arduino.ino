const int yourPin = 7; // Replace with the pin number you are using
const int relayPin = 4;

void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 bps
  pinMode(yourPin, INPUT); // Set the pin as an input
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
}

void loop() {
  int pinState = digitalRead(yourPin); // Read the state of the pin
  int relayState = digitalRead(relayPin); // Read the state of the relay pin

  if (pinState == HIGH && relayState == HIGH) {
    digitalWrite(relayPin, LOW); // Set relayPin LOW if it's not already LOW
    Serial.println("Turning relayPin LOW");
  } else if (pinState == LOW && relayState == LOW) {
    digitalWrite(relayPin, HIGH); // Set relayPin HIGH if it's not already HIGH
    Serial.println("Turning relayPin HIGH");
  }

  delay(1000); // Optional delay to avoid rapid serial prints
}
