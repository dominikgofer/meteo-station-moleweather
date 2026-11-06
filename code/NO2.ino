#define NO2_PIN 1

void setup() {
  Serial.begin(115200);

  analogReadResolution(12);
  analogSetPinAttenuation(NO2_PIN, ADC_11db);

  delay(1000);
  Serial.println("SEN0574 NO2 sensor");
}

void loop() {
  const int samples = 20;
  long sum = 0;

  for (int i = 0; i < samples; i++) {
    sum += analogRead(NO2_PIN);
    delay(10);
  }

  float adc = sum / (float)samples;
  float voltage = adc * 3.3 / 4095.0;

  Serial.print("ADC: ");
  Serial.print(adc, 1);

  Serial.print(" | Voltage: ");
  Serial.print(voltage, 3);

  Serial.println(" V");

  delay(1000);
}