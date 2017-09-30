#pragma once

#include "FramerateCounter.h"
#include "../Game/Subsystems/Subsystem.h"
#include "../nclgl/Text.h"
#include <map>

class Window;
class Renderer;

/*
  Handles the framerate counter, memory watcher
  and subsystem timers in one update function.
*/
class Profiler : public Subsystem
{
public:
	Profiler(Renderer* ren, Window* win, int numTimers);
	Profiler()
	{}

	~Profiler()
	{}

	void Update(const float& deltatime) override;

	//Name is used when displaying the information
	void AddSubSystemTimer(string name, SubsystemTimer* t)
	{
		if (numAdded == numTimers)
		{
			throw new exception("Reached max timers");
		}
		else if (name.empty())
		{
			throw new exception("Timer name cannot be empty");
		}
		else
		{
			timers.insert({ name, t });
			numAdded++;
		}
	}

private:
	void UpdateProfiling();
	void RenderToScreen();

	void RenderMemory() const;
	void RenderFPSCounter();
	void RenderTimers();

	int	numTimers;
	int	numAdded = 0;
	bool renderingEnabled = false;

	Window*			 window;
	Renderer*		 renderer;
	FramerateCounter fpsCounter;
	map<string, SubsystemTimer*> timers;
};

