#include "SubsystemTimer.h"

SubsystemTimer::SubsystemTimer()
{
	start	= timer.GetMS();
	finish	= timer.GetMS();
}

void SubsystemTimer::DisplayTimer() 
{
	cout << fixed << setprecision (2) << "Time: " << timePassed;
}

void SubsystemTimer::DisplayTimer(string timername)
{
	cout << timername + ": " << setprecision(2) << fixed << timePassed;
}