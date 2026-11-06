
#include <PMS.h>

// UART1 - osobny port dla PMS7003
PMS pms(Serial1);
PMS::DATA data;

unsigned long lastMessage = 0;
unsigned long count = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // ESP32-C6: UART1, RX=GPIO4, TX=GPIO5
  Serial1.begin(9600, SERIAL_8N1, 4, 5);

  Serial.println("\n================================");
  Serial.println("       PMS7003 DIAGNOSTICS");
  Serial.println("================================");
  Serial.println("UART1 RX: GPIO4");
  Serial.println("UART1 TX: GPIO5");
  Serial.println("Baudrate: 9600");
  Serial.println("Waiting for PMS7003...");
}

void loop() {

  if (pms.read(data)) {

    count++;
    lastMessage = millis();

    Serial.println("\n========== MEASUREMENT ==========");

    Serial.print("Measurement number: ");
    Serial.println(count);

    Serial.print("PM1.0: ");
    Serial.print(data.PM_AE_UG_1_0);
    Serial.println(" ug/m3");

    Serial.print("PM2.5: ");
    Serial.print(data.PM_AE_UG_2_5);
    Serial.println(" ug/m3");

    Serial.print("PM10:  ");
    Serial.print(data.PM_AE_UG_10_0);
    Serial.println(" ug/m3");

    Serial.println("STATUS: VALID DATA");
    Serial.println("================================");
  }

  // Status co 3 sekundy
  if (millis() - lastMessage > 3000) {
    static unsigned long lastWarning = 0;

    if (millis() - lastWarning > 3000) {
      lastWarning = millis();

      Serial.println("[WARNING] No valid PMS measurement!");
      Serial.print("UART bytes waiting: ");
      Serial.println(Serial1.available());
    }
  }

  delay(10);
}
