#include "Contactor.h"

// =========   CONSTRUCTORS   =========
Contactor::Contactor() : Unit("contactor", UT_CONTACTOR) 
{
	_state = CS_NOTINIT;
}

Contactor::Contactor(String title, uint8_t pinOut, uint8_t pinIn) : Contactor(title, pinOut, pinIn, 0) 
{
}

Contactor::Contactor(String title, uint8_t pinOut, uint8_t pinIn, uint8_t pinLed) : Unit(title, UT_CONTACTOR)
{
	_timeOutOn = TURN_ON_TIMEOUT;
	_timeOutOff = TURN_OFF_TIMEOUT;
	KeyIn = PinIn(title + "_KeyIn", pinIn);
	KeyOut = PinOut(title + "_KeyOut", pinOut);
	LedIndicator = Led(title + "_led", pinLed);
	
	_state = CS_UNKNOWN;
}

void Contactor::Init()
	{
 	LogTextLn(_title + " Init");
	KeyIn.GetState();
	KeyOut.SetOff();
	KeyOut.GetState();
	_state = CS_OFF;
	LedIndicator.SetOff();
	GetState();
	}

// ------------------------------------
ContactorInfo Contactor::GetInfo()
	{
	UnitInfo ui = Unit::GetInfo();
	PinInInfo kii = KeyIn.GetInfo();
	PinOutInfo koi = KeyOut.GetInfo();
	LedInfo li = LedIndicator.GetInfo();
    return {ui.Title,
			ui.UnitType, 
			GetContactorStateText(_state), 
			kii.Pin, 
			koi.Pin, 
			li.Pin,
			_timeOutOn,
			_timeOutOff 
			}; 
	}
	
// ------------------------------------
void Contactor::LogInfo()
	{
	ContactorInfo ci = GetInfo();
	LogText(ci.Title + "; ");
	LogText(ci.UnitType + "; ");
	LogText(String(ci.PinOut) + "; ");
	LogText(String(ci.PinIn) + "; ");
	LogText(String(ci.PinLed) + "; ");
	LogText(ci.State + ".");
	LogLn();
	}

//------------------------------
String Contactor::GetContactorStateText(ContactorState state)
{
switch (state)
	{
	case CS_NOTINIT		: return "NOTINIT";
	case CS_ON			: return "ON";
	case CS_OFF			: return "OFF";
	case CS_STARTING	: return "STARTING";
	case CS_STOPPING	: return "STOPPING";
	case CS_UNKNOWN		: return "UNKNOWN";
	default			    : return "ContactorState error " + String(state);
	}
}

// ------------------------------------
ContactorState2 Contactor::GetState()
	{
	ContactorState2 cs2;
	if (_state != CS_NOTINIT)
		{
		cs2.Old = _state;
//LogTextLn(GetContactorStateText(cs2.Old));
		PinState stateKeyIn = (KeyIn.GetState()).New;
		PinState stateKeyOut = KeyOut.GetState(); 
		if(_state == CS_OFF)
			{
			if (stateKeyOut != KS_OFF) 		_state = CS_ERR1;
			else if (stateKeyIn != KS_OFF) 	_state = CS_ERR2;
			}
		else if(_state == CS_STARTING)
			{
			if(_millsCheck == 0)
				{
				KeyOut.SetOn();
				_millsCheck = millis();
				}
			else if (millis() - _millsCheck > _timeOutOn)
				_state = CS_ON;

			stateKeyOut = KeyOut.GetState(); 
			stateKeyIn = (KeyIn.GetState()).New;

			if (stateKeyOut != KS_ON) 		_state = CS_ERR3;
			else if (stateKeyIn != KS_ON) 	_state = CS_ERR4;

			}
		else if(_state == CS_STOPPING)
			{
			if(_millsCheck == 0)
				{
				_millsCheck = millis();
				}
			else if (millis() - _millsCheck > _timeOutOff)
				{
				KeyOut.SetOff();
				_state = CS_OFF;
				stateKeyOut = KeyOut.GetState(); 
				stateKeyIn = (KeyIn.GetState()).New;
				}

			if (_state == CS_OFF)
				{
				if (stateKeyOut != KS_OFF) 		_state = CS_ERR5;
				else if (stateKeyIn != KS_OFF) 	_state = CS_ERR6;
				
				if (!(stateKeyOut == KS_OFF && stateKeyIn == KS_OFF))
					_state = CS_ERR3;
				}
			else    // US_STOPPING
				{
				if (stateKeyOut != KS_ON) 		_state = CS_ERR7;
				else if (stateKeyIn != KS_ON) 	_state = CS_ERR8;
				}
			}
		else if(_state == CS_ON)
			{
			if (stateKeyOut != KS_ON) 		_state = CS_ERR9;
			else if (stateKeyIn != KS_ON) 	_state = CS_ERR10;
			}

		cs2.New = _state;
		IfChanged(cs2);
		}
	LedIndicator.Refresh();
	return cs2;
	}

// ------------------------------------
void Contactor::TurnOn()
	{
 	LogTextLn(_title + " TurnOn");
    _Turn(CS_STARTING);
	}

// ------------------------------------
void Contactor::TurnOff()
	{
 	LogTextLn(_title + " TurnOff");
    _Turn(CS_STOPPING);
	}

void Contactor::_Turn(ContactorState csNew)
	{
	if (csNew == CS_STARTING || csNew == CS_STOPPING)
		{
		ContactorState csCurr = (GetState()).New;
		bool err = false; 
		switch (csNew)
			{
			case CS_STARTING :
				if (csCurr == CS_OFF)   // start!
					{
					_millsCheck = 0;
					_state = csNew;
					}
				break;
			case CS_STOPPING :
				if (csCurr == CS_ON || csCurr == CS_STARTING)
					{
					_millsCheck = 0;
					_state = csNew;
					}
				break;
			}
		ContactorState2 cs2(csCurr, csNew); 
		IfChanged(cs2);
		}
	else
		{
		 	LogTextLn("_Turn: wrong arg " + GetContactorStateText(csNew));
		}
	}
	

// ------------------------------------
/*void Contactor::TurnOffAlarm()
	{
	if (_uzelType == UT_CONTACTOR && _active)
		{
		//_millsCheck = 0;
		digitalWrite(_pinOut, 0);
		_state = US_OFF;
		}
	}*/

void Contactor::IfChanged(ContactorState2 cs2)
	{
	if(cs2.Old != cs2.New)
		{
		if (cs2.New == CS_ON)
			LedIndicator.SetOn();
		else if (cs2.New == CS_STARTING || cs2.New == CS_STOPPING)
			LedIndicator.SetBlink();
		else if (cs2.New >= CS_ERR)
			LedIndicator.SetBlinkFast();
		else 
			LedIndicator.SetOff();

		if (LOGLEVEL > LL_MIN) 
			{
			LogText(GetInfo().Title);
			LogText(" " + GetContactorStateText(cs2.Old));
			LogText(" -> " + GetContactorStateText(cs2.New));
			LogLn();
			}
		}
	}
	

/*void Contactor::LogKeysState()
	{
	if (LOGLEVEL > LL_NORMAL) 
		{
		KeyIn.LogState();
		KeyOut.LogState();
		}
	}*/
