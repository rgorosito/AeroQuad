/*
  AeroQuad v3.1.x - August 2012
  www.AeroQuad.com
  Copyright (c) 2012 AeroQuad developers.  All rights reserved.
  An Open Source Arduino based multicopter.
 
  This program is free software: you can redistribute it and/or modify 
  it under the terms of the GNU General Public License as published by 
  the Free Software Foundation, either version 3 of the License, or 
  (at your option) any later version. 

  This program is distributed in the hope that it will be useful, 
  but WITHOUT ANY WARRANTY; without even the implied warranty of 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
  GNU General Public License for more details. 

  You should have received a copy of the GNU General Public License 
  along with this program. If not, see <http://www.gnu.org/licenses/>. 
*/

#ifndef _AEROQUAD_HCSR04_STM32_H_
#define _AEROQUAD_HCSR04_STM32_H_

#if !defined(AeroQuadSTM32)
  #error This code only works on STM32
#endif

#if defined(CameraControl)
  #error Current implementation conflict with CameraControl
#endif

#include "RangeFinder.h"

#define TRIGGER_PIN (Port2Pin('A',2))	// TIM5CH3
#define ECHO_PIN (Port2Pin('A',1))	// TIM5CH2

#define TRIGGER_CLOCK_SPEED 100000 // in Hz
#define TRIGGER_FREQUENCY 10       // in Hz
#define TRIGGER_LENGTH 11	   // in us
#define TRIGGER_COMPARATOR (11*TRIGGER_CLOCK_SPEED/1000000+1)	// +1 to ensure is greater
#define TRIGGER_ARR     (TRIGGER_CLOCK_SPEED/TRIGGER_FREQUENCY)

#define SPEED_OF_SOUND 340.0	// in m/s
#define DISTANCE_SCALE (2.0*TRIGGER_CLOCK_SPEED/SPEED_OF_SOUND)

#define MAX_DISTANCE 6	// in m
#define ECHO_MAX_VALUE (MAX_DISTANCE*DISTANCE_SCALE)

#define ECHO_TIMEOUT 200

typedef struct {
  timer_gen_reg_map  *timer_regs;
  __io uint32        *timer_ccr;
  uint16              rise_time;
  uint32              last_change;
  int                 polarity_mask;
  bool                valid;
} timer_data_t;

volatile timer_data_t timer_data;

void echo_change() {

	timer_gen_reg_map *timer = timer_data.timer_regs;
	uint16_t c = *(timer_data.timer_ccr);
	bool rising = (timer->CCER & timer_data.polarity_mask) == 0;


	if(timer_data.valid) {
		if(rising) {
			timer_data.rise_time = c;
		}
		else {
			int d = c-timer_data.rise_time;
			if ( d > ECHO_MAX_VALUE ) {
				rangeFinderRange[ALTITUDE_RANGE_FINDER_INDEX]= INVALID_RANGE;
			} else {
				rangeFinderRange[ALTITUDE_RANGE_FINDER_INDEX]=d / DISTANCE_SCALE;
				timer_data.last_change = millis();
			}
		}
	}
	else if(rising) {
		timer_data.rise_time = c;
		timer_data.valid = true;
	}

	timer_data.timer_regs->CCER ^= timer_data.polarity_mask; // invert polarity

}


void inititalizeRangeFinders() {

	timer_dev *timer_sonar = PIN_MAP[ECHO_PIN].timer_device;

	gpio_set_mode(PIN_MAP[ECHO_PIN].gpio_device, PIN_MAP[ECHO_PIN].gpio_bit, GPIO_AF_INPUT_PD);

	int aTimerChannel = PIN_MAP[ECHO_PIN].timer_channel-1;  // transform timer channel numbering from 1-4 to 0-3

	timer_data.valid        = false;
	rangeFinderRange[ALTITUDE_RANGE_FINDER_INDEX]= INVALID_RANGE;

	timer_gen_reg_map *timer = timer_sonar->regs.gen;
	timer_data.timer_regs    = timer;

	timer_data.timer_ccr    = &timer->CCR1 + aTimerChannel;

	int TimerEnable = (1 << (4*aTimerChannel));
	timer_data.polarity_mask = TimerEnable << 1;

	uint32 clock_speed = rcc_dev_timer_clk_speed(timer_sonar->clk_id);

	timer->PSC    = (clock_speed/TRIGGER_CLOCK_SPEED)-1;
	timer->ARR    = TRIGGER_ARR;
	timer->CR1    = 0;
	timer->DIER &= ~(1);

	timer->CCER &= ~TimerEnable; // Disable timer
	timer->CCER &= ~(timer_data.polarity_mask);

	volatile uint32 *mr;
	if(aTimerChannel < 2) {
		mr = &(timer->CCMR1);
	}
	else {
		mr = &(timer->CCMR2);
	}
	*mr &= ~(0xFF << (8*(aTimerChannel&1)));      // prescaler 1
	*mr |= 0x61 << (8*(aTimerChannel&1));         // 0x61 -> 6=filter, 1=inputs 1,2,3,4

	timer->CCER |= TimerEnable; // Enable
	timer->CR1 = 1;

	timer_attach_interrupt(timer_sonar, PIN_MAP[ECHO_PIN].timer_channel, echo_change);

	pinMode(TRIGGER_PIN,PWM);
	timer_generate_update(timer_sonar);
	timer_set_compare(timer_sonar, PIN_MAP[TRIGGER_PIN].timer_channel,  TRIGGER_COMPARATOR);
}


void    updateRangeFinders() {
	// only detect timeouts
	if ( ( millis() - timer_data.last_change ) > ECHO_TIMEOUT ) {
		rangeFinderRange[ALTITUDE_RANGE_FINDER_INDEX]= INVALID_RANGE;
	}
}


#endif  // #define _AEROQUAD_HCSR04_STM32_H_

