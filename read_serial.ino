#define SENSOR_PIN A1

float meanValue = 0;
unsigned long lastSpikeTime = 0;
unsigned long timeSinceFirstSpike = 0;
const unsigned long spikeCooldown = 1000;
float maxRelativeDeviation = 0;  // Track the maximum relative deviation during the spike

void setup() {
  pinMode(SENSOR_PIN, INPUT);
  Serial.begin(9600);
  
  long sum = 0;
  int samples = 0;
  unsigned long startTime = millis();
  
  // Calibrate the mean value over 3 seconds
  while (millis() - startTime < 3000) {
    int sensorValue = analogRead(SENSOR_PIN);
    sum += sensorValue;
    samples++;
    delay(10);
  }
  
  meanValue = (float)sum / samples;  // Calculate mean value
  Serial.print("{\"meanValue\":");
  Serial.print(meanValue);
  Serial.println("}");
}

void loop() {
  float currentVal = analogRead(SENSOR_PIN);
  
  String jsonOutput = "{\"currentValue\":";
  jsonOutput += currentVal;
  
  // Calculate relative deviation from the mean
  float relativeDeviation = abs(currentVal - meanValue) / meanValue;  // Relative deviation as a fraction of the mean value

  if (currentVal < meanValue * 0.98) {
    if (timeSinceFirstSpike == 0) {
      // Spike starts
      timeSinceFirstSpike = micros();
      maxRelativeDeviation = relativeDeviation;  // Initialize with the first relative deviation
      jsonOutput += ", \"spikeStatus\":\"start\"";
      Serial.println(jsonOutput + "}");
    } else {
      // During the spike, keep track of the maximum relative deviation
      if (relativeDeviation > maxRelativeDeviation) {
        maxRelativeDeviation = relativeDeviation;
      }
    }
    lastSpikeTime = micros();
  } else {
    if ((micros() - lastSpikeTime > spikeCooldown * 1000) && (timeSinceFirstSpike != 0)) {
      // Spike ends
      unsigned long spikeDuration = micros() - timeSinceFirstSpike;

      // Convert the max relative deviation to a percentage
      float spikeIntensity = maxRelativeDeviation * 100.0;
      
      jsonOutput += ", \"spikeStatus\":\"end\", \"spikeDuration\":";
      jsonOutput += spikeDuration / 1000.0;
      jsonOutput += ", \"spikeIntensity\":";
      jsonOutput += spikeIntensity;
      jsonOutput += "}";

      // Reset spike tracking variables
      timeSinceFirstSpike = 0;
      maxRelativeDeviation = 0;
      Serial.println(jsonOutput);
    }
  }

  delay(50);
}
