#ifndef TINYCLOCK_H
#define TINYCLOCK_H

#include <time.h>
#include <stdio.h>
#include <math.h>

#if defined ( _WIN32 ) || defined ( _WIN64 )
#include <Windows.h>
#elif defined ( __linux__ )
#include <sys/time.h>
#include <stdint.h>
#endif
#if defined ( _WIN32 )
#define TINYCLOCK_RESOLUTION_LOW 000.1
#elif defined ( __linux__ )
#define TINYCLOCK_RESOLUTION_FLOAT_LOW 1e-6
#define TINYCLOCK_RESOLUTION_FLOAT_HIGH 1e-9
#define TINYCLOCK_RESOLUTION_HIGH 1e9
#define TINYCLOCK_RESOLUTION_LOW 1e6
#endif
#define TINYCLOCK_RESOLUTION_MILLISECOND 1000.0
#define TINYCLOCK_RESOLUTION_MICROSECOND 1000000.0

#define TINYCLOCK_ERROR_FIRST 0
#define TINYCLOCK_ERROR_NOT_INITIALIZED TINYCLOCK_ERROR_FIRST + 1 

class tinyClock
{
	public:

	tinyClock ( void )
	{
		initialized = false;
	}
	~tinyClock ( void );

	/** 
	 *Initialize the TinyClock API
	 */
	static void Intialize ( void )
	{
		if ( !tinyClock::initialized )
		{
			GetInstance()->totalTime = 0;
			GetInstance()->deltaTime = 0;

#if defined ( _WIN32 ) || defined ( _WIN64 )
			GetInstance()->Windows_Initialize ( );
#elif defined ( __linux__ )
			GetInstance()->Linux_Initialize ( );
#endif
			tinyClock::initialized = true;
		}
	}

	/** 
	 * update the clock using a fixed time step. e.g 60 
	 */
	static inline void UpdateClockFixed ( double TimeStep )
	{
		if ( tinyClock::initialized )
		{
			GetInstance()->deltaTime = 1.0 / TimeStep;
			GetInstance()->totalTime += GetInstance()->deltaTime;
		}

		else
		{
			PrintErrorMessage( TINYCLOCK_ERROR_NOT_INITIALIZED );
		}
	}

	/** 
	 *update the clock using adaptive CPU clocking
	 */
	static inline void UpdateClockAdaptive( void )
	{
		if ( tinyClock::initialized )
		{
			double NewTime = tinyClock::GetTime();
			GetInstance()->deltaTime = ( NewTime - GetInstance()->totalTime );
			GetInstance()->totalTime = NewTime;
		}
		else
		{
			PrintErrorMessage( TINYCLOCK_ERROR_NOT_INITIALIZED );
		}
	}

	/**
	 *get the total amount of time TinyClock has been running in milliseconds
	 */
	static inline double GetTotalTime( void )
	{
		if ( tinyClock::initialized )
		{
			return GetInstance()->totalTime;
		}
		PrintErrorMessage( TINYCLOCK_ERROR_NOT_INITIALIZED );
		return NULL;
	}
	/** 
	 *get the current CPU delta time ( time between CPU cycles in milliseconds )
	 */
	static inline double GetDeltaTime( void )
	{
		if ( tinyClock::initialized )
		{
			return GetInstance()->deltaTime;
		}
		PrintErrorMessage( TINYCLOCK_ERROR_NOT_INITIALIZED );
		return NULL;
	}

	private:

	/**
	 *get the Time of the computer
	 */
	static double GetTime( void )
	{
		if ( tinyClock::initialized )
		{
#if defined ( _WIN32 )
			return Windows_GetTime();
#elif defined ( __linux__ )
			return Linux_GetTime();
#endif
		}
		PrintErrorMessage( TINYCLOCK_ERROR_NOT_INITIALIZED );
		return NULL;
	}

	/** 
	 *get the amount of time the computer has been running
	 */
	static double GetRawTime( void )
	{
#if defined ( _WIN32 ) || defined ( _WIN64 )
			return ( double )Windows_GetRawTime();
#elif defined ( __linux__ ) 
			return ( double )Linux_GetRawTime();
#endif
	}

	/** 
	 *print the error message corresponding to the given error number
	 */
	static void PrintErrorMessage( unsigned int ErrorNumber )
	{
		switch ( ErrorNumber )
		{
			case TINYCLOCK_ERROR_NOT_INITIALIZED:
			{
				printf( "Error: TinyClock needs to be initialized first \n" );
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
	static inline tinyClock* GetInstance( void )
		{
			if ( tinyClock::initialized )
			{
				return instance;
			}

			tinyClock::instance = new tinyClock();
			tinyClock::initialized = true;
			return instance;
		}

#if defined ( _WIN32 )
	/**
	 *Initialize tinyClock on the Windows platform
	 */
	static void Windows_Initialize( void )
	{
		unsigned __int64 Frequency;

		//the frequency of the performance counter in seconds
		if ( QueryPerformanceFrequency( ( LARGE_INTEGER* )&Frequency ) )
		{
			/*
			* if QueryPerformanceFrequency does not return 0 then your computer supports
			* high resolution Time steps.
			*/
			GetInstance()->supportsHighRes = true;
			GetInstance()->timeResolution = 1.0 / ( double )Frequency;
		}

		else
		{
			GetInstance()->supportsHighRes = false;
			GetInstance()->timeResolution = TINYCLOCK_RESOLUTION_LOW;
		}

		//base time is your computers time when tinyWindow initializes
		GetInstance()->baseTime = ( double )Windows_GetRawTime();
	}

	/**
	 *get the amount of time since the system was turned on
	 */
	static unsigned __int64 Windows_GetRawTime( void )
	{
			if ( GetInstance()->supportsHighRes )
			{
				unsigned __int64 Time = 0;
				//the current value of the performance counter in counts for high resolution time counts
				QueryPerformanceCounter( ( LARGE_INTEGER* )&Time );
				return Time;
			}

			//the amount of milliseconds since the system was started
			return ( unsigned __int64 )GetTickCount();
	}

	/*
	 *gets the current raw time of the computer and subtracts the value using the raw 
	 *time that was collected when TinyClock was Initialized. then multiply that value
	 *by the current Resolution.
	 */
	static double Windows_GetTime( void )
	{
			return ( double ) ( Windows_GetRawTime() - GetInstance()->baseTime ) * GetInstance()->timeResolution;
	}

#elif defined ( __linux__ )
	/*
	 *Initialize TinyClock on the Linux platform
	 */
	static void Linux_Initialize( void )
	{
		GetInstance()->monoticSupported = false;
		
		//If monotonic is supported. Monotonic time since an unspecified starting point 
#if defined ( CLOCK_MONOTONIC )
		struct timespec ts;

		if ( !clock_gettime( CLOCK_MONOTONIC, &ts ) )
		{
			GetInstance()->monoticSupported = true;
			GetInstance()->timeResolution = TINYCLOCK_RESOLUTION_FLOAT_HIGH;
		}
		else
#endif
		{
			GetInstance()->timeResolution = TINYCLOCK_RESOLUTION_FLOAT_LOW;
		}

		GetInstance()->baseTime = GetRawTime();
	}

	/**
	 *get the amount of time since the system was turned on in milliseconds
	 */
	static uint64_t Linux_GetRawTime( void )
	{
#if defined ( CLOCK_MONOTONIC )
		if ( GetInstance()->monoticSupported )
		{
			struct timespec ts;
			clock_gettime ( CLOCK_MONOTONIC, &ts );
			return ( uint64_t )ts.tv_sec * ( uint64_t )TINYCLOCK_RESOLUTION_HIGH + ( uint64_t )ts.tv_nsec;
		}

		else
#endif
		{
			struct timeval TimeVal;
			gettimeofday ( &TimeVal, 0 );
			return ( uint64_t )TimeVal.tv_sec * ( uint64_t )TINYCLOCK_RESOLUTION_HIGH + ( uint64_t )TimeVal.tv_usec;
		}
	}

	/**
	 *gets the current raw time of the computer and subtracts the value using the raw
	 *time that was collected when TinyClock was Initialized. then multiply that value
	 *by the current Resolution.
	 */
	static double Linux_GetTime()
	{
		return ( double ) ( GetRawTime() - GetInstance()->baseTime ) * GetInstance()->timeResolution;
	}
#endif

	static tinyClock*		instance;				/**< A static instance of TinyClock */

	double					totalTime;				/**< The total amount of time since TinyClock was initialized */
	double					deltaTime;				/**< The delta time. The amount of time between CPU cycles */
	double					timeResolution;			/**< The resolution of the time step. ( seconds, milliseconds, etc. ) */
	double					baseTime;				/**< The system time since TinyClock was initialize */

#if defined ( _WIN32 ) || defined ( _WIN64 )
	bool					supportsHighRes;		/**< Whether high resolution time step is supported */
#elif defined ( __linux__ )
	bool					monoticSupported;		/**< Is monotonic supported */
#endif

	static bool				initialized;			/**< Whether tinyWindow has been initialized */
};

tinyClock* tinyClock::instance = nullptr;
bool tinyClock::initialized = false;
#endif
