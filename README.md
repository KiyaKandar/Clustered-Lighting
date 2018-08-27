# Clustered Forward Lighting

![Sponza](https://github.com/KiyaKandar/Clustered-Forward-Lighting/blob/master/sponza_pretty_pbr.png)

https://youtu.be/5odGp2NOhzw

## ABOUT
http://kiyavashkandar.com/clustered-forward-lighting

## Description
* C++ OpenGL lighting technique using GPU computing.
* Suitable for large numbers of lights in a scene.
* Crytek Sponza scene is used to demonstrate.

![Sponza](https://github.com/KiyaKandar/Clustered-Forward-Lighting/blob/master/sponza_pbr_1024_lights.png)

**WARNING: ASSIMP LOAD TIMES FOR SPONZA TAKE ~35 SECONDS**

Requires OpenGL 4.5 or above.

### Algorithm
1. Splits screen into a 3 dimensional grid.
2. Assigns a light to a tile if the light's sphere intersects with it.
3. Fragment shader in lighting pass calculates which tile it belongs to and applies all lights that collide with it to the fragment colour.

#### Culling Stage
Uses an additional culling stage.

* One screen space sized cube is used.
* Any lights that do not collide with it are not considered for the algorithm above.

### Other Graphical Features
* Real time shadows + soft shadows.
* Screen space ambient occlusion.
* Bloom.
* Motion blur
* PBR (on PBR branch)
* Transparency

![Sponza](https://github.com/KiyaKandar/Clustered-Forward-Lighting/blob/master/sponza_tiled_view.png)

## How To Use
*See Game/run.cpp*

*See Game/GLConfig*

### Adjusting The Algorithm
Global settings can be modified in GLConfig.h and GLConfig.cpp.
Any changes that are made to the grid and lights here are then written to Shaders/Compute/configuration.glsl on start-up (before shaders are compiled).
This is included/added in the required compute shaders and lighting pass.

GLConfig:
```cpp
	//...
	static const int NUM_X_AXIS_TILES = 10;
	static const int NUM_Y_AXIS_TILES = 10;
	static const int NUM_Z_AXIS_TILES = 10;
	static constexpr int NUM_TILES = NUM_X_AXIS_TILES * NUM_Y_AXIS_TILES * NUM_Z_AXIS_TILES;

	static const int NUM_LIGHTS = 100;
	//...
```

configuration.glsl
```cpp
	const int numTiles = 1000;
	const int numLights = 100;
	const vec3 tilesOnAxes = vec3(10, 10, 10);
```

Some preset scenes and configurations are provided to assist with performance analysis including:
* A more visually appealing scene.
* 100 lights.
* 512 lights.
* 1024 lights.
* 2048 lights.

To choose a preset, uncomment the macro in GLConfig.h
```cpp
#define DEMO_PRETTY
//#define DEMO_100_LIGHTS
//#define DEMO_512_LIGHTS
//#define DEMO_1024_LIGHTS
//#define DEMO_2048_LIGHTS
```

### Controls

* WASD = Move.
* C = Move down.
* Space = Move up.
* R = Recompile and link shaders.
* P = Display profiler.
* O (when profiler enabled) = Display light positions.
* J = Move lights every frame.
* T = Enabled tiled view. This displays the number of lights intersecting with each tile.
