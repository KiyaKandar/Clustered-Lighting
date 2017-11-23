#include "RobotScene.h"

float RobotScene::counter = 0.0f;
int RobotScene::explosionCounter = 0;
float RobotScene::waveCounter = 0.0f;
bool RobotScene::triggered = false;
bool RobotScene::destroyEnemy = false;
Vector3 RobotScene::centre = Vector3(0, 0, 0);