#!/usr/bin/env bash
source _functions.sh

mkdir -p sketches
cd sketches

# Sketches from Git-Repositories
GIT_REPOS=(
  # Jerome
  "https://github.com/jp112sdl/Beispiel_AskSinPP.git"
  "https://github.com/jp112sdl/HB-UNI-Sen-LEV-US.git"
  "https://github.com/jp112sdl/HB-UNI-Sen-DIST-US.git"
  "https://github.com/jp112sdl/HB-UNI-Sen-CAP-MOIST.git"
  "https://github.com/jp112sdl/HB-UNI-SenAct-4-4.git"
  "https://github.com/jp112sdl/HB-UNI-DMX-MASTER.git"
  "https://github.com/jp112sdl/HB-UNI-Sen-PRESS.git"
  "https://github.com/jp112sdl/HB-UNI-Sen-WEIGHT.git"
  "https://github.com/jp112sdl/HB-Dis-EP-42BW.git"
  "https://github.com/jp112sdl/HB-OU-MP3-LED.git"
  "https://github.com/jp112sdl/HB-RC-12-EP.git"
  "https://github.com/jp112sdl/HB-RC-4-Dis-TH.git"
  "https://github.com/jp112sdl/HB-OU-MOT-SERVO.git"
  "https://github.com/jp112sdl/HB-UNI-Sen-RFID-RC.git"
  "https://github.com/jp112sdl/HB-UNI-Sen-TEMP-MAX6675.git"
  "https://github.com/jp112sdl/HM-SEC-RHS-ATmega168.git"

  # TomMajor
  "https://github.com/TomMajor/SmartHome.git"
  
  # Papa
  "https://github.com/pa-pa/HB-Sec-RHS-3.git"
)

# Clone / update Repos in parallel
# downloadGitRepos $GIT_REPOS
downloadGitRepos "${GIT_REPOS[@]}"
