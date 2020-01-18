#include "Slave.h"

// ------------------------------------
Slave::Slave()
	{
	}

ShomCanBus Slave::CanBus = ShomCanBus();
Unit    Slave::Timer = Unit("Timer", UT_TIMER); 
byte    _pinOutCount = 0;
byte    _pinOutArray[MAX_UNIT_NUMBER];
	
// ------------------------------------
static int Slave::DoCmd()
	{
	int res = -1;    // error by default
	unsigned int id = Slave::CanBus.GetMsgId();
	CanBusCmd cmd = Slave::CanBus.GetDataByte(DATA_CMD);
	byte pin = Slave::CanBus.GetDataByte(DATA_PIN);
	byte data = Slave::CanBus.GetDataByte(DATA_VALUE);
	if (cmd == CANBUS_READ || cmd == CANBUS_WRITE || cmd == CANBUS_MODE || cmd == CANBUS_RESET || cmd == CANBUS_NOPE)
	    {
		if( pin > 0 && pin < 54)
			{
			res = 0;
			if(cmd == CANBUS_WRITE)
				{
				if(data == 0 || data == 1)
					digitalWrite(pin, data);
				else
					Slave::CanBus.SetErrState(SL_ERR601, "wrong data = " + String(data)); 
				}
			else if(cmd == CANBUS_READ)
				{
				res = digitalRead(pin);
				id = Slave::CanBus.SendCmd(id, CANBUS_RESPONSE, pin, res);
				Slave::CanBus.LogData("after send");
				}
			else if (cmd == CANBUS_MODE)
				{
				if(data == INPUT || data == INPUT_PULLUP || data == OUTPUT)
                    {
                    if (data == OUTPUT)
                        {
                        _pinOutArray[_pinOutCount] = pin;
                       _pinOutCount++;
                        }
                        
					pinMode(pin, data);
                    }
				else
					Slave::CanBus.SetErrState(SL_ERR602, "wrong pin mode = " + String(data)); 
				}
			else if (cmd == CANBUS_RESET)
				{
				id = Slave::CanBus.SendCmd(id, CANBUS_RESPONSE, 0, 0); // send zeros
				}
			else if (cmd == CANBUS_NOPE)
				{
				id = Slave::CanBus.SendCmd(id, CANBUS_NOPE, 0, 0); // send zeros
				}
			}
		else 
			Slave::CanBus.SetErrState(SL_ERR603, "wrong pin = " + String(data)); 
		}
	else 
		Slave::CanBus.SetErrState(SL_ERR604, "wrong cmd = " + String(data)); 

    Timer.Time(TA_FIX);
	return res;	
	}

// ------------------------------------
static void Slave::CheckConnection()
  {
//  Slave::CanBus.Log("Slave::CheckConnection()");
//  Slave::CanBus.Log(String(Timer.Time(TA_PERIOD)));
  if (Timer.Time(TA_PERIOD) > CANBUS_TIMEOUT)
    {
    Slave::CanBus.Log("Connection fault!");
    for(int i = 0; i < _pinOutCount; i++)
    	{
		digitalWrite(_pinOutArray[i], DEFAULTSTATE);
	    Slave::CanBus.Log("reset pin=" + String(i));
    	}
    }
  }

