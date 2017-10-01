#include "FramerateCounter.h"
#include <string.h>
#include <iostream>
#include <iomanip>

FramerateCounter::FramerateCounter(float starttime){
	frames		= 0;
	lastTime	= starttime;
	fps			= 0.0f;
}

void FramerateCounter::DisplayFPS() {
	std::cout << "FPS: " << fps;
}