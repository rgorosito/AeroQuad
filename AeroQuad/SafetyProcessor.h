/*
  AeroQuad v3.0 - Febuary 2012
  www.AeroQuad.com
  Copyright (c) 2011 Ted Carancho.  All rights reserved.
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

// TODO: breve explicación de lo que hace esto


#ifndef _AQ_SafetyProcessor_H_
#define _AQ_SafetyProcessor_H_

byte safetyMode = 0;

#if defined (RSSIReader)
// TODO: poner un tiempo de critical 
#define RSSI_CRITICAL 1<<0
#endif


void processSafety()
{

#if defined (RSSIReader)
    // if rssi is critical & !returning => return to home
    // if rssi is !critical & returning => return previous mode
    if (safetyMode & RSSICRITICAL) {	// still 
	if (rssiRawValue > RSSI_CRITVAL) {	// no, permit radio control
	    safetyMode &= ~RSSI_CRITICAL;
	}
    } else {
	if (rssiRawValue < RSSI_CRITVAL) {	// enter on "return to
	    // home" mode ?
	    safetyMode |= RSSI_CRITICAL;
	}
    }
#endif


    // if distance_to_home is warining => return to home
    // if distance_to_home is critical => shutdown





#endif
