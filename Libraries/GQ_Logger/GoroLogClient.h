
#ifdef WITH_GOROLOG
  #include "GoroLog.h"
  
binary_log_data_t logdata;


void initialize_logger() {
  GOROL_CLIENT.begin(GOROL_SPEED);
}

void send_log_uptime() {
  
  GOROL_CLIENT.write(GOROL_UPTIME);
  logdata.largo_sin_signo = millis();
  send_log_data(sizeof(logdata.largo_sin_signo));
  
}
/*
void send_log_compass() {
  
  GOROL_CLIENT.write(GOROL_COMPASS);
  logdata.compass.hdgX = compass.hdgX;
  logdata.compass.hdgY = compass.hdgY;
  logdata.compass.measuredMagX = compass.measuredMagX;
  logdata.compass.measuredMagY = compass.measuredMagY;
  logdata.compass.measuredMagZ = compass.measuredMagZ;
  send_log_data(sizeof(compass_logdata_t));

}

void send_log_gps() {
  GOROL_CLIENT.write(GOROL_GPS);
  gps.get_position(&logdata.gps.lat, &logdata.gps.lon, &logdata.gps.fix_age);
  gps.stats(&logdata.gps.chars,&logdata.gps.good_sentences,&logdata.gps.failed_cs);
  send_log_data(sizeof(gps_logdata_t));
}
*/
void send_log_data(uint8_t buflen) {
  GOROL_CLIENT.write(logdata.dato,buflen);
}


#endif




