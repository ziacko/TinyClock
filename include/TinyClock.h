#ifndef TINYCLOCK_H_
#define TINYCLOCK_H_

#include <time.h>
#include <stdio.h>
#include <math.h>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#elif defined(__linux__)
#include <sys/time.h>
#include <stdint.h>
#endif
#if defined(_WIN32)
#define RESOLUTION_LOW 000.1
#elif defined(__linux__)
#define RESOLUTION_FLOAT_LOW 1e-6
#define RESOLUTION_FLOAT_HIGH 1e-9
#define RESOLUTION_HIGH 1e9
#define RESOLUTION_LOW 1e6
#endif
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
		GetInstance()->Linux_Initialize();
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
		return Linux_GetTime();
#endif
	}

	static double GetRawTime()
	{
#if defined(_WIN32) || defined(_WIN64)
		return Windows_GetRawTime();
#elif defined(__linux__) 
		return Linux_GetRawTime();
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
		}

		else
		{
			GetInstance()->SupportsHighRes = false;
			GetInstance()->Resolution = RESOLUTION_LOW;
		}

		GetInstance()->BaseTime = Windows_GetRawTime();
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
	static void Linux_Initialize()
	{
		GetInstance()->MonoticSupported = false;
		
#if defined(CLOCK_MONOTONIC)
		struct timespec ts;

		if(!clock_gettime(CLOCK_MONOTONIC, &ts))
		{
			GetInstance()->MonoticSupported = true;
			GetInstance()->Resolution = RESOLUTION_FLOAT_HIGH;
		}
		else
#endif
		{
			GetInstance()->Resolution = RESOLUTION_FLOAT_LOW;
		}

		GetInstance()->BaseTime = GetRawTime();		
	}

	static uint64_t Linux_GetRawTime()
	{
#if defined(CLOCK_MONOTONIC)
		if(GetInstance()->MonoticSupported)
		{
			struct timespec ts;
			clock_gettime(CLOCK_MONOTONIC, &ts);
			return (uint64_t)ts.tv_sec * (uint64_t)RESOLUTION_HIGH + (uint64_t)ts.tv_nsec;
		}

		else
#endif
		{
			struct timeval TimeVal;
			gettimeofday(&TimeVal, 0);
			return (uint64_t)TimeVal.tv_sec * (uint64_t)RESOLUTION_HIGH + (uint64_t)TimeVal.tv_usec;
		}
	}

	static double Linux_GetTime()
	{
		return (double)(GetRawTime() - GetInstance()->BaseTime) * GetInstance()->Resolution;
	}
#endif

	static TinyClock* Instance;

	double TotalTime, DeltaTime, Resolution, BaseTime;

#if defined(_WIN32) || defined(_WIN64)
	bool SupportsHighRes;
#elif defined(__linux__)
	bool MonoticSupported;
#endif

	static bool Initialized;
};

TinyClock* TinyClock::Instance = nullptr;
bool TinyClock::Initialized = false;
#endif
