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

#define ERROR_FIRST 0
#define ERROR_NOTINITIALIZED ERROR_FIRST + 1 

class TinyClock
{
	public:

	TinyClock()
	{
		Initialized = false;
	};
	~TinyClock();

	/** 
	 *Initialize the TinyClock API
	 */
	static void Intialize()
	{
		GetInstance()->TotalTime = 0;
		GetInstance()->DeltaTime = 0;

#if defined(_WIN32) || defined(_WIN64)
		GetInstance()->Windows_Initialize();
#elif defined(__linux__)
		GetInstance()->Linux_Initialize();
#endif
		TinyClock::Initialized = true;
	}

	/** 
	 * update the clock using a fixed time step. e.g 60 
	 */
	static inline void UpdateClockFixed(double TimeStep)
	{
		if (TinyClock::Initialized)
		{
			GetInstance()->DeltaTime = 1.0 / TimeStep;
			GetInstance()->TotalTime += GetInstance()->DeltaTime;
		}

		else
		{
			PrintErrorMessage(ERROR_NOTINITIALIZED);
		}
	}

	/** 
	 *update the clock using adaptive CPU clocking
	 */
	static inline void UpdateClockAdaptive()
	{
		if (TinyClock::Initialized)
		{
			double NewTime = TinyClock::GetTime();
			GetInstance()->DeltaTime = (NewTime - GetInstance()->TotalTime);
			GetInstance()->TotalTime = NewTime;
		}
		else
		{
			PrintErrorMessage(ERROR_NOTINITIALIZED);
		}
	}

	/**
	 *get the total amount of time TinyClock has been running in milliseconds
	 */
	static inline double GetTotalTime()
	{
		if (TinyClock::Initialized)
		{
			return GetInstance()->TotalTime;
		}
		PrintErrorMessage(ERROR_NOTINITIALIZED);
		return NULL;
	}
	/** 
	 *get the current CPU delta time(time between CPU cycles in milliseconds)
	 */
	static inline double GetDeltaTime()
	{
		if (TinyClock::Initialized)
		{
			return GetInstance()->DeltaTime;
		}
		PrintErrorMessage(ERROR_NOTINITIALIZED);
		return NULL;
	}

	private:

	/**
	 *get the Time of the computer
	 */
	static double GetTime()
	{
		if (TinyClock::Initialized)
		{
#if defined(_WIN32)
			return Windows_GetTime();
#elif defined(__linux__)
			return Linux_GetTime();
#endif
		}
		PrintErrorMessage(ERROR_NOTINITIALIZED);
		return NULL;
	}

	/** 
	 *get the amount of time the computer has been running
	 */
	static double GetRawTime()
	{
#if defined(_WIN32) || defined(_WIN64)
			return Windows_GetRawTime();
#elif defined(__linux__) 
			return Linux_GetRawTime();
#endif
	}

	/** 
	 *print the error message corresponding to the given error number
	 */
	static void PrintErrorMessage(unsigned int ErrorNumber)
	{
		switch (ErrorNumber)
		{
			case ERROR_NOTINITIALIZED:
			{
				printf("Error: TinyClock needs to be initialized first \n");
				break;
			}

			default:
			{
				break;
			}
		}
	}

	/** 
	 *return an instance of the TinyClock class
	 */
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
	/**
	 *Initialize tinyClock on the Windows platform
	 */
	static void Windows_Initialize()
	{
		unsigned __int64 Frequency;

		//the frequency of the performance counter in seconds
		if (QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency))
		{
			/*
			* if QueryPerformanceFrequency does not return 0 then your computer supports
			* high resolution Time steps.
			*/
			GetInstance()->SupportsHighRes = true;
			GetInstance()->Resolution = 1.0 / (double)Frequency;
		}

		else
		{
			GetInstance()->SupportsHighRes = false;
			GetInstance()->Resolution = RESOLUTION_LOW;
		}

		//base time is your computers time when tinyWindow initializes
		GetInstance()->BaseTime = Windows_GetRawTime();
	}

	/**
	 *get the amount of time since the system was turned on
	 */
	static unsigned __int64 Windows_GetRawTime()
	{
			if (GetInstance()->SupportsHighRes)
			{
				unsigned __int64 Time = 0;
				//the current value of the performance counter in counts for high resolution time counts
				QueryPerformanceCounter((LARGE_INTEGER*)&Time);
				return Time;
			}

			//the amount of milliseconds since the system was started
			return (unsigned __int64)GetTickCount();
	}

	/*
	 *gets the current raw time of the computer and subtracts the value using the raw 
	 *time that was collected when TinyClock was Initialized. then multiply that value
	 *by the current Resolution.
	 */
	static double Windows_GetTime()
	{
			return (double)(Windows_GetRawTime() - GetInstance()->BaseTime) * GetInstance()->Resolution;
	}

#elif defined(__linux__)
	/*
	 *Initialize TinyClock on the Linux platform
	 */
	static void Linux_Initialize()
	{
		GetInstance()->MonoticSupported = false;
		
		//If monotonic is supported. Monotonic time since an unspecified starting point 
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

	/**
	 *get the amount of time since the system was turned on in milliseconds
	 */
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

	/**
	 *gets the current raw time of the computer and subtracts the value using the raw
	 *time that was collected when TinyClock was Initialized. then multiply that value
	 *by the current Resolution.
	 */
	static double Linux_GetTime()
	{
		return (double)(GetRawTime() - GetInstance()->BaseTime) * GetInstance()->Resolution;
	}
#endif

	static TinyClock* Instance;/**<A static instance of TinyClock*/

	double TotalTime; /**<the total amount of time since TinyClock was initialized*/
	double DeltaTime; /**<the delta time. The amount of time between CPU cycles*/
	double Resolution; /**<the resolution of the time step. (seconds, milliseconds, etc.)*/
	double BaseTime; /**<the system time since TinyClock was initialize*/

#if defined(_WIN32) || defined(_WIN64)
	bool SupportsHighRes; /**<whether high resolution time step is supported*/
#elif defined(__linux__)
	bool MonoticSupported; /**<Is monotonic supported*/
#endif

	static bool Initialized; /**<whether tinyWindow has been initialized*/
};

TinyClock* TinyClock::Instance = nullptr;
bool TinyClock::Initialized = false;
#endif
