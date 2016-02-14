#include <stdio.h>
#include <TinyClock.h>
int main()
{
	tinyClock::Intialize();

	for (double Time = 0; Time < 10; Time = tinyClock::GetTotalTime())
	{		
		tinyClock::UpdateClockAdaptive();
		printf("%f \r", tinyClock::GetTotalTime());
	}

	return 0;
}
