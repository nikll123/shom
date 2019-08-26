#ifndef Core_h
	#define Core_h
	
	#include "Arduino.h"

	#define UNIT_AUTOMAT   	 0
	#define UNIT_CONTACTOR	 1
	
	#define LOGIC_NORMAL     0
	#define LOGIC_INVERSE    1
	
    enum ConveyorState {NOTINIT, ON, OFF, FAULT, STARTING, STOPPING, UNKNOWN, ERROR};	
    
	#define PortMonitorLog 1

	#define TURN_STATE_UNKNOWN	99
	#define TURN_STATE_OFF		0
	#define TURN_STATE_ON		1
	#define TURN_STATE_NEXT		2
	#define TURN_STATE_STARTING	3
	#define TURN_STATE_STOPPING	4
	#define TURN_STATE_ERROR	100

   	#define TURN_ON_TIMEOUT 2000
   	#define TURN_OFF_TIMEOUT 1000

	
    class Core
	{
	public:
		Core();
		static void LogState(String string, uint8_t state);
		static String GetStateText(uint8_t state);
	};

    
#endif

