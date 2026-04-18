#!/bin/bash
set -e

ARDUINO_LIBS="$HOME/Documents/Arduino/libraries"
mkdir -p "$ARDUINO_LIBS"

# ── RF24 ────────────────────────────────────────────────────────────────────
if [ -d "$ARDUINO_LIBS/RF24" ]; then
    echo "RF24 already installed, skipping."
else
    echo "Downloading RF24..."
    curl -L "https://github.com/nRF24/RF24/archive/refs/heads/master.zip" \
         -o /tmp/RF24.zip
    echo "Installing RF24..."
    unzip -q /tmp/RF24.zip -d "$ARDUINO_LIBS"
    mv "$ARDUINO_LIBS/RF24-master" "$ARDUINO_LIBS/RF24"
    rm /tmp/RF24.zip
    echo "RF24 installed."
fi

# ── Add further libraries here as the project grows ─────────────────────────

echo
echo "Setup complete! Open the Arduino IDE and you are ready to go."
