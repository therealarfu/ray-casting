# ray-casting
> It's a Ray Casting Algorithm in C with gaming controllers support, deltatime and SDL library.

<img width="802" height="627" alt="raycasting_BdJm7t7Atd" src="https://github.com/user-attachments/assets/b06cfcb1-90cb-46b5-888c-2b78b020fceb" />

## Features
* Real-time ray casting rendering
* Gamepad/controller support
* Delta time for frame-rate-independent movement
* Built with SDL2

## Controls
| Input | Action |
|---|---|
| ``W`` ``A`` ``S`` ``D`` | Move |
| ``<- ->`` (arrow keys) | Move |

## Getting Started
**Requirements**
* GCC
* SDL2

## Build and Run
**Using make**
```
make run
```
**Or manually**
```
gcc main.c -o raycasting -lmingw32 -lSDL2main -lSDL2
```

## References
* [Lode's Raycasting Tutorial](https://lodev.org/cgtutor/raycasting.html)
