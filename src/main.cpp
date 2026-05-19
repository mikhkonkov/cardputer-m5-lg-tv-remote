#include <Arduino.h>

#include "app/App.h"

static app::App g_app;

void setup() {
    g_app.setup();
}

void loop() {
    g_app.loop();
}
