# Mesh Viewer
## Simple Mesh Viewer Made with C++ using OpenGL & freeGlut

## Features
- Program can view .off files.
- User can freely rotate around object an zoom in and out using mouse.
- User can rotate, change size and place of the object using keyboard.
- There are 4 viewing modes: Shading, Vertex, Face and Edge mode.
- Default viewing mode is face mode (draws filled triangles with randomly filled color).
- All objects are properly resized and repositioned to fit screen before rendering.
- Objects are displayed using perspective viewing.

## Mouse Usage
- Left Click (Hold): Rotate around the object.
- Mouse wheel: Zoom in and out.

## Keyboard Shortcuts
- 'v': Switch to vertex displaying mode (draws vertices only).
- 'e': Switch to edge displaying mode (draws edges only).
- 'f': Switch to face displaying mode (draws filled triangles with randomly filled color).
- 'm': Switch to shading displaying mode (draws green object and applies shading to each face).
- 'n': Draw next object (circular).
- 'p': Draw previous object (circular).
- 's': Scale object to half size.
- 'S': Scale object to twice size.
- 'x': Move object +1 in x-plane.
- 'X': Move object -1 in x-plane.
- 'y': Move object +1 in y-plane.
- 'Y': Move object -1 in y-plane.
- 'z': Move object +1 in z-plane.
- 'Z': Move object -1 in z-plane.
- Right arrow key rotates object by 2 degrees in y-plane.
- Left arrow key rotates object by -2 degrees in y-plane.

## Running
To compile in linux
```
g++ MeshViewer.cpp -lGL -lGLU -lglut -o MeshViewer
```
To run ( any number of objects can be written after executable )
```
./MeshViewer <Object names to draw>
```
