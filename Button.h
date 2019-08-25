#ifndef Button_h
	#define Button_h
	
	#include "Arduino.h"
	#include "Core.h"


	class Button
	{
	public:
	    //Button(String title, uint8_t pinButton);
	    Button();
		uint8_t GetState();
		uint8_t GetStateLog();
		String GetStateTxt();
		String GetStateTxtLog();
		String GetTitle();
		String GetLog();
		void InitPin(uint8_t pinButton, String title);

	protected:
		bool	_state;

	private:
		uint8_t _pinButton;
		String	_title;
	};
#endif

