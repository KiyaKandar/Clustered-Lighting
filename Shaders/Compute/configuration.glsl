//DO NOT CHANGE THIS FILE - THIS IS WRITTEN TO BY APPLICATION CODE ON START-UP
const int numLights = 2048;
const vec3 tilesOnAxes = vec3(16, 16, 16);
const int numTiles = int(tilesOnAxes.x * tilesOnAxes.y * tilesOnAxes.z);

int GetTileIndex(const int xIndex, const int yIndex, const int zIndex)
{
	return (int(tilesOnAxes.y) * int(tilesOnAxes.x) * zIndex) + (int(tilesOnAxes.x) * yIndex) + xIndex;
	// (3 * 3 * 1) + (3 * 2
}














































































