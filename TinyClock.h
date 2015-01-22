#ifndef TINYCLOCK_H_
#define TINYCLOCK_H_

#include <time.h>
#include <stdio.h>
#include <math.h>

class TinyClock
{
	public:
	TinyClock(){};
	~TinyClock();

	static void Intialize()
	{
		GetInstance()->TotalTime = 0.0;
		GetInstance()->DeltaTime = 0.0;
		GetInstance()->PreviousTime = (double)clock() / CLOCKS_PER_SEC;
	}

	static inline void UpdateClockFixed(double TimeStep)
	{
		GetInstance()->DeltaTime = 1.0 / TimeStep;
		GetInstance()->TotalTime += GetInstance()->DeltaTime;
	}

	static inline void UpdateClockAdaptive()
	{
		double NewTime = (double)clock() / CLOCKS_PER_SEC;
		GetInstance()->DeltaTime = (NewTime - GetInstance()->PreviousTime);
		GetInstance()->TotalTime += GetInstance()->DeltaTime;
		GetInstance()->PreviousTime = NewTime;
	}

	static inline double GetTotalTime()
	{
		return GetInstance()->TotalTime;
	}

	static inline double GetDeltaTime()
	{
		return GetInstance()->DeltaTime;
	}

	private:

	static inline TinyClock* GetInstance()
	{
		if(Instance == nullptr)
		{
			Instance = new TinyClock();
			return Instance;
		}
		return Instance;
	}

	static TinyClock* Instance;

	double TotalTime, DeltaTime, PreviousTime;
};

TinyClock* TinyClock::Instance = nullptr;

#endif
