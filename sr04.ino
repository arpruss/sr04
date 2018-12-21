const uint32 triggerPin = PB3;
const uint32 echoPin = PB4;

void setup() {
  pinMode(triggerPin,OUTPUT);
  pinMode(echoPin,INPUT);
  Serial.begin(9600);
}

void pulse() {
  digitalWrite(triggerPin,1);
  delayMicroseconds(10);
  digitalWrite(triggerPin,0);
}

int32 measureEcho(int samples) {
  uint32 total = 0;
  for (int i=0; i<samples; i++) {  
    digitalWrite(triggerPin,1);
    delayMicroseconds(10);
    digitalWrite(triggerPin,0);
    
    uint32 t0 = micros();
    uint32 t;
    do {
      t = micros()-t0;
      if (t > 100000) {
        return -1;
      }
    }
    while (! digitalRead(echoPin));
    t0 = micros();
    do {
      t = micros()-t0;
      if (t > 100000) {
        return -1;
      }
    }
    while (digitalRead(echoPin));
    total += t;
  }

  return total / samples;
}

void loop() {
  int32 t = measureEcho(10);
  if (0 <= t) 
    Serial.println(t);
  else
    Serial.println("No echo");
  delay(100);
}

