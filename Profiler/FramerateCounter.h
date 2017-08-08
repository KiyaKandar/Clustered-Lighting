#pragma once

#define SECONDS 1000
#define MIN_FRAMES 4
#define MIN_TIME_PASSED 0.25f

class FramerateCounter
{
public:
	FramerateCounter(float starttime = 0);

	virtual ~FramerateCounter() {}

	inline void CalculateFPS(float time)
	{
		//Only update it periodically to stop a flickering number.
		//After a set amount of time and number of frames.
		if (time - lastTime > MIN_TIME_PASSED && frames > MIN_FRAMES) {
			fps = ((float)frames / (time - lastTime)) * SECONDS;
			lastTime = time;
			frames = 0;
		}
		//else just dont change it...
	}

	void DisplayFPS();

	int		frames;
	float	lastTime;
	float	fps;
};

