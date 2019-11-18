#include "PinOut.h"
// ------------------------------------
PinOut::PinOut() : PinOut("Dummy PinOut", 0) 
	{
	}

PinOut::PinOut(String title, uint8_t pin) : Pin(title, pin, UT_PINOUT)
	{
	pinMode(pin, OUTPUT);
	Init();
	}

// ------------------------------------
void PinOut::Init()
	{
 	SetOff();
	}   

// ------------------------------------
void PinOut::SetOn()
	{
	SetOn(0);
	}

// ------------------------------------
void PinOut::SetOn(bool noLog)
	{
	_setState(KS_ON, noLog);
	}

// ------------------------------------
void PinOut::SetOff()
	{
	SetOff(0);
	}

// ------------------------------------
void PinOut::SetOff(bool noLog)
	{
	_setState(KS_OFF, noLog);
	}

// ------------------------------------
void PinOut::Inverse()
	{
	if (_state == KS_ON)
		SetOff();	
	else
		SetOn();
	}

// ------------------------------------
void PinOut::_setState(PinState state, bool noLog)
	{
	PinState2 ps2;
	ps2.Old = _state;
	digitalWrite(_pin, (state == KS_ON));
	_state = state; 
	ps2.New = _state;
	if (ps2.Changed() && !noLog) 
		{
		String str = GetPinStateText(ps2.Old) + " -> " + GetPinStateText(ps2.New); 
		Log(str);
		}
	}

// ------------------------------------
PinState PinOut::GetState()
	{
	return _state;
	}

// ------------------------------------
PinOutInfo PinOut::GetInfo()
	{
	PinInfo pi = Pin::GetInfo();
    return {pi.Title,
			pi.UnitType, 
			GetPinStateText(_state), 
			_pin
			}; 
	}

// ------------------------------------
void PinOut::LogInfo()
	{
	PinOutInfo pi = GetInfo();
	LogText(pi.Title + "; ");
	LogText(pi.UnitType + "; ");
	LogText(pi.State + "; ");
	LogText(String(_pin) + "; ");
	LogLn();
	}

// ------------------------------------
void PinOut::LogState()
	{
	PinOutInfo pi = GetInfo();
	LogText(pi.Title + "; ");
	LogText(pi.State + "; ");
	LogLn();
	}

