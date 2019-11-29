#include "Pin.h"


// ------------------------------------
Pin::Pin() : Pin("DummyPin", 0, UT_PIN) 
{
}

Pin::Pin(String title, uint8_t pin, UnitType ut) : Unit(title, ut)
{
	_logLevel = LL_NORMAL;
	_pin = pin;
	_state = KS_NONE;
}

ShomCanBus Pin::CanBus = ShomCanBus(); 

// ------------------------------------
bool Pin::IsHigh()
	{
	return ShomPinRead().Value; 
	}

// ------------------------------------
bool Pin::IsLow()
	{
 	return !IsHigh();
	}

// ------------------------------------
PinState Pin::GetState()
{
	return _state;
}

// ------------------------------------
void Pin::SetState(PinState state, bool noLog)
	{
	PinState2 ps2;
	ps2.Old = _state;
	//digitalWrite(_pin, (state == KS_ON));
	ShomPinWrite(state == KS_ON);
	_state = state; 
	ps2.New = _state;
	if (ps2.Changed() && !noLog) 
		LogStates(ps2);
	}
// ------------------------------------
void Pin::LogStates(PinState2 ps2)
	{
	String str  = String(_pin) + "; ";
	str = str + GetPinStateText(ps2.Old) + " -> " + GetPinStateText(ps2.New); 
	Log(str);
	}

// ------------------------------------
PinInfo Pin::GetInfo()
	{
	UnitInfo ui = Unit::GetInfo();
    return {ui.Title,
			ui.UnitType,
			GetPinModeText(), 
			GetPinStateText(_state), 
			_pin}; 
	}

//------------------------------
String Pin::GetPinStateText(PinState instate)
{
switch (instate)
	{
	case KS_NONE		: return "NONE";
	case KS_ON			: return "ON";
	case KS_OFF			: return "OFF";
	default			    : return "GetPinStateText: unknown-" + String(instate);
	}
}

//------------------------------
PinRespCode Pin::ShomPinRead()
	{
	PinRespCode res;
	if (_pin < 100)
		{
		res.Value = digitalRead(_pin);
		}
	else
		{
		byte pin = _pin - 100; 
		Pin::CanBus.ResetData();
		Pin::CanBus.SetDataByte(0, CANBUS_READ);
		Pin::CanBus.SetDataByte(1, pin);
		//Pin::CanBus.LogData();
		Pin::CanBus.Send();
		bool received = false;
		for (int i=0; i < RESPONSE_TRY_CNT; i++)
			{
			if(!received)
				{
				delay(RESPONSE_DELAY);
				byte len = Pin::CanBus.Receive();
				received = len > 0;
				if(received)
					{
					CanBusCmd cmd = Pin::CanBus.GetDataByte(0);
					if(CANBUS_RESPONSE == cmd)
						{
						byte pin = Pin::CanBus.GetDataByte(1);
						if(pin == pin)
							{
							byte data = Pin::CanBus.GetDataByte(2);
							if(data == 0 || data == 1)
								res.Value = data; 
							else
								res.RespCode = -5;
							}
						else
							res.RespCode  = -6;
						}
					else
						res.RespCode = -7;
					}
				}
			}
		if(!received)
			{
			res.RespCode = -8;
			}	
		}
	if(res.Value == true)
		_state = KS_ON;
	else
		_state = KS_OFF;
		
	if (res.RespCode < 0)
		LogErr(res.RespCode);

	return res;
	}

//------------------------------
void Pin::ShomPinWrite(bool val)
	{
	//Log("ShomPinWrite");
	if(val == true)
		_state = KS_ON;
	else
		_state = KS_OFF;
		
	if (_pin < 100)
		{
		digitalWrite(_pin, val);
		}
	else
		{
		Pin::CanBus.ResetData();
		Pin::CanBus.SetDataByte(0, CANBUS_WRITE);
		Pin::CanBus.SetDataByte(1, _pin - 100);
		Pin::CanBus.SetDataByte(2, val);
		//Pin::CanBus.LogData();
		Pin::CanBus.Send();
		}
	}

//------------------------------
void Pin::ShomPinMode(byte pinmode)
	{
	Log("ShomPinMode " + GetPinModeText());
	_pinmode = pinmode;
	if (_pin < 100)
		{
		pinMode(_pin, _pinmode);
		}
	else
		{
		Pin::CanBus.ResetData();
		Pin::CanBus.SetDataByte(0, CANBUS_MODE);
		Pin::CanBus.SetDataByte(1, _pin - 100);
		Pin::CanBus.SetDataByte(2, _pinmode);
		Pin::CanBus.Send();
		}
	}

//------------------------------
void Pin::LogInfo()
	{
	PinInfo	pi = GetInfo();
	String str = pi.UnitType + ", " + pi.PinMode + ", " + pi.State + ", " + String(pi.Pin);
	Log(str);   
  	}

//------------------------------
String Pin::GetPinModeText()
	{
	if(_pinmode == INPUT_PULLUP)
		return "INPUT_PULLUP";
	else if(_pinmode == INPUT)
		return "INPUT";
	else if(_pinmode == OUTPUT)
		return "OUTPUT";
	else
		return "Unknown";
	}
