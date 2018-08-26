const int numLights = 1000;
const vec3 tilesOnAxes = vec3(16, 16, 16);
const int numTiles = int(tilesOnAxes.x * tilesOnAxes.y * tilesOnAxes.z);

int GetTileIndex(const int xIndex, const int yIndex, const int zIndex)
{
	return xIndex + int(tilesOnAxes.x) * (yIndex + int(tilesOnAxes.y) * zIndex);
}