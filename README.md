# Advancing Front Triangulation

This project implements advancing front triangulation, also known as marching triangles, over a signed distance field. The algorithm works by finding a surface and then placing triangles as the surface traversal progresses.

It can be used to triangulate large terrains around a camera/player as well as static objects. I originally wrote the code in Java around 2014 for a personal game that was never released.

Since there are not many available triangulation libraries, I took some time to port it to C++ and open-source it.

It uses GLAD and GLFW and has been tested on Windows, macOS, and Linux (Ubuntu).

## Installation

TODO

## Contributions

Feel free to contribute, I'm not sure how much time I have but please reach out to me with any questions.

## Examples

Running this project will triangulate a few example objects and a planet, as shown in the images below. You can move around using the keyboard (`WASD` + arrow keys).

### A sphere and cube
<img src="examples/cube_sphere.png" width="480" alt="A cube and a sphere">

### A Sphere minus a cuboid
<img src="examples/sphere_hole.png" width="480" alt="A sphere perforated with a cube">

### The union of a sphere and cube
<img src="examples/sphere_union_cube.png" width="480" alt="The union of a sphere and cube">

### A planet with mountains
<img src="examples/planet.png" width="480" alt="A planet with mountains">

## Images/Video from the old Java version

Uses the same algorithm but with more color and complex terrains and objects. Everything in the images is triangulated from SDFs (flowers, trees, terrain, skybox, etc.).

<img src="examples/different_materials.png" width="640" alt="A planet with mountains and sea">

### Flower and objects
<img src="examples/objects.png" width="640" alt="Flowers, trees and rocks">

### Complex terrain
<img src="examples/complex_terrain.png" width="640" alt="Complex terrain, overhang, caves and holes">

### Early video illustrating the marching triangles

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/Vfwo_p4jffM/0.jpg)](https://www.youtube.com/watch?v=Vfwo_p4jffM)
