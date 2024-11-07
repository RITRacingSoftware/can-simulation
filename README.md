# STM32G4xx Project Template (CAN TELEMETRY TESTING)

## Overview
This is a Github template with everything needed to make a new STM32G4xx project from scratch.
This is not meant to be edited with production code.
To use it, go to the Github website view of this repository, and in the top right corner, 
create a clone of this template with the "Use This Template" button.

## Requirements
### Windows-Specific:
- [Windows Subsystem for Linux](https://learn.microsoft.com/en-us/windows/wsl/install)
- [Docker Desktop](https://docs.docker.com/desktop/install/windows-install/)

Not strictly required, but advised, is Github Desktop.

## SETUP INSTRUCTIONS

```text

Run “scripts/enter-docker.sh”
This will start an instance of the docker image and ssh you into it in its terminal
Run “make”
This will build the project and link in the submodules
This should put a lot of rolling text onto the screen, then finish without an error
NOT IN THE DOCKER TERMINAL, run “scripts/flash.sh”
At this point, the code should flash with no errors
```