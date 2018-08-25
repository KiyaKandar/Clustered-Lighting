const int numTiles = 8000;
const int numLights = 100;
const vec3 tilesOnAxes = vec3(20, 20, 20);

int GetTileIndex(const int xIndex, const int yIndex, const int zIndex)
{
	return xIndex + int(tilesOnAxes.x) * (yIndex + int(tilesOnAxes.y) * zIndex);
}