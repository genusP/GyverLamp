#!/usr/bin/env bash
set -e

if ! command -v "arduino-cli" >/dev/null 2>&1; then
    echo Install Arduino CLI
    sudo sh -c 'curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=/usr/bin sh'
fi;

if [ ! -f /home/codespace/.arduino15/arduino-cli.yaml ]; then
    arduino-cli config init
fi;

rm /home/codespace/.arduino15/arduino-cli.yaml
ln -s /workspaces/GyverLamp/arduino-cli.yaml /home/codespace/.arduino15/arduino-cli.yaml

arduino-cli core update-index
arduino-cli core install esp8266:esp8266
