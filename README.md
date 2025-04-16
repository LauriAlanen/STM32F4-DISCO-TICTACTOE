# Tic Tac Toe on RTOS

**A learning project for an RTOS course**

While Tic Tac Toe doesn't inherently require the complexity of an RTOS, this project demonstrates how to implement multitasking and resource management on a real-time operating system. The game runs on an STM32F429-DISCO board using uC/OS-III and showcases touchscreen input, LCD graphics, and modular task design.

## About the Project

This project implements a simple Tic Tac Toe game on an STM32F429-DISCO. It uses a touchscreen for input, an LCD for display, and separate tasks for each player (Circle and Cross) along with a dedicated game state checker task. The game grid size is configurable, allowing for variations beyond the traditional 3x3 board.

![Tic Tac Toe on RTOS](https://github.com/user-attachments/assets/70ab73df-b350-4303-bd12-d83d2cc27d15)

## Features

- **Multitasking with uC/OS-III:** Demonstrates real-time task management.
- **Touchscreen-based Input:** Uses interrupts for responsive touch detection.
- **Memory Pool Allocation:** Manages shared resource allocation efficiently.
- **Mutex-Protected Game State:** Ensures safe access to the game grid.
- **Semaphore-Based Resource Control:** Coordinates touchscreen and LCD usage.
- **Flag-Based Turn-Taking:** Controls the game flow between players.
- **Win Condition Detection:** Automatically checks and displays the game outcome.

## Task Breakdown

- **Start Task:** Initializes peripherals and spawns all other tasks.
- **Circle Task:** Manages input and drawing for the Circle player.
- **Cross Task:** Manages input and drawing for the Cross player.
- **Game State Checker Task:** Periodically evaluates the board to detect a win.

## Dependencies

The project includes the following dependencies:

- uC/OS-III RTOS
- STM32 HAL drivers
- STM32F4 BSP

## Build Instructions

This project uses **CMake** for building. Follow the steps below to configure and build the project.

### Prerequisites

- CMake
- ARM GCC toolchain (e.g., `arm-none-eabi-gcc`)
- Make or Ninja
- ST-Link tools for flashing and debugging

Install the prerequisites on Ubuntu using:

```bash
sudo apt install -y cmake gcc-arm-none-eabi ninja-build stlink-tools
```

## Setting Up WSL (If not using native Linux)

If you're not running native Linux, you can use Windows Subsystem for Linux (WSL) and Visual Studio Code to easily compile this project.

### 1. Install WSL

- **Open PowerShell as Administrator** and run:
  
  ```powershell
  wsl --install
  ```
This installs WSL 2 and the default Ubuntu distribution.
Restart your computer when prompted.
Launch Ubuntu from the Start menu and follow the prompts to create your Linux user account.

### 2. Install the Remote - WSL Extension (Host):
- Install WSL Extension to your Host VScode if not already installed. https://marketplace.visualstudio.com/items/?itemName=ms-vscode-remote.remote-wsl
- Press Ctrl+Shift+P to open the Command Palette. Enter Connect to WSL (or press the blue WSL button on the bottom left corner of your screen).

![image](https://github.com/user-attachments/assets/f85b2b31-2ac9-4d8a-a3fc-2ec6de3cdf3e)

### 3. Install dependencies to WSL
```bash
sudo apt install -y cmake gcc-arm-none-eabi ninja-build stlink-tools
```
 
## Compiling
These instructions are meant to be used for VScode, just make sure that you are connected to WSL if not using a native Linux machine.
1. **Clone the repository**

   ```bash
   git clone https://github.com/LauriAlanen/STM32F4-DISCO-TICTACTOE.git
   cd STM32F4-DISCO-TICTACTOE

2. **Install CMake Tools (VSCode)**
- Link to the extension : https://marketplace.visualstudio.com/items/?itemName=ms-vscode.cmake-tools

3. **Navigate to the CMake Extension**
- Now you can buid the project. Preconfigured POST_BUILD commands defined in `CMakeLists.txt` will automatically flash the target board as long as its connected and `st-flash` is available from `st-tools`.

4. **Play the game**
- Now a game should start on your STM32F4-DISCO and you should be able to play it.
- You can adjust the playable grid size from `main.h`

## License

This project is for educational purposes.
