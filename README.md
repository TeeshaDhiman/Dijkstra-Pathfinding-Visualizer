# Dijkstra PathFinding Visualizer

An integrated platform that allows users to book flights and view real-time simulations of flight routes using SFML graphics.

## Project Overview

This project blends logic and visual presentation through two integrated modules:
- A **Flight Booking System** where users can search for flights and input passenger data.
- A **Flight Simulator** that dynamically visualizes the chosen route with weather-based rerouting and smooth aircraft animation.

## Key Features

- Interactive terminal-based booking  
- Dynamic pricing based on travel date  
- Weather condition simulation and rerouting  
- Real-time flight animation on an SFML-rendered map  
- Modular C++ code for easy extension  
- Command-line or visual launch supported  
- SFML integration for graphics and animation  
- Windows build-ready with `.dll` dependencies included

## Components

### 1. Flight Booking (`flight_booking.cpp`)

The booking system provides a user-friendly terminal interface to:
- Select departure and arrival airports
- View flight options for 5 days with dynamic pricing
- Enter passenger details
- Confirm bookings
- Launch the flight simulator with the selected airports

### 2. Flight Simulator (`flight_simulator.cpp`)

The SFML-based flight simulator visualizes:
- Flight paths between airports
- Dynamic weather conditions
- Rerouting based on conditions
- Animated aircraft movement
- Map-based visualization

## Dependencies

- SFML 2.6.2 (Graphics Library)
  - Include Path: C:\SFML-2.6.2\include
  - Library Path: C:\SFML-2.6.2\lib
  - Required DLLs (place in project folder):
      - `sfml-graphics-2.dll`
      - `sfml-window-2.dll`
      - `sfml-system-2.dll`
      - `sfml-network-2.dll`
      - `sfml-audio-2.dll`
- **C++ Compiler** (C++11 or later  (e.g., `g++`))
- **Windows OS** (Current supported build)

##  Requirements
- C++ compiler supporting C++11 (e.g., g++)
- Windows OS (recommended for current build)
- SFML libraries (DLLs are included)

## How to Compile

### Method 1 : Using **compile.bat**
   - Just double-click compile.bat in your project folder. It will compile and link both programs (flight_booking.cpp and flight_simulator.cpp).

### Method 2 : Manual Compilation
   - ``cmd
     cd "C:\Users\USER\OneDrive\Desktop\Dijkstra PathFinder Visualizer"
     g++ -IC:\SFML-2.6.2\include -c flight_booking.cpp
     g++ flight_booking.o -o flight_booking.exe
     
     g++ -IC:\SFML-2.6.2\include -c flight_simulator.cpp
     g++ flight_simulator.o -o flight_simulator.exe -LC:\SFML-2.6.2\lib -lsfml-graphics -lsfml-window   -lsfml-system

## How to Run

1. **Booking & Simulation** : 
   flight_booking.exe     

2. **Run FLight Simulator Directly**:
   flight_simulator.exe

## Project Structure

- `flight_booking.cpp` - Source code for the booking system
- `flight_simulator.cpp` - Source code for the flight simulator
- `compile.bat` - Batch file for compiling the project
- `*.dll` - SFML library dependencies

## Features

- Dynamic flight pricing based on day selection
- Simulated weather impacts that alter flight routes dynamically
- Visual representation of the flight journey
- Seamless and fluid aircraft animation across the route
- Comprehensive booking workflow
- Integration between booking and simulation

## License

This project is created for educational purposes.

## Author

Teesha Dhiman