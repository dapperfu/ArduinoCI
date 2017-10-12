#include <Wire.h>
#include <PV_RTD_RS232_RS485_Shield.h>
// Create an object to talk to the RTD shield.
// 82 is the I2C (Wire) interface address.
// 100.0 is the type of RTD sensor being used (100.0 for Pt-100)
// (Replace the 100.0 below with 1000.0 if you are using a Pt-1000 RTD)
PV_RTD_RS232_RS485 my_rtds( 82, 100.0 );

int d = 2500;

void setup() {
  Serial.begin( 115200 );
  Serial.println( "t,RT1," );
  
  // This calls Wire1.begin() for Due and Wire.begin() for other Arduinos
  I2C_RTD_PORTNAME.begin();
  
  // This will put us in a "fresh" state at startup, though you probably
  // wouldn't want to do a factory reset every time you power up.
  my_rtds.Factory_Reset();
  // Next, we enable the channels which we want to read
  my_rtds.Disable_All_RTD_Channels();
  my_rtds.Enable_RTD_Channel( 3, 1 );
  
  // Now, the next three commands configure the shield to maximize stability
  // The following settings are particular to 3-wire Pt-100 RTDs.  If you 
  // are using different RTDs contact us for the best settings
  // (support@protovoltaics.com)
  
  // Slow the shield down to 20 samples-per-second.  The shield automatically
  // takes 16 samples at this rate then stores the median value for reporting
  // to the Arduino.  This also enables the 50Hz/60Hz rejection filters,
  // which are only enabled for speeds at or below 20 samples-per-second.
  // You can reduce this value to as low as 5, but you will have to wait
  // about 6.6 seconds for each new reading.  The slower you go, the less
  // noise there will be in the measurements.
  my_rtds.Set_RTD_SPS(16);
  
  // Set the RTD drive current to 250uA.  This is typically the best setting.
  // Higher settings will provide common-mode errors to the shield.  Lower
  // values will be more susceptible to noise.
  my_rtds.Set_RTD_Idac( 3, 1, 0.000250 );
  
  // Set the programmable gain amplifier to 64.  This will limit readings to
  // 89.1 deg C (192.4 deg F).
  // A PGA value of 32 will allow measurements up to 463.5 deg C (866.3 deg F) 
  // but the noise rejection is not as good as it is at 64.
  // A PGA value of 16 will allow measurements up to the limit of the RTD 
  // sensor but the noise rejection is not as good as it is at 64 or 32.
  my_rtds.Set_RTD_PGA( 3, 1, 32 );
  
  // A short delay so that the first reading can be taken.  If you request
  // a reading before the shield has taken its first reading you will get a 
  // bogus number.  The shield also performs a self-calibration that we have
  // to allow to complete.
  // Set this delay to 10000 if using 5 samples-per-second above.
  delay(d);
}
void loop() {
  Serial.print(millis());
  Serial.print(",");
  Serial.print(my_rtds.Get_RTD_Temperature_degC( 3, 1 ));
  Serial.println("C," );
  
  // Add a delay to allow the shield to take a new measurement.
  // Set this delay to 6600 if using 5 samples-per-second
  delay(d);
}

