#ifndef PV_RTD_SHIELD
#define PV_RTD_SHIELD

#include <Arduino.h>
#include <Print.h>

/// The I2C port to use.
#ifdef _VARIANT_ARDUINO_DUE_X_
  #define I2C_RTD_PORTNAME Wire1
#else
  #define I2C_RTD_PORTNAME Wire
#endif


//#define RTD_DEBUG true    ///< Verbose debugging flag.


/** \mainpage ProtoVoltaics Multi-Channel RTD Arduino Shield with RS232 and RS485 Transceivers
    \section intro_sec Introduction
    Thanks for taking a look at our shield! We value your feedback and contributions.  Contact us at
    <A HREF="mailto:support@protovoltaics.com">support@protovoltaics.com</A> if you find any errors.
    
    This Arduino shield has multiple channels to read 
    <A HREF="http://en.wikipedia.org/wiki/Resistance_thermometer">Resistance Temperature Detectors</A> 
    as well as transceivers to communicate with RS232 and RS485 devices. The Arduino communicates with the shield 
    over the I2C (Wire) pins, so the RS232 (3-wire RS232) and RS485 transceivers don't connect to the Arduino's 
    serial ports. The shield can also connect to the Arduino's D2, D3, D4, and D5 pins to alert the Arduino of 
    various conditions such as new data from the shield's RS232 or RS485 ports or if an RTD reading meets some 
    criteria.

    If you are looking for details on all of the commands you can give the RTD shield, click on the 
    <A HREF="http://prods.protovoltaics.com/rtd-rs232-rs485/docs/html/annotated.html">"Classes"</A>
    link in the menubar at the top of the page.  To dive straight in you can 
    <A HREF="http://prods.protovoltaics.com/rtd-rs232-rs485/docs/html/class_p_v___r_t_d___r_s232___r_s485.html">click here.</A>
    
    \section quick_start_sec Quick Start
        1. Download and extract the interface library for communicating with the shield.
           <A HREF="http://prods.protovoltaics.com/rtd-rs232-rs485/lib/PV_RTD_RS232_RS485_Shield.zip">Download link for Arduino RTD interface library.</A>   
        2. Install the library. 
           <A HREF="http://arduino.cc/en/Guide/Libraries">Instructions for installing an Arduino library.</A>
        3. Create a new Arduino sketch.
        4. Add the RTD library to the sketch: Sketch | Import Library... | PV_RTD_RS232_RS485_Shield 
        5. Create a minimal program to read your RTDs:
           <A HREF="http://prods.protovoltaics.com/rtd-rs232-rs485/examples/simple/RTD_Shield_Minimal_Test.ino">Download the code below</A>
    \code

#include <Wire.h>
#include <PV_RTD_RS232_RS485_Shield.h>

// Create an object to talk to the RTD shield.
// 82 is the I2C (Wire) interface address.
// 100.0 is the type of RTD sensor being used (100.0 for Pt-100)
// (Replace the 100.0 below with 1000.0 if you are using a Pt-1000 RTD)
PV_RTD_RS232_RS485 my_rtds( 82, 100.0 );


void setup() {
  Serial.begin( 115200 );
  Serial.println( "Starting up..." );
  
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
  my_rtds.Set_RTD_SPS( 20 );
  
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
  my_rtds.Set_RTD_PGA( 3, 1, 64 );
  
  // A short delay so that the first reading can be taken.  If you request
  // a reading before the shield has taken its first reading you will get a 
  // bogus number.  The shield also performs a self-calibration that we have
  // to allow to complete.
  // Set this delay to 10000 if using 5 samples-per-second above.
  delay( 3000 );
}


void loop() {
  // Read the temperature and display the results
  float t = my_rtds.Get_RTD_Temperature_degC( 3, 1 );
  Serial.print( "The three-wire RTD on channel 1 is reading " );
  Serial.print( t, 6 );
  Serial.println( " degrees Celsius" );
  
  // Add a delay to allow the shield to take a new measurement.
  // Set this delay to 6600 if using 5 samples-per-second
  delay( 1600 );
}

    \endcode
        6. Set the jumpers on top of the shield.<br>
	   If you have any 2-wire RTDs then you should place a jumper on the header labeled "2W".  If you do
	   not have any 2-wire RTDs, then it's better to remove the jumper from the header labeled "2W".
	   <b>Remember, 2-wire RTDs can not give you an accurate temperature reading and you should expect to 
	   be off about +/- 1 degree Celsius for each inch of wire between a 2-wire RTD and the shield.
	   There is no way to physically compensate for the resistance loss in the wires of a 2-wire RTD, 
	   so you should always use a 3-wire or 4-wire connection. </b><br>
	   The "LAST RTD PIN" should be set for the type of RTD being connected to the last screw terminal.
	   If you are connecting a 3-wire RTD to channel 4 (that's 3-wire channel 4) on the shield put a 
	   jumper connecting the "3W" pin to the center pin on the "LAST RTD PIN" header.  If you are 
	   connecting a 4-wire RTD to channel 3 (that's 4-wire channel 3) of the shield put a jumper 
	   connecting the "4W" pin to the center pin on the "LAST RTD PIN" header.<br>
	   If you want the shield to send the Arduino alerts when the RTD readings meet a criteria that you 
	   set, then add the jumpers desired on the "IRQ D2" through "IRQ D5" headers.  If you are using 
	   those pins for other purposes just remove the jumpers.
        7. Connect your RTD sensors to the shield's screw terminals.<br>
	   For a 3-wire RTD, two of the wires will have very little (nearly 0) resistance between them and
	   these should go in to the "-" terminals.  The other wire should have more resistance (about 
	   110-ohms for a Pt-100 RTD or 1100-ohms for a Pt-1000 RTD) and should go in to the "+" terminal.
	   For a 4-wire RTD, you will have two groups of wires with very little (nearly 0) resistance
	   between them.  One group goes to the termials labeled "+" and the other group to the terminals 
	   labeled "-".
        8. Run your code.
        9. Throw your arms up in 'V' and chant "victorious".
    
    \section rtd_sec Resistance Temperature Detectors Overview
    RTDs have better linearity and accuracy compared to other temperature measurement technologies such as 
    thermocouples. However RTDs are typically limited to measurements less than 600 degrees Celsius. 
    
    Changes in temperature cause a change the resistance of the RTD. The shield is actually just measuring the 
    resistance on its terminals, so it's really just a very accurate ohm-meter.  Since the temperature changes 
    cause only slight changes in the sensor's resistance, very accurate measurements must be made.
    
    \section hw_sec Hardware Overview
    The shield has a 12-pin screw terminal for connecting RTDs (or other resistance based sensors), a 3-pin 
    terminal for RS232 communication, and a 2-pin terminal for RS485 communication. The RS232 and RS485 terminals 
    are wired to transceivers that perform voltage level shifting for UARTs on a microcontroller that manages 
    the shield. The RTD terminals are wired to a 24-bit analog-to-digital converter (ADC) which also provides 
    excitation for the RTDs. A precision current source is used to excite the RTDs. The voltage across the RTD 
    is then measured and the resistance is calculated based on the voltage and current values.
    
    \section channels_sec RTD Channels
    The shield can connect to seven two-wire RTDs, four three-wire RTDs, three four-wire RTDs, or certain 
    combinations of various RTDs. The "or" in the previous sentence is an exclusive-or: meaning you can't connect
    seven two-wire RTDs, "and" four three-wire RTDs, "and" three four-wire RTDs. If a channel is used to connect to a 
    three-wire RTD then the pins used for that channel cannot be used to connect to a two-wire or four-wire RTD. 
    
    The RTD channels are labeled in numeric order starting from the RS232 port down to the RS485 port. Meaning 
    channel 1 is the closest to the RS232 port, channel 2 is next, and so on. The last channel is channel 7 for the 
    two-wire ports, channel 4 for the three-wire ports, and channel 3 for the four-wire ports.
    
    \section rtd_connections_sec RTD Connections
    \subsection two_wire_subsec Two-Wire RTDs
    Directly behind the screw-terminals for the RTDs on the shield is the labeling diagram for making two-wire
    connections. Two-wire RTDs have two wires. One wire must go into a numbered terminal, which are labeled 1 
    through 7. The other wire must go into a terminal labeled 'C'. The 'C' stands for "common". It doesn't matter
    which 'C' pin is used and a 'C' pin can be used to connect to more than one two-wire RTD. The RTD is not
    polarized, so either wire can go in the numbered terminal and either wire can go in the 'C' terminal.
    
    If two-wire RTDs are used, the jumper terminal labeled "2W" must have a jumper installed on it. 
    
    If two-wire RTDs are used, then the pin labeled '7' can only be used for two-wire RTDs. You cannot use the 
    pin labeled '7' for three-wire or four-wire RTDs when a jumper is installed in the jumper terminal labeled "2W".
    
    \subsection three_wire_subsec Three-Wire RTDs
    Three-wire RTDs have three wires, obviously. Two of the wires are connected together at the tip of the sensor. 
    If you measure the resistance with an ohm-meter you will see zero or almost zero. The two wires that have
    nearly zero resistance go into the terminals labeled '-'.  The other wire, which will have a measurable 
    resistance much larger than zero (it depends on the RTD type, but 110 ohms or 1100 ohms are typical values), 
    should go into the terminal labeled '+'.
    
    If three-wire channel 4 is going to be used to measure a three-wire RTD then the jumper terminal labeled 
    'LAST RTD PIN' must have a jumper installed between the middle pin and the pin labeled '3W'.
    
    \subsection four_wire_subsec Four-Wire RTDs
    Four-wire RTDs are the most accurate type of RTDs. Two wires are connected to each side of the RTD element 
    at the tip of the sensor. The wires in a pair will have zero or nearly zero resistance between them. One of 
    the pairs gets installed in the terminals labeled '+' and the other pair gets installed in the terminals 
    labeled '-'.
    
    If four-wire channel 3 is going to be used to measure a four-wire RTD then the jumper terminal labeled 
    'LAST RTD PIN' must have a jumper installed between the middle pin and the pin labeled '4W'.
    
    \section rtd_sw_subsec RTD Commands
    The general process for reading a temperature measurement is the following: 
        1. Include the appropriate header libraries
            \code
              #include <Wire.h>
              #include <PV_RTD_RS232_RS485_Shield.h>
            \endcode
            <BR>
        2. Create the PV_RTD_RS232_RS485 object
            \code
               // Use I2C address 82 for the shield.  
               // Tell the shield we are using Pt-100 RTDs (by sending the 100.0).
               PV_RTD_RS232_RS485 my_rtds( 82, 100.0 );  
            \endcode
            <BR>
        3. Initialize the Wire library by calling Wire library's begin() function
            \code
              I2C_RTD_PORTNAME.begin();
            \endcode
            The above (I2C_RTD_PORTNAME.begin()) will work for all Arduinos. If you have an Arduino Due 
            you could call Wire1.begin() instead. If you have any other type of Arduino you could call 
            Wire.begin() instead. I2C_RTD_PORTNAME is defined in the PV_RTD_RS232_RS485_Shield.h file. 
            <BR><BR>
        4. Enable the desired RTD channel via PV_RTD_RS232_RS485::Enable_RTD_Channel()
            \code
              my_rtds.Enable_RTD_Channel( 3, 1 );  // Tell the shield a 3-wire RTD is on channel 1.
            \endcode
            <BR>
        5. Read the temperature via PV_RTD_RS232_RS485::Get_RTD_Temperature_degC() or similar function
            \code
              float the_temperature = my_rtds.Get_RTD_Temperature_degC();
            \endcode
    
    \section rtd_accuracy_sec RTD Repeatability
    Since the shield is really just a precision ohmmeter, the performance of the shield can be verified 
    by connecting a precision resistor of known accuracy to the shield and taking multiple readings. 
    The factors that will impact the quality of your measurements the most are the speed 
    ( PV_RTD_RS232_RS485::Set_RTD_SPS() ), gain ( PV_RTD_RS232_RS485::Set_RTD_PGA() ), and drive current 
    ( PV_RTD_RS232_RS485::Set_RTD_Idac() ) settings.  The graph 
    below shows how the speed settings impacts readings.
    \image html sps_comparison.png
    As you can see slower speeds provide less variation when the input is fixed.  However, at a speed of
    5 samples-per-second the shield only updates the temperature measurement about once every 6.6 seconds.
    The 6.6 second time is due to the speed setting, time for internal calibration, and sampling of 
    multiple readings done internally by the shield.

    The settings for PGA and Idac depend on the maximum temperature you need to measure and the type of 
    sensor you are using.  Our team can provide you with these values based on your application.

    The following graph shows a histogram of readings made a precision 100 ohm resistor using a four-wire
    connection.  Typical Pt100 RTDs have a resistance of 100 ohms at 0 degrees Celsius.
    \image html four_wire_rtd_v8.gif
    The graph above gives an indication of how repeatable the shield is.  The spread between the maximum
    and minimum reading for this test was 0.007675 ohms, which corresponds to 0.020 degrees Celsius or 
    0.035 degrees Fahrenheit.  This data was collected at 160 samples-per-second.
    
    The following graph shows histogram data for a fixed resistor connected in a 3-wire configuration: 
    a jumper wire connected between the two "-" terminals.
    \image html three_wire_rtd_v8.gif
    This time the spread between minimum and maximum readings was 0.00975 ohms, 0.025 degrees Celsius, or
    0.045 degrees Fahrenheit.  This data was collected at 160 samples-per-second.
    
    A two-wire connection is almost never used.  This is because the two-wire connection has no 
    compensation for wire length, which will have a dramatic impact on measured values.  Do not use 
    two-wire RTDs unless you are an expert.  
    \image html two_wire_rtd_v8.gif
    This time the spread between minimum and maximum readings was 0.007531 ohms, 0.019 degrees Celsius, or
    0.035 degrees Fahrenheit.  The results demonstrate good repeatability, but the accuracy of the 
    measurements will decrease with every millimeter of distance from the shield's screw terminals to
    the active part of the RTD sensor.  This data was collected at 160 samples-per-second.
    
    \section rtd_range_sec RTD Measurement Range
    The maximum measurable resistance value is controlled by the programmable gain amplifier (PGA), 
    which is controlled with the PV_RTD_RS232_RS485::Set_RTD_PGA() method.  Using a larger gain gives 
    better resolution but reduces the maximum measurable resistance, which reduces the maximum measurable 
    temperature.  Lower gain values reduce the resolution in the readings, but allow larger resistances 
    to be measured.  
    
    For version 1 of the shield (that is for shield's with a date stamp prior to "2-MAY-2014") 
    a gain of 1 will allow resistances up to 833 ohms to be measured for two-wire and four-wire 
    RTDs and up to 1666 ohms for three-wire RTDs.  As the gain is doubled, the maximum measurable 
    resistance is cut in half.  The maximum gain of 128 allows readings up to 6.5078125 ohms to be made for 
    two-wire and four-wire RTDs and up to 13.015625 ohms for three-wire RTDs.
    
    For version 2 of the shield (that is for shield's with a date stamp of "2-MAY-2014" and after)
    a gain of 1 will allow resistance up to 4300 ohms to be measured for two-wire and four-wire
    RTDs and up to 8600 ohms for three-wire RTDs.  As the gain is doubled, the maximum measurable
    resistance is cut in half.  The maximum gain of 128 allows readings up to 33.59375 ohms to be made for 
    two-wire and four-wire RTDs and up to 67.1875 ohms for three-wire RTDs.

    The factory reset value for the PGA is 1 for version 2 of the shield. This means that you can measure
    the full range for Pt-100 and Pt-1000 RTDs without having to make any changes.  You may want to change 
    the PGA value to increase your temperature resolution if you will not need to take measurements across 
    the entire temperature range of your sensor.  See the details in the description for 
    PV_RTD_RS232_RS485::Set_RTD_PGA().
    
    \section i2c_sec I2C Communication
    Communication between the Arduino and the shield takes place over the I2C pins. The software provided with 
    the shield performs the necessary communication operations, so most users won't need to know about or care 
    how this communication occurs at the low level. For those who are interested, the source code is available 
    to see how this communication is used to control the shield. The registers are enumerated in the 
    PV_RTD_RS232_RS485_Memory_Map.h file if low-level communication is needed.
    
    \subsection i2c_addr_subsec I2C Address
    Solder pads on the top of the shield are used to set the I2C address of the shield. The default value is 82.
    The only reason you might want to change this is because you need to use multiple RTD shields or if you 
    have another I2C device using address 82.  You will have to cut the gold line connecting the center pad 
    before adding a solder jumper to connect it to the other pad.
    
    \subsection i2c_pu_subsec I2C Pull Ups
    The shield has two pull up resistors for the I2C bus. These are 10 kilo-ohm resistors. To disable the pull-ups the 
    solder jumper pads on the top of the shield can be removed with solder wick. The I2C pull up pads are under the 
    pads for setting the I2C address. They are labeled "I2C PU". The pin next to the 'D' is connected to SDA and the 
    pin next to the 'C' is connected to SCL.
    
    \subsection i2c_old_uno Older Arduino Unos
    Older Arduino Unos that do not conform to the Arduino R3 specification have their I2C pins in a different 
    location. To enable the shield to communicate to those pins a solder jumper must be installed on the solder 
    jumper ports labeled "UNO SDA" and "UNO SCL"
    
    \section rs232_sec RS232 Interface
    The RS232 screw terminal exposes pins for connecting the Tx (transmit), Rx (receive), and GND (ground) lines. 
    No provision is included for connecting other RS232 control signals such as CTS or RTS. The pins are also 
    labeled for connecting to DB9 connectors: Tx on DB9 pin 2, Tx on DB9 pin 3, and GND on DB9 pin 5. 
    
    Data can be written to the shield's RS232 port with Arduino's print() and println() functions if configured. 
    Configuration is done by calling the PV_RTD_RS232_RS485::Connect_Print_To() function.
    
    \section rs485_sec RS485 Interface
    The RS485 screw terminal exposes pins for connecting the A and B lines to an RS485 network. The shield has a 
    120 ohm termination resistor installed between the A and B terminals. Different hardware will define ports A 
    and B differently. If communication is not working properly, often it is useful to try reversing the pins to 
    see if this solves the problem. Refer to the transceiver's datasheet if necessary. Transceivers from Texas 
    Instruments and Maxim are typically used.

    Data can be written to the shield's RS485 port with Arduino's print() and println() functions if configured. 
    Configuration is done by calling the PV_RTD_RS232_RS485::Connect_Print_To() function.
    
    \section alarms_sec Alarms
    The shield has jumpers to connect to pins D2, D3, D4, and D5 on the Arduino. When the jumpers are installed 
    the shield can signal the Arduino when certain events occur. For example, the shield can send an alert when 
    new data arrives on the RS232 port, or on the RS485 port, or when RTD data matches a given criteria.
    
    See PV_RTD_RS232_RS485::Set_Alarm_Source(), PV_RTD_RS232_RS485::Configure_RTD_Alarm(), and 
    PV_RTD_RS232_RS485::Enable_Alarm().
    
    \section mpu_sec Shield Microcontroller
    The shield has a basic microcontroller to perform the functions of the shield. Having a separate 
    microcontroller frees up the Arduino's processor for other tasks.
    
    \subsection mpu_mem_subsec Microcontroller Memory
    Non-volatile EEPROM memory is used on the microcontroller to store all static variables. This means when 
    the shield is powered off and then powered on the shield will go back to processing the same RTD channels 
    and using the same alert configuration that was being used before the power was turned off.
    
    Dynamic variables, such as the last RTD measurements, RS232 buffers, RS485 buffers, and internal ADC 
    registers are not stored in EEPROM and will be erased when the power is turned off. The variables that 
    are retained in non-volatile EEPROM are those with addresses below FIRST_RAM_REGISTER in 
    PV_RTD_RS232_RS485_Memory_Map.h
    
    \subsection mpu_prog_subsec Microcontroller Firmware
    The shield's microcontroller has an ICSP port on the top of the shield. A Microchip programmer, such as the 
    PICkit3 is needed to program the flash memory on the microcontroller. The firmware on the shield's 
    microcontroller cannot be updated directly from the Arduino at this point in time.
*/


/// ProtoVoltaics Resistance Temperature Detector Class.
/** Class object for communicating with the ProtoVoltaics multichannel RTD shield with RS232 and RS485 transceivers.
*/
class PV_RTD_RS232_RS485 : public Print {
  public:

    /// Class constructor.
    /** Creates an object to communicate with the RTD shield.
        \param i2c_address The I2C address of the RTD shield.
	\param R0 The resistance of the RTD sensor at 0 degrees Celsius.  This is 100.0 for a Pt-100 
	          RTD sensor, 1000.0 for a Pt-1000.0 RTD sensor, and so on.
     */
    PV_RTD_RS232_RS485( byte i2c_address, float R0 );
    
    /// Perform a software reset on the shield.
    void Reset();
    
    /// Perform a factory reset on the shield.
    void Factory_Reset();
    
    /// Returns the amperage output on the digital to analog converter output in units of amperes.
    /** The ADS1248 has two current sourcing digital-to-alalog converters.  The output current can not be independently 
        controlled, but the output connections of the DACs can be independently controlled.  This function returns the 
        output current setting for the DAC given a RTD wire and channel value.
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \return The output drive current of the current digital-to-analog converter.
        \sa Set_RTD_Idac()
     */
    float Get_RTD_Idac( byte wires, byte channel );
    
    /// Sets the amperage output on the digital to analog converter output current in units of amperes.
    /** This will set the output amperage used when making measurements for a RTD wire and channel configuration. If the value 
        of new_idac_value is not one of the possible settings then the value will not be changed. 1500e-6 ampere is the default value.
        
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \param new_idac_value The drive amperage to use for the wire and channel configuration. Must be a value from the ADS1248
               datasheet: 0, 50e-6, 100e-6, 250e-6, 500e-6, 750e-6, 1000e-6, or 1500e-6.
        \return The output drive amperage of the digital-to-analog converter.
        \sa Get_RTD_Idac()
     */
    float Set_RTD_Idac( byte wires, byte channel, float new_idac_value );
    
    /// Returns the reference voltage being used to bias input to the analog-to-digital converter in units of volts.
    /** The ADS1248 requires a biasing voltage for best performance.  This voltage is controlled by the hardware 
        configuration and can not be changed in software.
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \return The reference voltage based on the hardware configuration.
    */
    float Get_RTD_Vref( byte wires, byte channel );
    
    /// Returns the gain setting of the programmable gain amplifier.
    /** The ADS1248 amplifies the signal from the RTD before measuring it with its 24-bit analog-to-digital converter.
        This is the gain (which is a unitless quantity) for the given wire and channel values.
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \return The gain setting of the programmable gain amplifier.
        \sa Set_RTD_PGA()
    */
    float Get_RTD_PGA( byte wires, byte channel );
    
    /// Sets the gain for the programmable gain amplifier.
    /** 
	The multiplier to use when measuring the input voltage on the analog-to-digital converter's input terminals. If the 
	value of new_gain_value is not one of the possible settings then the value will not be changed.  For version 1 of the 
	shield (shield's with a date stamp prior to "2-MAY-2014"), the factory reset value is 4 for three-wire RTDs and 2 for 
	two-wire and four-wire RTDs.  For version 2 of the shield (shield's with a date stamp of "2-MAY-2014" and after), the 
	factory reset value is 1 for all wire types. <br>
	<center>
	<table border=1 cellpadding=0 cellspacing=0>
	 <tr>
	  <td colspan=11 align=center><b>Version 1 - Shields with a date stamp before "2-MAY-2014"</b></td>
	 </tr>
	 <tr>
	  <td></td>
	  <td colspan=5 align=center><b>2-Wire / 4-Wire</b></td>
	  <td colspan=5 align=center><b>3-Wire</b></td>
	 </tr>
	 <tr>
	  <td rowspan=2><b>PGA</b></td>
	  <td rowspan=2 align=center><b>Max R [ohms]</b></td>
	  <td colspan=2 align=center><b>Pt-100</b></td>
	  <td colspan=2 align=center><b>Pt-1000</b></td>
	  <td rowspan=2 align=center><b>Max R [ohms]</b></td>
	  <td colspan=2 align=center><b>Pt-100</b></td>
	  <td colspan=2 align=center><b>Pt-1000</b></td>
	 </tr>
	 <tr>
	  <td align=center><b>Max [&deg;C]</b></td>
	  <td align=center><b>Max [&deg;F]</b></td>
	  <td align=center><b>Max [&deg;C]</b></td>
	  <td align=center><b>Max [&deg;F]</b></td>
	  <td align=center><b>Max [&deg;C]</b></td>
	  <td align=center><b>Max [&deg;F]</b></td>
	  <td align=center><b>Max [&deg;C]</b></td>
	  <td align=center><b>Max [&deg;F]</b></td>
	 </tr>
	 <tr>
	  <td align=right><b>1</b></td>
	  <td align=right>833.3</td>
	  <td>Full Range</td>
	  <td>Full Range</td>
	  <td align=right>-42.4</td>
	  <td align=right>-44.3</td>
	  <td align=right>1666.7</td>
	  <td>Full Range</td>
	  <td>Full Range</td>
	  <td align=right>175.1</td>
	  <td align=right>347.2</td>
	 </tr>
	 <tr>
	  <td align=right><b>2</b></td>
	  <td align=right>416.7</td>
	  <td align=right>941.1</td>
	  <td align=right>1726.0</td>
	  <td align=right>-146.1</td>
	  <td align=right>-231.0</td>
	  <td align=right>833.3</td>
	  <td>Full Range</td>
	  <td>Full Range</td>
	  <td align=right>-42.4</td>
	  <td align=right>-44.3</td>
	 </tr>
	 <tr>
	  <td align=right><b>4</b></td>
	  <td align=right>208.3</td>
	  <td align=right>289.6</td>
	  <td align=right>553.3</td>
	  <td align=right>-196.8</td>
	  <td align=right>-322.3</td>
	  <td align=right>416.7</td>
	  <td align=right>941.1</td>
	  <td align=right>1726.0</td>
	  <td align=right>-146.1</td>
	  <td align=right>-231.0</td>
	 </tr>
	 <tr>
	  <td align=right><b>8</b></td>
	  <td align=right>104.2</td>
	  <td align=right>10.7</td>
	  <td align=right>51.2</td>
	  <td align=right>-221.9</td>
	  <td align=right>-367.5</td>
	  <td align=right>208.3</td>
	  <td align=right>289.6</td>
	  <td align=right>553.3</td>
	  <td align=right>-196.8</td>
	  <td align=right>-322.3</td>
	 </tr>
	 <tr>
	  <td align=right><b>16</b></td>
	  <td align=right>52.1</td>
	  <td align=right>-120.5</td>
	  <td align=right>-184.8</td>
	  <td align=right>-234.4</td>
	  <td align=right>-390.0</td>
	  <td align=right>104.2</td>
	  <td align=right>10.7</td>
	  <td align=right>51.2</td>
	  <td align=right>-221.9</td>
	  <td align=right>-367.5</td>
	 </tr>
	 <tr>
	  <td align=right><b>32</b></td>
	  <td align=right>26.0</td>
	  <td align=right>-184.2</td>
	  <td align=right>-299.6</td>
	  <td align=right>-240.6</td>
	  <td align=right>-401.2</td>
	  <td align=right>52.1</td>
	  <td align=right>-120.5</td>
	  <td align=right>-184.8</td>
	  <td align=right>-234.4</td>
	  <td align=right>-390.0</td>
	 </tr>
	 <tr>
	  <td align=right><b>64</b></td>
	  <td align=right>13.0</td>
	  <td align=right>-215.7</td>
	  <td align=right>-356.2</td>
	  <td align=right>-243.8</td>
	  <td align=right>-406.8</td>
	  <td align=right>26.0</td>
	  <td align=right>-184.2</td>
	  <td align=right>-299.6</td>
	  <td align=right>-240.6</td>
	  <td align=right>-401.2</td>
	 </tr>
	 <tr>
	  <td align=right><b>128</b></td>
	  <td align=right>6.5</td>
	  <td align=right>-231.3</td>
	  <td align=right>-384.3</td>
	  <td align=right>-245.3</td>
	  <td align=right>-409.6</td>
	  <td align=right>13.0</td>
	  <td align=right>-215.7</td>
	  <td align=right>-356.2</td>
	  <td align=right>-243.8</td>
	  <td align=right>-406.8</td>
	 </tr>
	</table>
	<br>
	<table border=1 cellpadding=0 cellspacing=0>
	 <tr>
	  <td colspan=11 align=center><b>Version 2 - Shields with a date stamp of "2-MAY-2014" or later</b></td>
	 </tr>
	 <tr>
	  <td></td>
	  <td colspan=5 align=center><b>2-Wire / 4-Wire</b></td>
	  <td colspan=5 align=center><b>3-Wire</b></td>
	 </tr>
	 <tr>
	  <td rowspan=2><b>PGA</b></td>
	  <td rowspan=2 align=center><b>Max R [ohms]</b></td>
	  <td colspan=2 align=center><b>Pt-100</b></td>
	  <td colspan=2 align=center><b>Pt-1000</b></td>
	  <td rowspan=2 align=center><b>Max R [ohms]</b></td>
	  <td colspan=2 align=center><b>Pt-100</b></td>
	  <td colspan=2 align=center><b>Pt-1000</b></td>
	 </tr>
	 <tr>
	  <td align=center><b>Max [&deg;C]</b></td>
	  <td align=center><b>Max [&deg;F]</b></td>
	  <td align=center><b>Max [&deg;C]</b></td>
	  <td align=center><b>Max [&deg;F]</b></td>
	  <td align=center><b>Max [&deg;C]</b></td>
	  <td align=center><b>Max [&deg;F]</b></td>
	  <td align=center><b>Max [&deg;C]</b></td>
	  <td align=center><b>Max [&deg;F]</b></td>
	 </tr>
	 <tr>
	  <td align=right><b>1</b></td>
	  <td align=right>4300.0</td>
	  <td>Full Range</td>
	  <td>Full Range</td>
	  <td align=right>988.8</td>
	  <td align=right>1811.9</td>
	  <td align=right>8600.0</td>
	  <td>Full Range</td>
	  <td>Full Range</td>
	  <td>Full Range</td>
	  <td>Full Range</td>
	 </tr>
	 <tr>
	  <td align=right><b>2</b></td>
	  <td align=right>2150.0</td>
	  <td>Full Range</td>
	  <td>Full Range</td>
	  <td align=right>308.3</td>
	  <td align=right>586.9</td>
	  <td align=right>4300.0</td>
	  <td>Full Range</td>
	  <td>Full Range</td>
	  <td align=right>988.8</td>
	  <td align=right>1811.9</td>
	 </tr>
	 <tr>
	  <td align=right><b>4</b></td>
	  <td align=right>1075.0</td>
	  <td>Full Range</td>
	  <td>Full Range</td>
	  <td align=right>19.2</td>
	  <td align=right>66.6</td>
	  <td align=right>2150.0</td>
	  <td>Full Range</td>
	  <td>Full Range</td>
	  <td align=right>308.3</td>
	  <td align=right>586.9</td>
	 </tr>
	 <tr>
	  <td align=right><b>8</b></td>
	  <td align=right>537.5</td>
	  <td align=right>1415.5</td>
	  <td align=right>2579.8</td>
	  <td align=right>-116.3</td>
	  <td align=right>-177.4</td>
	  <td align=right>1075.0</td>
	  <td>Full Range</td>
	  <td>Full Range</td>
	  <td align=right>19.2</td>
	  <td align=right>66.6</td>
	 </tr>
	 <tr>
	  <td align=right><b>16</b></td>
	  <td align=right>268.8</td>
	  <td align=right>463.5</td>
	  <td align=right>866.3</td>
	  <td align=right>-182.2</td>
	  <td align=right>-296.0</td>
	  <td align=right>537.5</td>
	  <td align=right>1415.5</td>
	  <td align=right>2579.8</td>
	  <td align=right>-116.3</td>
	  <td align=right>-177.4</td>
	 </tr>
	 <tr>
	  <td align=right><b>32</b></td>
	  <td align=right>134.4</td>
	  <td align=right>89.1</td>
	  <td align=right>192.4</td>
	  <td align=right>-214.7</td>
	  <td align=right>-354.4</td>
	  <td align=right>268.8</td>
	  <td align=right>463.5</td>
	  <td align=right>866.3</td>
	  <td align=right>-182.2</td>
	  <td align=right>-296.0</td>
	 </tr>
	 <tr>
	  <td align=right><b>64</b></td>
	  <td align=right>67.2</td>
	  <td align=right>-82.9</td>
	  <td align=right>-117.3</td>
	  <td align=right>-230.8</td>
	  <td align=right>-383.4</td>
	  <td align=right>134.4</td>
	  <td align=right>89.1</td>
	  <td align=right>192.4</td>
	  <td align=right>-214.7</td>
	  <td align=right>-354.4</td>
	 </tr>
	 <tr>
	  <td align=right><b>128</b></td>
	  <td align=right>33.6</td>
	  <td align=right>-165.8</td>
	  <td align=right>-226.5</td>
	  <td align=right>-238.8</td>
	  <td align=right>-397.9</td>
	  <td align=right>67.2</td>
	  <td align=right>-82.9</td>
	  <td align=right>-117.3</td>
	  <td align=right>-230.8</td>
	  <td align=right>-383.4</td>
	 </tr>
	</table>
	</center>

        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \param new_gain_value The desired multiplier for the ADC measurements.
        \return The gain setting of the programmable gain amplifier: either 1, 2, 4, 8, 16, 32, 64, or 128.
        \sa Get_RTD_PGA()
    */
    float Set_RTD_PGA( byte wires, byte channel, byte new_gain_value );

    /// Returns the speed setting of the temperature measurements in samples-per-second.
    /** This is the number of temperature readings that the shield takes per second.  Larger values give you an updated measurement more frequently.
        Lower values give you measurements with more "effective number of bits" in the reading, which should give you more stable readings.
        \return The temperature measurement speed in samples-per-second.
    */
    unsigned int Get_RTD_SPS();

    /// Sets the speed setting of the temperature measurements in samples-per-second.
    /** This is the number of temperature readings that the shield takes per second.  Larger values give you an updated measurement more frequently.
        Lower values give you measurements with more "effective number of bits" in the reading and less noise, which should give you more stable readings.
        \param new_sps_value The number of samples to take each second, which should be one of the following values: 5, 10, 20, 40, 80, 160, 320, 
	       640, 1000, or 2000.
        \return The temperature measurement speed in samples-per-second.
    */
    unsigned int Set_RTD_SPS( unsigned int new_sps_value );

    /// The bit-weight of the analog-to-digital converter readings.
    /** This is the scale factor that converts analog-to-digital readings into a voltage value. Multiplying the 
        analog-to-digital readings by this value will tell you the voltage on the analog-to-digital converter's
        inputs.
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \return The multiplying factor to convert ADC readings to the voltage read on the ADC's inputs.
    */
    float Get_RTD_Bit_Weight( byte wires, byte channel );
    
    /// Acquire a reading from the analog to digital converter.
    /** \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \param timeout_ms The maximum amount of time to wait for a reading from the analog-to-digital converter.
        \return The multiplying factor to convert ADC readings to the voltage read on the ADC's inputs.
    */
    unsigned long Get_RTD_ADC_Reading( byte wires, byte channel, int timeout_ms = PV_RTD_COMMUNICATION_TIMOUT_MS );
    
    /// Disable all RTD channels.
    /** This will tell the shield to not collect and process RTD measurements. This is typically used if you only want to 
        enable one channel: you could disable all channels and then enable the channel you care about.
        \return The return value from the I2C transmission.
    */
    int Disable_All_RTD_Channels();
    
    /// Returns a byte with bits representing which channels are enabled.
    /** The least-significant-bit represents the first channel.  Bit positions with a 1 mean the channel is enabled and 
        the shield will take RTD measurements and process them. If wires is 2, the most-significant-bit is meaningless and
        the lower 7 bits represent the seven two-wire RTD channels. If wires is 3, the 4 most-significant-bits are 
        meaningless and the lower 4 bits represent the four three-wire RTD channels. If wires is 4, the 5 most-significant
        bits are meaningless and the lower 3 bits represent the three four-wire RTD channels.
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \return The channels that are enabled: the channels that will collect and process RTD measurements.
    */
    byte Get_Enabled_RTD_Channels( byte wires );
    
    /// Returns true if the wire, channel values represent a valid value pair.
    /** There are seven two-wire RTD channels, four three-wire RTD channels, or three four-wire RTD channels. This method
        returns true if a value 1 through 7 is given when wires is 2; true if a value 1 through 4 is given when wires is 
        3; true if a value 1 through 3 is given when wires is 4.
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \return True if the wires, channel values represent a valid value pair.
    */
    boolean Is_Valid_RTD_Channel( byte wires, byte channel );
    
    /// Enables a channel for processing.
    /** Tells the shield to take RTD measurements for the given wires, channel combination.
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \return The return value from the I2C transmission.
    */
    int Enable_RTD_Channel( byte wires, byte channel );
    
    /// Prints the memory registers of the shield.
    /** Prints all of the memory registers of the shield using 'Serial'. The registers are defined in the PV_RTD_Memory_Map.h
        file.
    */
    void Print_Registers();
    
    /// Read a register from the shield.
    /** Returns the value at the given register_address on the shield. The register addresses are defined in the 
        PV_RTD_Memory_Map.h file.
        \param register_address The register to read: see PV_RTD_Memory_Map.h for a description of the register contents.
        \return The value contained at the given register_address.
    */
    byte Read_Register( int register_address );
    
    /// Write a register to the shield.
    /** Sets the value at the given register_address on the shield.  The register addressses are defined in the 
        PV_RTD_Memory_Map.h file.
        \param register_address The register to write: see PV_RTD_Memory_Map.h for a description of the register contents.
        \param data The value to be written.
        \return The return value from the I2C transmission.
    */
    int Write_Register( int register_address, byte data );
    
    /// Set the RS232 communication configuration.
    /** Sets the RS232 communication parameters.
        \param baud The communication baud rate.
        \param data_bits The data bits to use: must be either 8 or 9.
        \param parity The parity to use: must be either 'N' for None, 'O' (the capital-letter O) for Odd, or 'E' for Even.
        \param stop_bits The number of stop bits to use: must be either 1 or 2.
        \param polarity Setting this to true will invert the polarity on the receive line. The receive idle state is typically '1', setting
            this to 'true' will make the expected receive idle state '0'.
        \return True if settings were successfully applied, false if settings were invalid or not successfully applied.
    */
    boolean Set_RS232_Configuration( unsigned long baud = 115200, byte data_bits = 8, byte parity = 'N', byte stop_bits = 1, boolean polarity = false );
    
    /// Set the RS485 communication configuration.
    /** Sets the RS485 communication parameters.
        \param baud The communication baud rate.
        \param data_bits The data bits to use: must be either 8 or 9.
        \param parity The parity to use: must be either 'N' for None, 'O' (the capital-letter O) for Odd, or 'E' for Even.
        \param stop_bits The number of stop bits to use: must be either 1 or 2.
        \param polarity Setting this to true will invert the polarity on the receive line. The receive idle state is typically '1', setting
            this to 'true' will make the expected receive idle state '0'.
        \return True if settings were successfully applied, false if settings were invalid or not successfully applied.
    */
    boolean Set_RS485_Configuration( unsigned long baud = 115200, byte data_bits = 8, byte parity = 'N', byte stop_bits = 1, boolean polarity = false );
    
    /// Write a byte to the RS232 port.
    /** Writes a byte to the shield's RS232 port.
        \param data The value to write to the RS232 port.
        \return The return value from the I2C transmission.
    */
    int Write_RS232( byte data );
    
    /// Write a byte to the RS485 port.
    /** Writes a byte to the shield's RS485 port.
        \param data The value to write to the RS485 port.
        \return The return value from the I2C transmission.
    */
    int Write_RS485( byte data );
    
    /// Read a byte from the RS232 port.
    /** Reads a byte to the shield's RS232 port.
        \return The last value received on the shield's RS232 port. Zero if the shield has never received data on its RS232 port.
    */
    byte Read_RS232();
    
    /// Read a byte from the RS485 port.
    /** Reads a byte to the shield's RS485 port.
        \return The last value received on the shield's RS485 port. Zero if the shield has never received data on its RS485 port.
    */
    byte Read_RS485();
    
    /// True if new data is waiting on the shield's RS232 port.
    /** Determines if new data is waiting on the shield's RS232 port.
        \return True if there is a byte in the shield's RS232 receive buffer that has never been read.
    */
    boolean Has_RS232_Data();
    
    /// True if new data is waiting on the shield's RS485 port.
    /** Determines if new data is waiting on the shield's RS485 port.
        \return True if there is a byte in the shield's RS485 receive buffer that has never been read.
    */
    boolean Has_RS485_Data();
    
    /// Returns the voltage when the ADC is connected to a given channel.
    /** This function connects the analog-to-digital converter to the given wire, channel configuration and returns the measured
        voltage.
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \return The voltage measured on the analog-to-digital converter's input pins in units of volts.
    */
    float Get_RTD_Voltage( byte wires, byte channel );
    
    /// Returns the calculated resistance between the ADC's input terminals.
    /** The resistance between the ADC's inputs is calculated as Get_Voltage/Get_Idac. This will be the resistance of the RTD 
        element connected to the given wire, channel value.
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \return The resistnace between the ADC's input terminals in units of ohms.
    */
    float Get_RTD_Resistance( byte wires, byte channel );
    
    /// Returns the temperature based on the RTD reading in units of degrees Celsius.
    /** Measures the resistance on the port of the given wires, channel parameter and caluclates the temperature. 
        This uses the Callendar-Van Dusen equation for the temperature calculation.
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \return The temperature of the sensor in units of degrees Celsius.
    */
    float Get_RTD_Temperature_degC( byte wires, byte channel );
    
    /// Returns the temperature based on the RTD reading in units of degrees Fahrenheit.
    /** Measures the resistance on the port of the given wires, channel parameter and caluclates the temperature. 
        This uses the Callendar-Van Dusen equation for the temperature calculation.
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \return The temperature of the sensor in units of degrees Fahrenheit.
    */
    float Get_RTD_Temperature_degF( byte wires, byte channel );
    
    /// Returns the temperature based on the RTD reading in units of Kelvin.
    /** Measures the resistance on the port of the given wires, channel parameter and caluclates the temperature. 
        This uses the Callendar-Van Dusen equation for the temperature calculation.
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \return The temperature of the sensor in units of Kelvin.
    */
    float Get_RTD_Temperature_K( byte wires, byte channel );
    
    /// Returns the temperature based on the RTD reading in units of degrees Rankine.
    /** Measures the resistance on the port of the given wires, channel parameter and caluclates the temperature. 
        This uses the Callendar-Van Dusen equation for the temperature calculation.
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \return The temperature of the sensor in units of degrees Rankine.
    */
    float Get_RTD_Temperature_degR( byte wires, byte channel );
    
    /// Sets the upper alarm limit for an RTD channel.
    /** This will set the upper threshold for an alarm limit for the given wires, channel pair.
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \param limit The threshold value given as a analog-to-digital converter measurement value.
    */
    void Set_RTD_Alarm_Upper_Limit( byte wires, byte channel, long limit );
    
    /// Sets the lower alarm limit for an RTD channel.
    /** This will set the lower threshold for an alarm limit for the given wires, channel pair.
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \param limit The threshold value given as a analog-to-digital converter measurement value.
    */
    void Set_RTD_Alarm_Lower_Limit( byte wires, byte channel, long limit );
    
    /// Returns a byte representing with alarms are enabled.
    /** The first four bits returned by this function are meaningless. The four least-significant-bits represent the enabled state of 
        the alarms on the shield. When an alarm is enabled it will trigger an output transition on it's alarm pin. When an alarm is not
        enabled it's alarm pin will never change state.
        \return A byte representing which alarm pins are enabled on the shield.
    */
    byte Get_Alarm_Enables();
    
    /// Enables the given alarm.
    /** Enables the given alarm while leaving the enabled state of the other alarms unchanged. 
        \param alarm The alarm to enable: use 1 to enable alarm 1 (on pin D2), use 2 to enable alarm 2 (on pin D3), use 3 to enable alarm 3
            (on pin D4), use 4 to enable alarm 4 (on pin D5).
    */
    void Enable_Alarm( byte alarm );
    
    /// Disables the given alarm.
    /** Disables the given alarm while leaving the enabled state of the other alarms unchanged. 
        \param alarm The alarm to disable: use 1 to disable alarm 1 (on pin D2), use 2 to disable alarm 2 (on pin D3), use 3 to disable alarm 3
            (on pin D4), use 4 to disable alarm 4 (on pin D5).
    */
    void Disable_Alarm( byte alarm );
    
    /// Configure the RTD trigger for a given alarm channel.
    /** This sets the configuration for an RTD alarm. If `below` and `above` are both false, the alarm will trigger when the value is between
        the lower limit threshold and upper limit threshold. If `below` and `above` are both true, the alarm will trigger when the value is
        outside (meaning not between) the lower limit threshold and upper limit threshold.
        \param alarm The alarm to configure: alarm 1 is on pin D2, alarm 2 is on pin D3, alarm 3 is on pin D4, alarm 4 is on pin D5.
        \param below Set to true if you want an alarm when the value is below the lower limit threshold. 
        \param above Set to true if you want an alarm when the value is above the upper limit threshold.
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \sa Set_Alarm_Upper_Limit(), Set_Alarm_Lower_Limit()
    */
    void Configure_RTD_Alarm( byte alarm, boolean below, boolean above, byte wires, byte channel );
    
    /// Sets the alarm source.
    /** Each of the alarms can be triggered by the RTD value (as configured using Configure_RTD_Alarm()), by new data arriving
        on the RS232 port, or by new data arriving on the RS485 port. The alarm may also be triggered by multiple sources: by 
        setting the rtd and rs232 parameters to true the alarm will trigger for both conditions.
        \param alarm The alarm for which you want to set the source.
        \param rtd Set to true if you want the RTD value to trigger the alarm.
        \param rs232 Set to true if you want new data arriving on the RS232 port to trigger the alarm.
        \param rs485 Set to true if you want new data arriving on the RS485 port to trigger the alarm.
        \sa Configure_RTD_Alarm()
    */
    void Set_Alarm_Source( byte alarm, boolean rtd, boolean rs232, boolean rs485 );
    
    /// Returns the number of channels for the wire type.
    /** Returns 7 when wires is 2, 4 when wires is 3, 3 when wires is 4, and 0 for all other values.
        \param wires The wiring configuration of the RTD.
        \return The number of channels available when using the given RTD wiring.
    */
    byte Get_RTD_Channel_Limit( byte wires );
    
    /// The base function that supports the print and println functions.
    /** This is the method that enables the print and println functions.  It writes one character
        to the connected ports.
        \param data The character to write.
        \return The number of characters written.
        \sa Connect_Print_To()
    */
    size_t write( uint8_t data );
    
    /// Set where print functions output.
    /** This controls where print and println function are sent. If both parameters are set to true,
        then the print and println functions will result in the same output being sent to both ports.
        \param rs232 Set to true if you want the print and println functions to output to the RS232 port.
        \param rs485 Set to true if you want the print and println functions to output to the RS485 port.
    */
    void Connect_Print_To( boolean rs232, boolean rs485 );
    
    /// Get the shield's signature.
    /** The shield's signature is just a register value that is given the value 0xA6 = 166 = 0b10100110
        when the shield is powered on.  The signature can be overwritten by the Arduino by using I2C 
        library and writing to the address containing the signature, but the shield itseld does not 
        modify the signature value.  This value can be checked against the default value of 0xA6 as a 
        means to see if communication with the shield has been corrupted.
        \return The signature of the shield. 
        \sa Is_Signature_Ok()
    */
    byte Get_Signature();
    
    /// Checks the sheild's signature.
    /** This function checks the shield's signature against the default value as a means to see if 
        the communication with the shield has been corrupted.  The shield's signature value should 
        not change unless manipulated by direct, low-level writes by the Arduino to the shield. 
        The thinking is that if something goes wrong it is unlikely that a query of the shield's 
        signature will return the default value.  
        \param signature The value to check the shield's signature against.
        \return True if the shield's signature matches the expected value.
        \sa Get_Signature()
    */ 
    boolean Is_Signature_Ok( byte signature );
    
    /// The amount of time to wait for an I2C replay message.
    static const int PV_RTD_COMMUNICATION_TIMOUT_MS = 100;
    
    /// Returns the shield's Rbias resistor value.
    /** Rbias sets the reference voltage on the ADS1248.  This resistor value has changed with different versions of the shield.  This
        function returns the value that should be used for the Rbias resistor value for the RTD shield.
	\return The Rbias resistor value in ohms.
    */
    float Get_RTD_Rbias();



  private:
    
    /// Used to set the register to read from or write to.
    /** Set the active register on the shield.
        \param register_address The register address to make active.
        \return The return value from the I2C transmission.
    */
    int Set_Register( int register_address );
    
    /// Get the Idac and PGA settings.
    /** Returns a byte representing the configuration state of the Idac and PGA settings.
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \return A byte representing the Idac and PGA configuration.
    */
    byte Get_RTD_Idac_PGA_Configuration( byte wires, byte channel );
    
    /// Get the register that holds the Idac and PGA setting.
    /** Returns the register on the sheild that holds the Idac and PGA settings for the given wires, channel values.
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \param channel The RTD channel. This can be 1 through 7 for two-wire RTDs, 1 through 4 for three-wire RTDs, or 1 through 3 for four-wire RTDs.
        \return The register address for the Idac and PGA configuration values.
    */
    int Get_RTD_Idac_PGA_Register( byte wires, byte channel );
    
    /// Translate the UART configuration into a control byte.
    /** Translates the parameters into the byte value the shield expects for the given UART configuration.
        \param data_bits The data bits to use: must be either 8 or 9.
        \param parity The parity to use: must be either 'N' for None, 'O' (the capital-letter O) for Odd, or 'E' for Even.
        \param stop_bits The number of stop bits to use: must be either 1 or 2.
        \param polarity Setting this to true will invert the polarity on the receive line. The receive idle state is typically '1', setting
            this to 'true' will make the expected receive idle state '0'.
    */
    byte Get_UART_Configuration_Byte( byte data_bits, byte parity, byte stop_bits, boolean polarity );
    
    /// Returns a byte with bits representing which channels are enabled.
    /** When `wires` is 2 this returns the same value as Get_Enabled_Channels(). When `wires` is 3 or 4 the return value is the same: a
        byte where the low 4 bits represent the enabled states of the 3W channels, the most-significant-bit is zero, and the other bits
        retpresent the enabled state of the the 4W channels.
        \param wires 2 for a two-wire RTD, 3 for a three-wire RTD, 4 for a four-wire RTD.
        \return The channels that are enabled: the channels that will collect and process RTD measurements.
    */
    byte Get_Enabled_RTD_Channels_Byte( byte wires );
    


    /// The I2C address of the shield.
    int m_i2c_address;

    /// The characteristic resistance of the RTD sensor at 0 degrees Celisus
    float m_R0;
    
    /// True if print operations should be output on the RS232 port.
    boolean m_print_to_rs232;
    
    /// True if print operations should be output on the RS485 port.
    boolean m_print_to_rs485;
};


#endif
