# Automated Birdcage Management System

This project implements an automated system for managing pet birdcages, focusing on automating feeding, water refill, and cage cleaning using ultrasonic sensors, water level sensors, and microcontrollers. The system also integrates with Arduino IoT Cloud for remote monitoring and notifications, providing a hands-free experience for bird owners, especially during long absences.

## Table of Contents
- [Installation](#installation)
- [Usage](#usage)
- [Features](#features)
- [Configuration](#configuration)
- [System Architecture](#system-architecture)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

## Installation

Follow these steps to install and run the project:

### Prerequisites
- Arduino IDE
- Arduino Uno R4 WiFI
- Arduino IoT Cloud Maker License
- HC-SR04 Ultrasonic sensor
- Water level sensor
- Servo motor
- IoT-enabled device for integration with Arduino IoT Cloud

### Setup

1. **Clone the repository**:
    ```bash
    git clone https://github.com/sof4raway/automated-birdcage-system.git
    cd automated-birdcage-system
    ```

2. **Install Arduino Libraries**:
   Ensure the following libraries are installed in your Arduino IDE:
   - `ArduinoIoTCloud`
   - `ArduinoCloudThing`
   - `NewPing`
   - `uRTCLib`
   - `DHT Sensor Library`
   - `AccelStepper`

3. **Upload the Code to Arduino**:
   Connect your Arduino board and upload the provided `.ino` file using the Arduino IDE.

4. **Wiring**:
   Follow the schematic in the documentation to connect the sensors and actuators to the microcontroller.

5. **Set up Arduino IoT Cloud**:
   - Configure your Arduino IoT Cloud dashboard with the appropriate variables to monitor sensor data.
   - Connect your Arduino board to WiFi and link it to the IoT Cloud for remote monitoring.

## Usage

Once the code is uploaded and the wiring is complete:

1. **Automatic Feeding and Watering**: The system automatically monitors and refills the bird's food and water using the connected sensors.
2. **Cage Cleaning**: The automatic scrubbing system is activated according to a schedule, ensuring the cage remains clean with minimal residue.
3. **Remote Monitoring**: Use the Arduino IoT Cloud dashboard or mobile app to monitor the bird's conditions, including food and water levels, and receive notifications.

## Features
- Automated Feeder Refill System: Monitors and refills the food with a tolerance of ±1.0 cm
- Atutomated Water Refill System: Monitors and refills the water with a tolerance of ±0.5 cm
- Automated Bird Dropping Cleaning: Scrubs and Spray Water to reduce manual maintenance
- Disinfectant Sprayer: periodic disinfection
- Remote Monitoring: Arduino IoT Cloud integration for real-time updates and notifications.

## Configuration
Make sure to configure the environment variables for WiFi and IoT Cloud settings in your Arduino code:

```bash
const char SSID[] = "your_wifi_ssid";
const char PASS[] = "your_wifi_password";
```
For Arduino IoT Cloud integration, use the appropriate cloud dashboard and ensure your device is registered and linked to the correct variables.

## System Architecture
The project structure is as follows:
``` bash
project-root/
├── src/
│   ├── capstone-ta_may09a.ino          # Main Arduino code
│   ├── thingProperties.h # Arduino IoT Cloud Properties
│   ├── ArduinoSecrets.h # Configuration etc.
└── README.md
```

## Citation
If you find this work useful for your research, please consider citing the following paper:

- **Author** : Muhammad Farid Rahman, and Rifki Suwandi
- **Title** : Rancang Bangun Sistem Otomatisasi Sangkar Burung Berbasis Mikrokontroler
- **Type** : Theses
- **Publisher** : Andalas University
- **Year** : 2024

Here are some BibTex Formate on APA, IEEE, and Harvard Style :
#### APA Style
```bibtex
@thesis{Rahman2024,
  author    = {Muhammad Farid Rahman and Rifki Suwandi},
  title     = {Rancang Bangun Sistem Otomatisasi Sangkar Burung Berbasis Mikrokontroler},
  year      = {2024},
  school    = {Andalas University},
  type      = {Thesis}
}
```

#### IEEE Style
```bibtex
@thesis{Rahman2024,
  author    = {M. F. Rahman and R. Suwandi},
  title     = {Rancang Bangun Sistem Otomatisasi Sangkar Burung Berbasis Mikrokontroler},
  year      = {2024},
  school    = {Andalas University},
  type      = {Thesis}
}
```
#### Harvard Style
```bibtex
@thesis{Rahman2024,
  author    = {Rahman, M. F. and Suwandi, R.},
  title     = {Rancang Bangun Sistem Otomatisasi Sangkar Burung Berbasis Mikrokontroler},
  year      = {2024},
  school    = {Andalas University},
  type      = {Thesis}
}
```

## Contact 
For any questions or feedback, feel free to reach out:
- **Name** : Muhammad Farid Rahman
- **Email** : muhammadfaridrahman.mfr@gmail.com
- **GitHub** : sof4raway
