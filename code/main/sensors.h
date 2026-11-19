#pragma once
#include <Arduino.h>
#include "types.h"

void beginSensors();
void pollSensors();

void readBME280(Reading &r);
void readProbe(Reading &r);
void readParticles(Reading &r);
void readOzone(Reading &r);
void readNO2(Reading &r);
void readGPS(Reading &r);

uint32_t readClockUTC(uint64_t ms);
bool getFormattedTimestamp(char *buffer, size_t len, uint64_t fallbackMs);
float readBMEAltitude();