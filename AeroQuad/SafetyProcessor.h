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

//byte safetyMode = 0;
unsigned long rssi_fail_start;
unsigned long dist_crit_start;
unsigned long dist_warn_start;

#define OK 0
#define WARNING 1
#define CRITICAL 2

byte rssi_status=OK;
byte dist_status=OK;
byte gps_status=OK;


// TODO: poner los ifdef que sean necesarios

//#define RSSI_CRITICAL_FLAG      1<<0
//#define DISTANCE_WARNING_FLAG   1<<1
//#define DISTANCE_CRITICAL_FLAG  1<<2

#define RSSI_STATUS_THRESHOLD 3000 // 3 segundos
#define RSSI_CRITICAL_TIMEOUT 5000 // 5 seconds
#define DISTANCE_WARNING   20 // in meters
#define DISTANCE_CRITICAL  40 // in meters
#define DISTANCE_TIMEOUT 5000 // 5 seconds

#define SAFETY_ACTION_NOTHING 0		// no action
#define SAFETY_ACTION_AUTODESCEND 1	// permit user
#define SAFETY_ACTION_POSITION_HOLD 2
#define SAFETY_ACTION_POSITION_HOLD_AUTODESCEND 3
#define SAFETY_ACTION_RETURN_HOME 4
#define SAFETY_ACTION_SHUTDOWN 255
// otras acciones: DONT_ARM (nota: va major algo tipo enum???)

// LA IDEA ES: #define ON_BATTERY_WARNING SAFETY_ACTION_POSITION_HOLD_AUTODESCEND
// algunos disparadores:
//   ON_BATTERY_W/C
//   ON_RSSI_W/C
//   ON_DISTANTCE_TO_HOME_W/C
//   ON_ALTITUDE_W/C
//   ON_GPS_SIGNAL_W/C
// nota: hay que ordenar las acciones por severidad, de modo de que si pasan más de una cosa, que la acción sea la más severa
//
#define ON_RSSI_CRITICAL SAFETY_ACTION_SHUTDOWN

    // if rssi is critical & !returning => return to home
    // if rssi is !critical & returning => return previous mode
    //
    // varios modos a definir con un define, ej:
    //   on_rssi_crit_shutdown , on_rssi_crit_return_home
    //
    // a ver ...
    // el si el status es ok, tiene que ser ok por 5 seg para que sea ok?
    // si: ok->1 seg warn, 2seg crit, 3seg warn, 4seg ok, 5seg warn, que pasa?
    //
    // renombrar a securityprocessor?

byte safety_action=SAFETY_ACTION_NOTHING;

void processSafety()
{
	byte new_rssi_status;
	byte rssi_action=SAFETY_ACTION_NOTHING;
	byte dist_action=SAFETY_ACTION_NOTHING;
	byte new_action=SAFETY_ACTION_NOTHING;

    if ( rssiRawValue < RSSI_CRIT ) {
	new_rssi_status=CRITICAL;
    } else if ( rssiRawValue < RSSI_WARN ) {
	new_rssi_status=WARNING;
    } else {
        new_rssi_status=OK;
    }

    // si ahora y antes habia error (el mismo) no hacer nada
    // si no habia error y ahora si, y si el tiempo está seteado a algo distinto de 0, verificarlo, sino, setearlo en actual
    // si habia error y ahora no, setear el tiempo en 0 y el estado en ok
    // si habia un error distinto y el tiempo se 0, setearlo, sino, verificarlo,


    // posibile implementacion: despues del tiempo, toma el valor que tenga. otra opcion es guardar el peor y tomar el peor
    // cambio el estado?
    if ( rssi_status != new_rssi_status) {
        if ( rssi_fail_start == 0 ) { // el estado es la primera vez que cambia?
		rssi_fail_start = millis();
	} else if ( rssi_fail_start+RSSI_STATUS_THRESHOLD < millis() ) { // pasó el tiempo?
		rssi_status = new_rssi_status;
		rssi_fail_start=0;
	}
    }

#ifdef ON_RSSI_WARNING
	if ( rssi_status == WARNING ) {
		rssi_action = ON_RSSI_WARNING;
	}
#endif
#ifdef ON_RSSI_CRITICAL
	if ( rssi_status == CRITICAL ) {
		rssi_action = ON_RSSI_CRITICAL;
	}
#endif

    if ( rssi_action > new_action ) {
	new_action = rssi_action;
    }


 // y al finaal
  safety_action = new_action;


 // y al más final, hacer las cosas:


 switch (safety_action) {

	case SAFETY_ACTION_SHUTDOWN:
		commandAllMotors(MINCOMMAND);
		motorArmed = OFF;
		inFlight = false;
		#ifdef OSD
			notifyOSD(OSD_CENTER|OSD_CRIT, "SAFETY SHUTDOWN!");
		#endif
		break;

	default:
		#ifdef OSD
			notifyOSD(OSD_CENTER|OSD_CRIT, "SAFETY: UNKNOW ACTION %d!", safety_action);
		#endif
}

/* PARA NO USAR MILISS:
 *     if (batteryMonitorAlarmCounter < BATTERY_MONITOR_MAX_ALARM_COUNT) {
 *       if (batteryAlarm) {
 *         batteryMonitorAlarmCounter++;
 *       }
 *     }
 *
 */

// IDEM distancia al home, señal de gps, batería, etc.
//
//
// IDEA!!! LED_MODE (quizas varios modos a la vez)
//
//

		
/*

    switch ( rssi_status ) {
	case OK:
		break;
	case WARNING:
		break;
	case CRITICAL:

	defaul:
	}
    if ( rssi_status ) {	// still 
	if (rssiRawValue > RSSI_CRITVAL) {	// no, permit radio control
	    safetyMode &= ~RSSI_CRITICAL;
	}
    } else {
	if (rssiRawValue < RSSI_CRITVAL) {	// enter on "return to
	    // home" mode ?
	    safetyMode |= RSSI_CRITICAL;
	}
    }
*/

    // if distance_to_home is warining => return to home
    // if distance_to_home is critical => shutdown




}

#endif
