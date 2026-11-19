#pragma once
#include <Arduino.h>
#include <math.h>
#include "config.h"

enum Metric : uint8_t { 
  AIR_TEMP,   
  HUMIDITY,   
  PRESSURE,   
  PROBE_TEMP, 
  PM1,        
  PM25,       
  PM10,       
  O3,         
  NO2,        
  METRIC_COUNT 
};

struct Reading { 
  uint32_t id = 0; 
  uint64_t ms = 0; 
  uint32_t utc = 0; 
  float v[METRIC_COUNT]; 
  double lat = NAN; 
  double lon = NAN; 
  float alt = NAN; 
  bool fix = false; 
  uint8_t sats = 0; 

  Reading() { 
    for (size_t i = 0; i < METRIC_COUNT; i++) { 
      v[i] = NAN; 
    }
  }
};