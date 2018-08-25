const int numTiles = 27000;
const int numLights = 100;
const vec3 tilesOnAxes = vec3(30, 30, 30);

int GetTileIndex(const int xIndex, const int yIndex, const int zIndex)
{
	return xIndex + int(tilesOnAxes.x) * (yIndex + int(tilesOnAxes.y) * zIndex);
}