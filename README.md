# CSOPESY Emulator

This is a simple OS console emulator with a scrolling text banner.

## How to Build

Open your terminal. Make sure you are in the project root folder (`CSOPESY-MO3-Group5`).

Run this command:

```
g++ -std=c++14 -Iinclude src/main.cpp src/Console.cpp src/Marquee.cpp src/Screen.cpp -o csopesy.exe -pthread
```

Use `-std=c++17` instead if your compiler supports it; the code builds under
either. The g++ bundled with Dev-Cpp (4.9.2) only understands `c++14`.

If you use CMake, run these commands from the root folder:

```
cmake -B build
cmake --build build
```

## How to Run

Make sure you are still in the project root folder (`CSOPESY-MO3-Group5`).

Run the executable from your terminal:

```
./csopesy.exe
```

If you built with CMake, run:

```
./build/csopesy.exe
```

## How to Use It

Once the program opens, you will see a `Command>` prompt. Type any of these commands and press Enter:

| Command | What it does |
| --- | --- |
| `start_marquee` | Starts the scrolling ASCII banner animation |
| `stop_marquee` | Stops the scrolling animation and restores the banner |
| `set_text <text>` | Sets custom marquee text, shown exactly as typed |
| `set_speed <ms>` | Sets animation refresh rate in milliseconds |
| `help` | Shows the list of commands |
| `exit` | Closes the emulator |

## File Structure

```
CSOPESY-MO3-Group5/
├── docs/
│   └── specs.md
├── include/
│   ├── Console.h
│   ├── Marquee.h
│   └── Screen.h
├── src/
│   ├── Console.cpp
│   ├── Marquee.cpp
│   ├── Screen.cpp
│   └── main.cpp
├── .gitignore
├── CMakeLists.txt
└── README.md
```

Here is what each file does:

| File | Description |
| --- | --- |
| `docs/specs.md` | Contains the project requirements and assignment specs |
| `include/Console.h` | Declares the Console class and CLI state |
| `include/Marquee.h` | Declares the Marquee animation class and thread controls |
| `src/main.cpp` | Entry point that launches the console |
| `src/Console.cpp` | Renders the banner, reads user input, and handles commands |
| `src/Marquee.cpp` | Handles the background thread and scrolling text animation |
| `include/Screen.h` | Declares the shared console lock, layout rows, and key input |
| `src/Screen.cpp` | Owns terminal setup, window size, scroll region, and key polling |
| `CMakeLists.txt` | Build setup for CMake |
| `.gitignore` | Tells git which build files to ignore |
| `README.md` | Explains how to build, run, and use the project |
