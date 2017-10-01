#pragma once

#include "../Profiler/Timers/SubsystemTimer.h"

/*
  All subsystems of the engine should inherit from this.
  The timer can be started and stopped as appropriate 
  for each update function.
*/
class Subsystem
{
public:

	Subsystem()
	{
	}

	virtual ~Subsystem()
	{
	}

	virtual void Update(const float& deltatime = 0) = 0;

	//For profiling
	SubsystemTimer updateTimer;
};

