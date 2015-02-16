#include <stdio.h>
#include <TinyClock.h>
int main()
{
	TinyClock::Intialize();

	for (double Time = 0; Time < 10; Time = TinyClock::GetTotalTime())
	{		
		TinyClock::UpdateClockAdaptive();
		printf("%f \r", TinyClock::GetTotalTime());
	}

	return 0;
}
