#include "Profiler.h"

#include "../nclgl/Rendering/View/Window.h"
#include "../nclgl/Rendering/Renderer/Renderer.h"

#define FRAME_MIN 1
#define TEXT_SIZE 15.0f

Profiler::Profiler(Renderer* ren, Window* win, int numTimers)
{
	window			= win;
	renderer		= ren;
	fpsCounter		= FramerateCounter(window->GetTimer()->GetMS());

	//Upper bound of how many timers can be added
	this->numTimers = numTimers;
}

void Profiler::Update(const float& deltatime)
{
	updateTimer.StartTimer();

	if (window->GetKeyboard()->KeyTriggered(KEYBOARD_P))
	{
		renderingEnabled = !renderingEnabled;
	}

	UpdateProfiling();
	if (renderingEnabled)
	{
		RenderToScreen();
	}
	else
	{
		updateTimer.StopTimer();
	}

	//Timer is stopped in the render function if that is enabled.
	//So it can time itself, with a minimal loss of accuracy.
}

void Profiler::UpdateProfiling() 
{
	++fpsCounter.frames;

	fpsCounter.CalculateFPS(window->GetTimer()->GetMS());
}

void Profiler::RenderToScreen()
{
	RenderFPSCounter();
	RenderTimers();
}

void Profiler::RenderFPSCounter()
{
	fpsCounter.CalculateFPS(window->GetTimer()->GetMS());
	renderer->AddProfilerText(Text(
		("FPS: " + to_string(fpsCounter.fps)),
		Vector3(0, 50, 0), TEXT_SIZE));
}

void Profiler::RenderTimers()
{
	float offset = 70.0f;
	for each(std::pair<string, SubsystemTimer*> timer in timers)
	{
		renderer->AddProfilerText(Text(
			(timer.first + ":" + std::to_string(timer.second->timePassed)),
			Vector3(0, offset, 0), TEXT_SIZE));
		offset += 20.0f;
	}

	/*
	Very slight loss in accuracy of the Profiler's own timer.
	Couldnt think of another way to display a timer without
	actually stopping the timer...
	*/
	updateTimer.StopTimer();
	renderer->AddProfilerText(Text(
		("Profiler:" + std::to_string(updateTimer.timePassed)),
		Vector3(0, offset, 0), TEXT_SIZE));
}