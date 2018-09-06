#include "Profiler.h"

#include "../nclgl/Rendering/View/Window.h"
#include "../nclgl/Rendering/Renderer/Renderer.h"
#include "../Game/Scenes/SponzaScene.h"

#include <iostream>
#include <fstream>
#include <ctime>

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

	if (window->GetKeyboard()->KeyTriggered(KEYBOARD_L))
	{
		loggingEnabled = !loggingEnabled;
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

void Profiler::SaveToFile()
{
	std::string fileName("../X" + std::to_string(GLConfig::NUM_X_AXIS_CLUSTERS)
		+ "Y" + std::to_string(GLConfig::NUM_Y_AXIS_CLUSTERS)
		+ "Z" + std::to_string(GLConfig::NUM_Z_AXIS_CLUSTERS)
		+ "L" + std::to_string(GLConfig::NUM_LIGHTS) + ".txt");

	std::ofstream outfile(fileName.c_str());

	outfile << "--------------Configuration--------------" << std::endl;
	outfile << "Resolution: " + std::to_string(GLConfig::RESOLUTION.x) + "x" + std::to_string(GLConfig::RESOLUTION.y) << std::endl;
	outfile << "X Clusters: " + std::to_string(GLConfig::NUM_X_AXIS_CLUSTERS) << std::endl;
	outfile << "Y Clusters: " + std::to_string(GLConfig::NUM_Y_AXIS_CLUSTERS) << std::endl;
	outfile << "Z Clusters: " + std::to_string(GLConfig::NUM_Z_AXIS_CLUSTERS) << std::endl;
	outfile << std::endl;

	outfile << "Num lights: " + std::to_string(GLConfig::NUM_LIGHTS) << std::endl;
	outfile << "Data Prep Work Group X: " + std::to_string(SponzaScene::workGroups.x) << std::endl;
	outfile << "Data Prep Work Group Y: " + std::to_string(SponzaScene::workGroups.y) << std::endl;
	outfile << "Data Prep Work Group Z: " + std::to_string(SponzaScene::workGroups.z) << std::endl;
	outfile << std::endl;

	outfile << "--------------Performance--------------" << std::endl;
	outfile << "Min FPS: " + std::to_string(minFPS) << std::endl;
	outfile << "Max FPS: " + std::to_string(maxFPS) << std::endl;
	outfile << "Avg FPS: " + std::to_string(frameRateTotal / float(frameCount)) << std::endl;

	outfile.close();
}

void Profiler::UpdateProfiling()
{
	++fpsCounter.frames;

	fpsCounter.CalculateFPS(window->GetTimer()->GetMS());

	if (loggingEnabled && fpsCounter.fps > 0.0f)
	{
		minFPS = min(minFPS, fpsCounter.fps);
		maxFPS = max(maxFPS, fpsCounter.fps);

		++frameCount;
		frameRateTotal += fpsCounter.fps;
	}
}

void Profiler::RenderToScreen()
{
	RenderFPSCounter();
	RenderTimers();

	if (loggingEnabled)
	{
		renderer->AddProfilerText(Text(
			("Recording stats for file log!"),
			Vector3(GLConfig::RESOLUTION.x - 400.0f, 50, 0), TEXT_SIZE));
	}
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