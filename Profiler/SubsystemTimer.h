#pragma once

#include <string>

#include "../nclgl/GameTimer.h"
#include <iomanip>
#include <iostream>

using namespace std;

/*
  Time a subsystem's update function.
*/
class SubsystemTimer
{
public:
	SubsystemTimer();

	virtual ~SubsystemTimer() {}

	inline void StartTimer()
	{
		start = timer.GetMS();
	}

	inline void StopTimer()
	{
		finish		= timer.GetMS();
		timePassed	= (finish - start);
	}

	inline float TimePassed()
	{
		return timePassed;
	}

	void DisplayTimer();
	void DisplayTimer(string timername);

	float timePassed;
protected:
	float start;
	float finish;
	GameTimer timer;
};

