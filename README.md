# Tic Tac Toe on RTOS

**This was done as a part of RTOS course**

Ofcourse there is no reason to run Tic Tac Toe on a RTOS as there is no strict deadlines, but for the sake of learning this was done.

## About the Project
This project implements a simple Tic Tac Toe game on an STM32F429-DISCO using an RTOS (uC/OS-III). 
The game uses a touchscreen interface, an LCD for display, and separate tasks for each player (Circle and Cross) and for game state checking.
The game grid check size can also be changed to play Tic Tac Toe on larger than tradiotional 3x3 grid.


![ezgif-6079c2c0e039b6](https://github.com/user-attachments/assets/70ab73df-b350-4303-bd12-d83d2cc27d15)



## Features

- Multitasking with uC/OS-III
- Touchscreen-based player input using interrupts
- Memory Pool allocation for shared resources
- Mutex-protected game state matrix
- Semaphore-based touchscreen and LCD access
- Flags for turn-taking between players
- Win condition detection and display

## Task Breakdown

- **Start Task**: Initializes game and spawns all other tasks
- **Circle Task**: Handles input and drawing for Circle player
- **Cross Task**: Handles input and drawing for Cross player
- **Game State Checker**: Checks the board state periodically and reports winner

## Dependencies
This project uses the following depencies, which are already included in the project.
- uC/OS-III RTOS
- STM32 HAL drivers
- STM32F4 BSP
  
## Build Instructions

This project uses **CMake** for building. Follow the steps below to configure and build the project:

### Prerequisites

- CMake
- ARM GCC toolchain (e.g., `arm-none-eabi-gcc`)
- Make or Ninja
- ST-Link tools for flashing and debugging

You can use the following command to install these requirements

``sudo apt install -y cmake gcc-arm-none-eabi ninja-build stlink-tools``

### Steps
These instructions are meant to be used for VScode, just make sure that you are connected to WSL if not using a native Linux machine.
1. **Clone the repository**

   ```bash
   git clone https://github.com/LauriAlanen/STM32F4-DISCO-TICTACTOE.git
   cd STM32F4-DISCO-TICTACTOE

2. **Install CMake Tools (VSCode)**
- Link to the extension : https://marketplace.visualstudio.com/items/?itemName=ms-vscode.cmake-tools

3. **Navigate to the CMake Extension**
- Now you can buid the project. Preconfigured POST_BUILD commands defined in `CMakeLists.txt` will automatically flash the target board as long as its connected and `st-flash` is available.

4. **Play the game**
- Now a game should start on your STM32F4-DISCO and you should be able to play it.
- You can adjust the playable grid size from `main.h`

## License

This project is for educational purposes.
