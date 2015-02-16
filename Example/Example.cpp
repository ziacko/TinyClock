#include <stdio.h>
#include <TinyClock.h>
int main()
{
	TinyClock::Intialize();

	for (double Time = 0; Time < 300; Time = TinyClock::GetTotalTime())
	{		
		TinyClock::UpdateClockFixed(30);
		printf("%f \r", TinyClock::GetTotalTime());
	}

	return 0;
}