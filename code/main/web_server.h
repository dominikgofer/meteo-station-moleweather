#pragma once
#include <stddef.h>
#include <stdint.h>
#include "types.h"

void initWebServerData(const Reading *historyBuf,
                       const size_t *countPtr,
                       const size_t *headPtr,
                       const uint32_t *seqPtr);

bool startAccessPoint();
void stopAccessPoint();
bool isAccessPointReady();
void loopWebServer();