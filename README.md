# Perspective-Tilt-Image-Viewer
Custom image viewer in C with rotation, zoom, and perspective tilt using SDL2

A custom image viewer written in **pure C** using **SDL2**, featuring 
real-time **rotation**, **zoom**, and a **software-based perspective tilt**
effect inspired by 3D rendering pipelines.


## Features and Controls
- Real-time image rotation (← / →)
- Forward & backward perspective tilt (↑ / ↓)
- Smooth zoom (+ / -)
- ESC to exit the window
- Manual perspective projection (no shaders)
- Cross-platform (Windows / Linux)


## How it works
- Image is first rotated into a render target
- Perspective tilt is simulated by slicing the texture
- Each slice is scaled independently to simulate depth

This mimics a simplified 3D projection using only 2D rendering.

## Build (MSYS2 UCRT64)
```bash
gcc src/main.c -o Image-Viewer \
-I/ucrt64/include/SDL2 -L/ucrt64/lib \
-lmingw32 -lSDL2main -lSDL2 -lSDL2_image -mwindows
