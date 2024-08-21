
---

# Seat Heater Control System

## Table of Contents

1. [Introduction](#introduction)
2. [System Overview](#system-overview)
3. [System Features](#system-features)
4. [Functionality](#functionality)
5. [Hardware Components](#hardware-components)
6. [Screenshots](#screenshots)
7. [Simso Diagram](#simso-diagram)

---

## Introduction

This document outlines the design of a Real-Time Operating System (RTOS) project aimed at controlling a vehicle's seat heating system using embedded systems. The project utilizes FreeRTOS as the operating system and the Tiva-C microcontroller as the target platform. It demonstrates the application of real-time operating systems in embedded environments, focusing on efficient task scheduling, resource management, and system reliability.

## System Overview

The system consists of several tasks designed to perform the full functionality:

1. **Button Press Management Task:** An event-based function triggered when the user (driver or passenger) presses a button to adjust the temperature. This function has two instances—one for the driver’s seat and one for the passenger’s seat.

2. **Temperature Monitor Task:** A periodic function that reads the current temperature of both seats using the ADC every 300 ms. This function has a single instance for both seats.

3. **Heat Control Task:** A periodic task executed every 500 ms that responds to the Button Press Management event to adjust the temperature to the desired level. This task has two instances—one for each seat.

4. **Display Update Task:** A periodic task that updates the display screen every 3 seconds. It has a single instance.

5. **CPU Load Measurement Task:** A periodic task called every 2 seconds to calculate CPU load. It has a single instance.

6. **Failure Logging Task:** A periodic function executed every 1.5 seconds to check and display failure logs if found. It has a single instance.

All tasks operate concurrently and communicate as necessary to ensure proper functionality and safe system operation.

## System Features

- **Heating Levels:**
  - **Off:** Heating is disabled.
  - **Low:** Target temperature of 25°C.
  - **Medium:** Target temperature of 30°C.
  - **High:** Target temperature of 35°C.

- **Temperature Control:** Maintains temperature within ±2°C of the target.
- **Diagnostics:** Detects and logs sensor failures (range: 5°C-40°C). Displays errors on the screen and turns on a red LED when the sensor fails.

- **Buttons:**
  - **Middle Console:** Controls each seat heater.
  - **Steering Wheel:** Controls the driver’s seat heater.

## Functionality

1. **Set Heating Level:**
   - Press the button to cycle through Off, Low, Medium, and High settings.

2. **Heater Control:**
   - **High Intensity:** If current temperature is ≥10°C below the target.
   - **Medium Intensity:** If current temperature is 5-10°C below the target.
   - **Low Intensity:** If current temperature is 2-5°C below the target.
   - **Disabled:** If current temperature is above the target.

   **Testing:** The heating element is simulated with LEDs:
   - **Green:** Low intensity.
   - **Blue:** Medium intensity.
   - **Cyan:** High intensity.

3. **Temperature Measurement:**
   - **Sensor:** LM35 or potentiometer (0-3.3V mapped to 0-45°C, valid range 5-40°C).
   - **ADC:** Used for accurate temperature readings.

4. **Display and Diagnostics:**
   - **Screen:** Shows current temperature, heating level, and heater status.
   - **Red LED:** Indicates sensor failure.

## Hardware Components

- **Each Seat Includes:**
  - Temperature sensor (simulated as a potentiometer).
  - Red LED (error reporting).
  - Heater (simulated with green and blue LEDs).
  - Button (heating level control).
- **Driver’s Seat:** Additional button on the steering wheel.

## Screenshots

Here are some screenshots of the system’s display:

1. **Screen Display:**
![](images/Screenshot2.png)

   *Description: Screenshot showing the current temperature, heating level, and heater status for both seats.*

2. **Run Time Measurments Display:**
![](images/Screenshot.png)
   *Description: Screenshot of the display the run time measurmenets*

## Simso Diagram

The Simso simulation diagram for the project is shown below:

![](images/Screenshot3.png)
*Description: Diagram illustrating the task scheduling and system operation as simulated in Simso.*

---

