# Clustered Forward Lighting

## Description
* C++ OpenGL lighting technique using GPU computing.
* Suitable for large numbers of lights in a scene.
* Crytek Sponza scene is used to demonstrate.

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
* Real time shadows.
* Screen space ambient occlusion.
* Bloom.



## How To Use
*See Game/run.cpp*

*See Game/GLConfig*

### Adjusting The Algorithm
Global settings can be modified in GLConfig.h and GLConfig.cpp.
Any changes that are made to the grid and lights here must correspond to those specified in Shaders/Compute/configuration.glsl.
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

### Controls

* WASD = Move.
* C = Move down.
* Space = Move up.
* R = Recompile and link shaders.
* P = Display profiler.