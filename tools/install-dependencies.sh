#!/bin/bash
echo "Installing EVolocityArduino dependencies..."
echo

if ! command -v arduino-cli &> /dev/null; then
    echo "ERROR: arduino-cli not found."
    echo "Download it from https://arduino.github.io/arduino-cli/latest/installation/"
    exit 1
fi

arduino-cli lib install RF24 || { echo "ERROR: Failed to install RF24."; exit 1; }

echo
echo "All dependencies installed successfully."
