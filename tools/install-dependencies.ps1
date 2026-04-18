$ErrorActionPreference = "Stop"

$arduinoLibsPath = "$env:USERPROFILE\Documents\Arduino\libraries"

# Create the libraries folder if it does not exist yet
if (-not (Test-Path $arduinoLibsPath)) {
    New-Item -ItemType Directory -Path $arduinoLibsPath -Force | Out-Null
}

# ── RF24 ────────────────────────────────────────────────────────────────────
$rf24Dest = "$arduinoLibsPath\RF24"

if (Test-Path $rf24Dest) {
    Write-Host "RF24 already installed, skipping."
} else {
    Write-Host "Downloading RF24..."
    $zipPath = "$env:TEMP\RF24.zip"
    Invoke-WebRequest -Uri "https://github.com/nRF24/RF24/archive/refs/heads/master.zip" `
                      -OutFile $zipPath `
                      -UseBasicParsing

    Write-Host "Installing RF24..."
    Expand-Archive -Path $zipPath -DestinationPath $arduinoLibsPath -Force
    Rename-Item -Path "$arduinoLibsPath\RF24-master" -NewName "RF24"
    Remove-Item $zipPath

    Write-Host "RF24 installed."
}

# ── Add further libraries here as the project grows ─────────────────────────
