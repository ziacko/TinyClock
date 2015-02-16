#ifndef TINYCLOCK_H_
#define TINYCLOCK_H_

#include <time.h>
#include <stdio.h>
#include <math.h>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

#define RESOLUTION_SECOND 1
#define RESOLUTION_MILLISECOND 1000.0
#define RESOLUTION_MICROSECOND 1000000.0

class TinyClock
{
	public:

	TinyClock()
	{
		Initialized = false;
	};
	~TinyClock();

	static void Intialize()
	{
		GetInstance()->TotalTime = 0;
		GetInstance()->DeltaTime = 0;

#if defined(_WIN32) || defined(_WIN64)
		GetInstance()->Windows_Initialize();
#elif defined(__linux__)

#endif
	}

	static inline void UpdateClockFixed(double TimeStep)
	{
		GetInstance()->DeltaTime = 1.0 / TimeStep;
		GetInstance()->TotalTime += GetInstance()->DeltaTime;
	}

	static inline void UpdateClockAdaptive()
	{
		double NewTime = TinyClock::GetTime();
		GetInstance()->DeltaTime = (NewTime - GetInstance()->TotalTime);
		GetInstance()->TotalTime = NewTime;
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

	static double GetTime()
	{
#if defined(_WIN32)
		return Windows_GetTime();
#elif defined(__linux__)

#endif
	}

	static inline TinyClock* GetInstance()
		{
			if (TinyClock::Initialized)
			{
				return Instance;
			}

			TinyClock::Instance = new TinyClock();
			TinyClock::Initialized = true;
			return Instance;
		}

#if defined(_WIN32)
	static void Windows_Initialize()
	{
		unsigned __int64 Frequency;

		if (QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency))
		{
			GetInstance()->SupportsHighRes = true;
			GetInstance()->Resolution = 1.0 / (double)Frequency;
			printf("%f\n", GetInstance()->Resolution);
		}

		else
		{
			GetInstance()->SupportsHighRes = false;
			GetInstance()->Resolution = 0.001;
		}

		GetInstance()->BaseTime = Windows_GetRawTime();
		//printf("%i \n", GetInstance()->BaseTime);
	}

	static unsigned __int64 Windows_GetRawTime()
	{
		if (GetInstance()->SupportsHighRes)
		{
			unsigned __int64 Time = 0;
			QueryPerformanceCounter((LARGE_INTEGER*) &Time);
			return Time;
		}

		return (unsigned __int64)GetTickCount();
	}

	static double Windows_GetTime()
	{
		return (double)(Windows_GetRawTime() - GetInstance()->BaseTime) * GetInstance()->Resolution;
	}
#elif defined(__linux__)
	static void Linux_Initialize(){}

	static unsigned int Linux_GetRawTime(){}

	static double Linux_GetTime();
#endif

	static TinyClock* Instance;

	double TotalTime, DeltaTime, Resolution, BaseTime;

	bool SupportsHighRes;
	static bool Initialized;
};

TinyClock* TinyClock::Instance = nullptr;
bool TinyClock::Initialized = false;

#endif
