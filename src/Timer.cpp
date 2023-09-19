#include "Timer.h"

//**********************************
void Timer::setTimer(unsigned long dt) { 
	t = millis() + (dt == 0 ? dT : dt);
}
