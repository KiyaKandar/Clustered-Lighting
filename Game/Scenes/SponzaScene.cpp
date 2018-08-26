#include "SponzaScene.h"

int SponzaScene::currentPositionIndex = 0;
bool SponzaScene::manual = true;
bool SponzaScene::moveLight = false;
vector<Vector2> SponzaScene::radii = vector<Vector2>();
Vector3 SponzaScene::worldLightPosition = Vector3(10, 1800, 200);

float SponzaScene::frameCounter = 0.0f;