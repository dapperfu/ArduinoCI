#include <Wire.h>
#include "PV_RTD_RS232_RS485_Shield.h"
#include "PV_RTD_RS232_RS485_Memory_Map.h"
#include <math.h>


PV_RTD_RS232_RS485::PV_RTD_RS232_RS485( byte i2c_address, float R0 ) {
  m_i2c_address = i2c_address;              // Set the I2C address
  m_R0 = R0;
}



void PV_RTD_RS232_RS485::Print_Registers() {
  int address = 0;    // The I2C register address
  byte data;          // The data at the I2C register address
  
  Serial.print( "\nRTD+RS232+RS485 Shield Registers:" );
  
  while( address < LAST_RAM_REGISTER ) {    // Loop through all of the registers
    Set_Register( address );                // Set the starting address to read from
    
    // BUFFER_LENGTH is #define'ed in Wire.h
    I2C_RTD_PORTNAME.requestFrom( m_i2c_address, BUFFER_LENGTH );    // Request data
    
    while( I2C_RTD_PORTNAME.available() ) {     // Loop through I2C receive buffer
      if( address % 10 == 0 ) {             // Formatting for address output
        Serial.print( "\n  " );             // Every 10 lines, start a new line
        if( address == 0 ) {                // Pad the address with spaces to align output
          Serial.print( "  " );
        } else if( address < 100 ) {
          Serial.print( " " );
        }
        Serial.print( address );            // Output address
        Serial.print( "  |  " );            // Address seperator
      }
      data = I2C_RTD_PORTNAME.read();           // Read data from I2C receive buffer
      if( data <= 0x0F ) {                  // Pad the data to align output
        Serial.print( "0" );
      }
      Serial.print( data, HEX );            // Output data
      Serial.print( " " );
      address++;                            // Increment address
      if( address > LAST_RAM_REGISTER ) {   // If we reach the end, let's stop
        break;
      }
    }
  }
  Serial.print( "\n\n" );                   // Trailing formatting  
}



int PV_RTD_RS232_RS485::Set_Register( int register_address ) {
  I2C_RTD_PORTNAME.beginTransmission( m_i2c_address );
  I2C_RTD_PORTNAME.write( register_address );
  return I2C_RTD_PORTNAME.endTransmission();
}



unsigned long PV_RTD_RS232_RS485::Get_RTD_ADC_Reading( byte wires, byte channel, int timeout_ms ) {
  unsigned long reading = 0;
  unsigned long start_time;
  unsigned reg;
  
  // Determine which register holds the value we seek
  switch( wires ) {
    case( 2 ):
      if( channel == 0 || channel > 7 ) return 0;
      reg = RTD_2W_CH1_MSB_ADDRESS + ( channel - 1 ) * 3;
      break;
    case( 3 ):
      if( channel == 0 || channel > 4 ) return 0;
      reg = RTD_3W_CH1_MSB_ADDRESS + ( channel - 1 ) * 3;
      break;
    case( 4 ):
      if( channel == 0 || channel > 3 ) return 0;
      reg = RTD_4W_CH1_MSB_ADDRESS + ( channel - 1 ) * 3;
      break;
    default:
      return 0;
  }
  
  Set_Register( reg );
  I2C_RTD_PORTNAME.requestFrom( m_i2c_address, 3 );    // Get the 24-bit (3-byte) value
  
  for( int i = 0; i < 3; i++ ) {
    start_time = millis();
    while( !I2C_RTD_PORTNAME.available() && millis() < start_time + timeout_ms );
    reading |= I2C_RTD_PORTNAME.read();                // Get byte
    if( i != 2 ) {
      reading <<= 8;                                   // Shift byte up
    }
  }
  
  return reading;
}



int PV_RTD_RS232_RS485::Disable_All_RTD_Channels() {
  I2C_RTD_PORTNAME.beginTransmission( m_i2c_address );
  I2C_RTD_PORTNAME.write( RTD_2W_ENABLE_ADDRESS );
  I2C_RTD_PORTNAME.write( 0x00 );      // This disables all 2W RTD channels
  I2C_RTD_PORTNAME.write( 0x00 );      // This disables all 3W & 4W RTD channels
  return I2C_RTD_PORTNAME.endTransmission();
}



byte PV_RTD_RS232_RS485::Get_Enabled_RTD_Channels( byte wires ) {
  if( wires < 2 || wires > 4 ) return 0;
  
  switch( wires ) {
    case( 2 ):
      return Read_Register( RTD_2W_ENABLE_ADDRESS );
      break;
    case( 3 ):
      // Mask out the 4W enabled channels
      return Read_Register( RTD_3W_4W_ENABLE_ADDRESS ) & RTD_3W_BITS;
      break;
    case( 4 ):
      // Shift down to eliminate the 3W enabled channels
      return Read_Register( RTD_3W_4W_ENABLE_ADDRESS ) >> 4;
      break;
    default:
      return 0;
  }
  
  return 0;
}



byte PV_RTD_RS232_RS485::Get_Enabled_RTD_Channels_Byte( byte wires ) {
  if( wires < 2 || wires > 4 ) return 0;
  
  switch( wires ) {
    case( 2 ):
      return Read_Register( RTD_2W_ENABLE_ADDRESS );
      break;
    case( 3 ):
      // Mask out the 4W enabled channels
      return Read_Register( RTD_3W_4W_ENABLE_ADDRESS );
      break;
    case( 4 ):
      // Shift down to eliminate the 3W enabled channels
      return Read_Register( RTD_3W_4W_ENABLE_ADDRESS );
      break;
    default:
      return 0;
  }
  
  return 0;
}



boolean PV_RTD_RS232_RS485::Is_Valid_RTD_Channel( byte wires, byte channel ) {
  boolean valid = false;
  switch( wires ) {
    case( 2 ):
      valid = ( ( channel != 0 ) && ( channel <= 7 ) );
      break;
    case( 3 ):
      valid = ( ( channel != 0 ) && ( channel <= 4 ) );
      break;
    case( 4 ):
      valid = ( ( channel != 0 ) && ( channel <= 3 ) );
      break;
  }
  
  #ifdef RTD_DEBUG
    if( !valid ) {
      Serial.print( "Is_Valid_Channel( " );
      Serial.print( wires );
      Serial.print( ", " );
      Serial.print( channel );
      Serial.print( " ) call has an invalid wires, channel pair\n" );
    }
  #endif
  
  return valid;
}



int PV_RTD_RS232_RS485::Enable_RTD_Channel( byte wires, byte channel ) {
  if( !Is_Valid_RTD_Channel( wires, channel ) ) {
    return 0;
  }
  
  byte enabled_channels = Get_Enabled_RTD_Channels_Byte( wires );
  byte channel_enable_bit = 1;
  channel_enable_bit <<= ( channel - 1 );
  if( wires == 4 ) {
    channel_enable_bit <<= 4;
  }
  
  if( channel_enable_bit & enabled_channels ) {
    // Channel is already enabled
    return 0;
  }
  
  enabled_channels |= channel_enable_bit;
  
  I2C_RTD_PORTNAME.beginTransmission( m_i2c_address );
  
  switch( wires ) {
    case( 2 ):
      I2C_RTD_PORTNAME.write( RTD_2W_ENABLE_ADDRESS );
      break;
    case( 3 ):
    case( 4 ):
      I2C_RTD_PORTNAME.write( RTD_3W_4W_ENABLE_ADDRESS );
      break;
    default:
      I2C_RTD_PORTNAME.endTransmission();
      return 0;
  }
  
  I2C_RTD_PORTNAME.write( enabled_channels );  
  
  return I2C_RTD_PORTNAME.endTransmission();
}



float PV_RTD_RS232_RS485::Get_RTD_Idac( byte wires, byte channel ) {
  if( !Is_Valid_RTD_Channel( wires, channel ) ) {
    return 0.0/0.0;
  }
  
  byte config = Get_RTD_Idac_PGA_Configuration( wires, channel );
  
  if( config == 0xFF ) {
    return 0.0/0.0;
  }
  
  config &= RTD_IDAC_BITS;
  
  switch( config ) {
    case( 0b000 ):
      return 0.0;
    case( 0b001 ):
      return 50E-6;
    case( 0b010 ):
      return 100E-6;
    case( 0b011 ):
      return 250E-6;
    case( 0b100 ):
      return 500E-6;
    case( 0b101 ):
      return 750E-6;
    case( 0b110 ):
      return 1000E-6;
    case( 0b111 ):
      return 1500E-6;
    default:
      return 0.0/0.0;
  }
    
  return 0.0/0.0;
}



float PV_RTD_RS232_RS485::Set_RTD_Idac( byte wires, byte channel, float new_idac_value ) {

  #ifdef RTD_DEBUG
    Serial.print( "Set_Idac( " );
    Serial.print( wires );
    Serial.print( ", " );
    Serial.print( channel );
    Serial.print( ", " );
    Serial.print( new_idac_value, 6 );
    Serial.print( " )\n" );
  #endif
  
  if( !Is_Valid_RTD_Channel( wires, channel ) ) {
    return 0.0/0.0;
  }
    
  byte idac_pga_value = Get_RTD_Idac_PGA_Configuration( wires, channel );
  byte new_idac_pga_value = idac_pga_value & RTD_PGA_BITS;    // Set the Idac bits to zeros
  
  if( new_idac_value == 0.0 ) {
    // new_idac_pga_value |= 0b000;
  } else if( new_idac_value == 50E-6f ) {
    new_idac_pga_value |= 0b001;
  } else if( new_idac_value == 100E-6f ) {
    new_idac_pga_value |= 0b010;
  } else if( new_idac_value == 250E-6f ) {
    new_idac_pga_value |= 0b011;
  } else if( new_idac_value == 500E-6f ) {
    new_idac_pga_value |= 0b100;
  } else if( new_idac_value == 750E-6f ) {
    new_idac_pga_value |= 0b101;
  } else if( new_idac_value == 1000E-6f ) {
    new_idac_pga_value |= 0b110;
  } else if( new_idac_value == 1500E-6f ) {
    new_idac_pga_value |= 0b111;
  } else {
    return Get_RTD_Idac( wires, channel );
  }
  
  #ifdef RTD_DEBUG
    Serial.print( "  " );
    Serial.print( idac_pga_value, HEX );
    Serial.print( " => " );
    Serial.print( new_idac_pga_value, HEX );
  #endif
  
  byte register_address = Get_RTD_Idac_PGA_Register( wires, channel );
  if( register_address != 0xFF && idac_pga_value != new_idac_pga_value ) {
    Write_Register( register_address, new_idac_pga_value );
  }
  
  return Get_RTD_Idac( wires, channel );
}



float PV_RTD_RS232_RS485::Get_RTD_Rbias() {
	switch( Get_Signature() ) {
		case( 166 ):
			return 833.3449447008621;
		case( 167 ):
			return 4300.0;
	}
	return 0.0;
}



float PV_RTD_RS232_RS485::Get_RTD_Vref( byte wires, byte channel ) {
  if( wires == 3 ) {
    return Get_RTD_Rbias() * Get_RTD_Idac( wires, channel ) * 2.0;
  } else {
    return Get_RTD_Rbias() * Get_RTD_Idac( wires, channel );
  }
}



int PV_RTD_RS232_RS485::Get_RTD_Idac_PGA_Register( byte wires, byte channel ) {
  if( !Is_Valid_RTD_Channel( wires, channel ) ) {
    return 0xFF;
  }
  
  byte register_address;
  switch( wires ) {
    case( 2 ):
      register_address = RTD_2W_CH1_IDAC_PGA_ADDRESS + ( channel - 1 );
      break;
    case( 3 ):
      register_address = RTD_3W_CH1_IDAC_PGA_ADDRESS + ( channel - 1 );
      break;
    case( 4 ):
      register_address = RTD_4W_CH1_IDAC_PGA_ADDRESS + ( channel - 1 );
      break;
    default:
      return 0xFF;
      break;
  }
  
  return register_address;
}



byte PV_RTD_RS232_RS485::Get_RTD_Idac_PGA_Configuration( byte wires, byte channel ) {
  if( !Is_Valid_RTD_Channel( wires, channel ) ) {
    return 0xFF;
  }
  
  byte register_address = Get_RTD_Idac_PGA_Register( wires, channel );
  return Read_Register( register_address );
}



float PV_RTD_RS232_RS485::Get_RTD_PGA( byte wires, byte channel ) {
  if( !Is_Valid_RTD_Channel( wires, channel ) ) {
    return 0.0/0.0;
  }
  
  byte config = Get_RTD_Idac_PGA_Configuration( wires, channel );
  
  if( config == 0xFF ) {
    return 0.0/0.0;
  }
  
  config &= RTD_PGA_BITS;
  config >>= 4;
  
  switch( config ) {
    case( 0b000 ):
      return 1.0;
    case( 0b001 ):
      return 2.0;
    case( 0b010 ):
      return 4.0;
    case( 0b011 ):
      return 8.0;
    case( 0b100 ):
      return 16.0;
    case( 0b101 ):
      return 32.0;
    case( 0b110 ):
      return 64.0;
    case( 0b111 ):
      return 128.0;
    default:
      return 0.0/0.0;
  }
    
  return 0.0/0.0;
}



float PV_RTD_RS232_RS485::Set_RTD_PGA( byte wires, byte channel, byte new_gain_value ) {
  if( !Is_Valid_RTD_Channel( wires, channel ) ) {
    return 0.0/0.0;
  }
  
  int register_address = Get_RTD_Idac_PGA_Register( wires, channel );
  byte idac_pga = Get_RTD_Idac_PGA_Configuration( wires, channel );
  byte new_idac_pga = idac_pga & RTD_IDAC_BITS;         // Mask out the PGA bits
  
  switch( new_gain_value ) {
    case( 1 ):
      break;
    case( 2 ):
      new_idac_pga |= 0b0010000;
      break;
    case( 4 ):
      new_idac_pga |= 0b0100000;
      break;
    case( 8 ):
      new_idac_pga |= 0b0110000;
      break;
    case( 16 ):
      new_idac_pga |= 0b1000000;
      break;
    case( 32 ):
      new_idac_pga |= 0b1010000;
      break;
    case( 64 ):
      new_idac_pga |= 0b1100000;
      break;
    case( 128 ):
      new_idac_pga |= 0b1110000;
      break;
    default:
      return Get_RTD_PGA( wires, channel );
  }
  
  if( idac_pga != new_idac_pga ) {
    Write_Register( register_address, new_idac_pga );
  }
  
  return Get_RTD_PGA( wires, channel );
}



unsigned int PV_RTD_RS232_RS485::Get_RTD_SPS() {
  byte sps_register = Read_Register( RTD_SPS_ADDRESS );
  switch( sps_register ) {
    case( 0x00 ):
      return 5;
      break;
    case( 0x01 ):
      return 10;
      break;
    case( 0x02 ):
      return 20;
      break;
    case( 0x03 ):
      return 40;
      break;
    case( 0x04 ):
      return 80;
      break;
    case( 0x05 ):
      return 160;
      break;
    case( 0x06 ):
      return 320;
      break;
    case( 0x07 ):
      return 640;
      break;
    case( 0x08 ):
      return 1000;
      break;
    default:
      return 2000;
      break;
  }
}



unsigned int PV_RTD_RS232_RS485::Set_RTD_SPS( unsigned int new_sps_value ) {
  unsigned int sps_value = ((unsigned int)(Get_RTD_SPS()));

  if( sps_value != new_sps_value ) {
    switch( new_sps_value ) {
      case(    5 ):
	Write_Register( RTD_SPS_ADDRESS, 0x00 );
	break;
      case(   10 ):
	Write_Register( RTD_SPS_ADDRESS, 0x01 );
	break;
      case(   20 ):
	Write_Register( RTD_SPS_ADDRESS, 0x02 );
	break;
      case(   40 ):
	Write_Register( RTD_SPS_ADDRESS, 0x03 );
	break;
      case(   80 ):
	Write_Register( RTD_SPS_ADDRESS, 0x04 );
	break;
      case(  160 ):
	Write_Register( RTD_SPS_ADDRESS, 0x05 );
	break;
      case(  320 ):
	Write_Register( RTD_SPS_ADDRESS, 0x06 );
	break;
      case(  640 ):
	Write_Register( RTD_SPS_ADDRESS, 0x07 );
	break;
      case( 1000 ):
	Write_Register( RTD_SPS_ADDRESS, 0x08 );
	break;
      case( 2000 ):
	Write_Register( RTD_SPS_ADDRESS, 0x09 );
	break;
    }
  }

  return Get_RTD_SPS();
}

float PV_RTD_RS232_RS485::Get_RTD_Voltage( byte wires, byte channel ) {
  return Get_RTD_ADC_Reading( wires, channel ) * Get_RTD_Bit_Weight( wires, channel );
}



float PV_RTD_RS232_RS485::Get_RTD_Resistance( byte wires, byte channel ) {
  return Get_RTD_Voltage( wires, channel ) / Get_RTD_Idac( wires, channel );
}



float PV_RTD_RS232_RS485::Get_RTD_Bit_Weight( byte wires, byte channel ) {
  return ( Get_RTD_Vref( wires, channel ) / Get_RTD_PGA( wires, channel ) ) / 8388607.0;
}



#define RTD_CVD_A      3.9083E-3                ///< The A coefficient from the Callendar-Van Dusen RTD equation
#define RTD_CVD_B      -5.775E-7                ///< The B coefficient from the Callendar-Van Dusen RTD equation
#define RTD_QUAD_A     (m_R0*RTD_CVD_B)       ///< The A coeffieicnt from the quadratic equation for the temperature
#define RTD_QUAD_2A    (2.0*RTD_QUAD_A)         ///< 2 x A
#define RTD_QUAD_B     (m_R0*RTD_CVD_A)         ///< The B coefficient from the quadratic equation for the temperature
#define RTD_QUAD_B2    (RTD_QUAD_B*RTD_QUAD_B)  ///< B^2
#define RTD_INV_CVD_A  1.70177628E-08           ///< The A coefficient to use when going from R to T when T < 0 degC
#define RTD_INV_CVD_B  -9.89375839E-06	        ///< The B coefficient to use when going from R to T when T < 0 degC
#define RTD_INV_CVD_C  2.85155075E-03           ///< The C coefficient to use when going from R to T when T < 0 degC
#define RTD_INV_CVD_D  2.21637862E+00           ///< The D coefficient to use when going from R to T when T < 0 degC
#define RTD_INV_CVD_E  -2.41963011E+02          ///< The E coefficient to use when going from R to T when T < 0 degC



float PV_RTD_RS232_RS485::Get_RTD_Temperature_degC( byte wires, byte channel ) {
  float rt = Get_RTD_Resistance( wires, channel );
  float rt2, rt3, rt4;
  
  if( rt < m_R0 ) {
    // Use a fitted curve to get the temperature
    rt  = rt / ( m_R0 / 100.0 );
    rt2 = rt * rt;
    rt3 = rt2 * rt;
    rt4 = rt3 * rt;
    return RTD_INV_CVD_A * rt4 + RTD_INV_CVD_B * rt3 + RTD_INV_CVD_C * rt2 + RTD_INV_CVD_D * rt + RTD_INV_CVD_E;
  } else {
    // Solve the quadratic equation to get the temperature
    float RTD_QUAD_C = m_R0 - rt;
    return ( -RTD_QUAD_B + sqrt( RTD_QUAD_B2 - 2.0 * RTD_QUAD_2A * RTD_QUAD_C ) ) / ( RTD_QUAD_2A );
  }
}



float PV_RTD_RS232_RS485::Get_RTD_Temperature_degF( byte wires, byte channel ) {
  return Get_RTD_Temperature_degC( wires, channel ) * 1.8 + 32.0;
}



float PV_RTD_RS232_RS485::Get_RTD_Temperature_K( byte wires, byte channel ) {
  return Get_RTD_Temperature_degC( wires, channel ) + 273.15;
}	



float PV_RTD_RS232_RS485::Get_RTD_Temperature_degR( byte wires, byte channel ) {
  return ( Get_RTD_Temperature_degC( wires, channel ) + 273.15 ) * 1.8;
}



byte PV_RTD_RS232_RS485::Read_Register( int register_address ) {
  Set_Register( register_address );
  I2C_RTD_PORTNAME.requestFrom( m_i2c_address, 1 );
  return I2C_RTD_PORTNAME.read();
}



int PV_RTD_RS232_RS485::Write_Register( int register_address, byte data ) {
  #ifdef RTD_DEBUG
    Serial.print( "  Write_Register( 0x" );
    Serial.print( register_address );
    Serial.print( ", 0x" );
    Serial.print( data, HEX );
    Serial.print( " )\n" );
  #endif
  I2C_RTD_PORTNAME.beginTransmission( m_i2c_address );
  I2C_RTD_PORTNAME.write( register_address );
  I2C_RTD_PORTNAME.write( data );
  return I2C_RTD_PORTNAME.endTransmission();
}



int PV_RTD_RS232_RS485::Write_RS232( byte data ) {
  return Write_Register( RS232_TX_BUFFER_ADDRESS, data );
}



int PV_RTD_RS232_RS485::Write_RS485( byte data ) {
  return Write_Register( RS485_TX_BUFFER_ADDRESS, data );
}



byte PV_RTD_RS232_RS485::Read_RS232() {
  return Read_Register( RS232_RX_BUFFER_ADDRESS );
}



byte PV_RTD_RS232_RS485::Read_RS485() {
  return Read_Register( RS485_RX_BUFFER_ADDRESS );  
}



boolean PV_RTD_RS232_RS485::Has_RS232_Data() {
  return Read_Register( RS232_STATUS_ADDRESS );
}



boolean PV_RTD_RS232_RS485::Has_RS485_Data() {
  return Read_Register( RS485_STATUS_ADDRESS );
}



byte PV_RTD_RS232_RS485::Get_UART_Configuration_Byte( byte data_bits, byte parity, byte stop_bits, boolean polarity ) {
  byte config = 0;
  
  if( polarity ) {
    config |= UART_POLARITY_BIT;
  }
  
  switch( stop_bits ) {
    case( 1 ):
      break;
    case( 2 ):
      config |= UART_STOP_BIT;
      break;
    default:
      Serial.println( "RTD+RS232+RS485 Shield: Bad stop bits" );
      return 0xFF;
  }
  
  if( ( data_bits == 9 && parity == 'N' ) || ( data_bits == 9 && parity == 'n' ) ) {
    config |= UART_PARITY_DATA_BITS;
  } else {
    if( data_bits == 8 ) {
      switch( parity ) {
        case( 'N' ):
        case( 'n' ):
          // config |= 0b0000;
          break;
        case( 'O' ):
        case( 'o' ):
          config |= 0b0100;
          break;
        case( 'E' ):
        case( 'e' ):
          config |= 0b0010;
          break;
        default:
          Serial.println( "RTD+RS232+RS485 Shield: Bad parity" );
          return 0xFF;
      }
    } else {
      Serial.println( "RTD+RS232+RS485 Shield: Bad data bits" );
      return 0xFF;
    }
  }

  return config;
}



boolean PV_RTD_RS232_RS485::Set_RS232_Configuration( unsigned long baud, byte data_bits, byte parity, byte stop_bits, boolean polarity ) {
  byte config = Get_UART_Configuration_Byte( data_bits, parity, stop_bits, polarity );
  
  #ifdef RTD_DEBUG
    Serial.print( "Set_RS232_Configuration( " );
    Serial.print( baud );
    Serial.print( ", " );
    Serial.print( data_bits );
    Serial.print( ", " );
    Serial.print( parity );
    Serial.print( ", " );
    Serial.print( stop_bits );
    Serial.print( ", " );
    Serial.print( polarity );
    Serial.print( " ) resulted in a configuration byte of 0x" );
    Serial.println( config, HEX );
  #endif
  
  if( config == 0xFF ) {
    return false;
  }
  
  byte baud_msb, baud_csb, baud_lsb;
  baud_lsb = (byte)baud;
  baud >>= 8;
  baud_csb = (byte)baud;
  baud >>= 8;
  baud_msb = (byte)baud;
  
  Write_Register( RS232_CONFIG_ADDRESS, config );
  Write_Register( RS232_BAUD_MSB_ADDRESS, baud_msb );
  Write_Register( RS232_BAUD_CSB_ADDRESS, baud_csb );
  Write_Register( RS232_BAUD_LSB_ADDRESS, baud_lsb );
  
  return true;
}



boolean PV_RTD_RS232_RS485::Set_RS485_Configuration( unsigned long baud, byte data_bits, byte parity, byte stop_bits, boolean polarity ) {
  byte config = Get_UART_Configuration_Byte( data_bits, parity, stop_bits, polarity );
  if( config == 0xFF ) {
    return false;
  }
  
  byte baud_msb, baud_csb, baud_lsb;
  baud_lsb = (byte)baud;
  baud >>= 8;
  baud_csb = (byte)baud;
  baud >>= 8;
  baud_msb = (byte)baud;
  
  Write_Register( RS485_CONFIG_ADDRESS, config );
  Write_Register( RS485_BAUD_MSB_ADDRESS, baud_msb );
  Write_Register( RS485_BAUD_CSB_ADDRESS, baud_csb );
  Write_Register( RS485_BAUD_LSB_ADDRESS, baud_lsb );
  
  return true;
}



void PV_RTD_RS232_RS485::Set_RTD_Alarm_Upper_Limit( byte wires, byte channel, long limit ) {
  if( !Is_Valid_RTD_Channel( wires, channel ) ) return;
  
  int register_address;
  switch( wires ) {
    case( 2 ):
      register_address = RTD_2W_CH1_HI_LIMIT_MSB_ADDRESS + ( channel - 1 ) * 3;
      break;
    case( 3 ):
      register_address = RTD_3W_CH1_HI_LIMIT_MSB_ADDRESS + ( channel - 1 ) * 3;
      break;
    case( 4 ):
      register_address = RTD_4W_CH1_HI_LIMIT_MSB_ADDRESS + ( channel - 1 ) * 3;
      break;
    default:
      return;
  }
  
  byte limit_lsb = (byte)limit;
  limit >>= 8;
  byte limit_csb = (byte)limit;
  limit >>= 8;
  byte limit_msb = (byte)limit;
  
  I2C_RTD_PORTNAME.beginTransmission( m_i2c_address );
  I2C_RTD_PORTNAME.write( register_address );
  I2C_RTD_PORTNAME.write( limit_msb );
  I2C_RTD_PORTNAME.write( limit_csb );
  I2C_RTD_PORTNAME.write( limit_lsb );
  I2C_RTD_PORTNAME.endTransmission();
}



void PV_RTD_RS232_RS485::Set_RTD_Alarm_Lower_Limit( byte wires, byte channel, long limit ) {
  if( !Is_Valid_RTD_Channel( wires, channel ) ) return;
  
  int register_address;
  switch( wires ) {
    case( 2 ):
      register_address = RTD_2W_CH1_LO_LIMIT_MSB_ADDRESS + ( channel - 1 ) * 3;
      break;
    case( 3 ):
      register_address = RTD_3W_CH1_LO_LIMIT_MSB_ADDRESS + ( channel - 1 ) * 3;
      break;
    case( 4 ):
      register_address = RTD_4W_CH1_LO_LIMIT_MSB_ADDRESS + ( channel - 1 ) * 3;
      break;
    default:
      return;
  }
  
  byte limit_lsb = (byte)limit;
  limit >>= 8;
  byte limit_csb = (byte)limit;
  limit >>= 8;
  byte limit_msb = (byte)limit;
  
  I2C_RTD_PORTNAME.beginTransmission( m_i2c_address );
  I2C_RTD_PORTNAME.write( register_address );
  I2C_RTD_PORTNAME.write( limit_msb );
  I2C_RTD_PORTNAME.write( limit_csb );
  I2C_RTD_PORTNAME.write( limit_lsb );
  I2C_RTD_PORTNAME.endTransmission();  
}



byte PV_RTD_RS232_RS485::Get_Alarm_Enables() {
  return Read_Register( IRQ_ENABLE_ADDRESS );
}



void PV_RTD_RS232_RS485::Enable_Alarm( byte alarm ) {
  if( alarm == 0 || alarm > 4 ) return;

  byte mask = 1;
  byte config = Get_Alarm_Enables();
  
  mask <<= ( alarm - 1 );
  
  if( mask & config ) {
    return;
  } else {
    I2C_RTD_PORTNAME.beginTransmission( m_i2c_address );
    I2C_RTD_PORTNAME.write( IRQ_ENABLE_ADDRESS );
    I2C_RTD_PORTNAME.write( mask | config );
    I2C_RTD_PORTNAME.endTransmission();
  }
}



void PV_RTD_RS232_RS485::Disable_Alarm( byte alarm ) {
  if( alarm == 0 || alarm > 4 ) return;

  byte mask = 1;
  byte config = Get_Alarm_Enables();
  
  mask <<= ( alarm - 1 );
  
  if( mask & ~config ) {
    return;
  } else {
    I2C_RTD_PORTNAME.beginTransmission( m_i2c_address );
    I2C_RTD_PORTNAME.write( IRQ_ENABLE_ADDRESS );
    I2C_RTD_PORTNAME.write( ~mask & config );
    I2C_RTD_PORTNAME.endTransmission();
  }
}



void PV_RTD_RS232_RS485::Configure_RTD_Alarm( byte alarm, boolean below, boolean above, byte wires, byte channel ) {
  if( alarm == 0 || alarm > 4 ) return;
  if( !Is_Valid_RTD_Channel( wires, channel ) ) return;
  
  byte register_address = IRQ1_CONFIG_ADDRESS + ( alarm - 1 );
  byte config = Read_Register( register_address );
  byte rtd_config = config & 0b11001111;
  
  byte request = 0;
  if( above ) request |= 0b10000000;
  if( below ) request |= 0b01000000;
  if( wires == 3 ) request |= 0b00001000;
  if( wires == 4 ) request |= 0b00001100;
  request |= channel;
    
  if( request == rtd_config ) 
    return;
  else {
    I2C_RTD_PORTNAME.beginTransmission( m_i2c_address );
    I2C_RTD_PORTNAME.write( register_address );
    I2C_RTD_PORTNAME.write( request | ( config & 0b00110000 ) );
    I2C_RTD_PORTNAME.endTransmission();
  }
}



void PV_RTD_RS232_RS485::Set_Alarm_Source( byte alarm, boolean rtd, boolean rs232, boolean rs485 ) {
  if( alarm > 4 || alarm == 0 ) {
    return;
  }
  
  int alarm_config_register_address = IRQ1_CONFIG_ADDRESS + ( alarm - 1 );
  byte alarm_config = Read_Register( alarm_config_register_address );
  byte new_alarm_config = alarm_config;
  
  if( rs232 ) {
    new_alarm_config |= RS232_IRQ_BIT;
  } else {
    new_alarm_config &= ~RS232_IRQ_BIT;
  }
  
  if( rs485 ) {
    new_alarm_config |= RS485_IRQ_BIT;
  } else {
    new_alarm_config &= ~RS485_IRQ_BIT;
  }
  
  if( alarm_config != new_alarm_config ) {
    Write_Register( alarm_config_register_address, new_alarm_config );
  }
  
  byte alarm_enables = Get_Alarm_Enables();
  byte rtd_alarm_enables = alarm_enables & 0xF0;
  byte new_rtd_alarm_enables = rtd_alarm_enables;
  if( rtd ) {
    switch( alarm ) {
      case( 1 ):
        new_rtd_alarm_enables |= IRQ1_RTD_ALARM_ENABLE_BIT;
        break;
      case( 2 ):
        new_rtd_alarm_enables |= IRQ2_RTD_ALARM_ENABLE_BIT;
        break;
      case( 3 ):
        new_rtd_alarm_enables |= IRQ3_RTD_ALARM_ENABLE_BIT;
        break;
      case( 4 ):
        new_rtd_alarm_enables |= IRQ4_RTD_ALARM_ENABLE_BIT;
        break;
    }
  } else {
    switch( alarm ) {
      case( 1 ):
        new_rtd_alarm_enables &= ~IRQ1_RTD_ALARM_ENABLE_BIT;
        break;
      case( 2 ):
        new_rtd_alarm_enables &= ~IRQ2_RTD_ALARM_ENABLE_BIT;
        break;
      case( 3 ):
        new_rtd_alarm_enables &= ~IRQ3_RTD_ALARM_ENABLE_BIT;
        break;
      case( 4 ):
        new_rtd_alarm_enables &= ~IRQ4_RTD_ALARM_ENABLE_BIT;
        break;
    }
  }
  
  if( rtd_alarm_enables != new_rtd_alarm_enables ) {
    Write_Register( IRQ_ENABLE_ADDRESS, new_rtd_alarm_enables | ( alarm_enables & 0x0F ) );
  }
}



byte PV_RTD_RS232_RS485::Get_RTD_Channel_Limit( byte wires ) {
  switch( wires ) {
    case( 2 ): return 7;
    case( 3 ): return 4;
    case( 4 ): return 3;
    default: return 0;
  }
}



void PV_RTD_RS232_RS485::Connect_Print_To( boolean rs232, boolean rs485 ) {
  m_print_to_rs232 = rs232;
  m_print_to_rs485 = rs485;
}



size_t PV_RTD_RS232_RS485::write( uint8_t data ){
  if( m_print_to_rs232 ) {
    Write_RS232( data );
  }
  if( m_print_to_rs485 ) {
    Write_RS485( data );
  }
  if( m_print_to_rs232 || m_print_to_rs485 ) {
    return 1;
  } 
  return 0;
}



void PV_RTD_RS232_RS485::Reset() {
  Write_Register( RESET_ADDRESS, 0x01 );
}



void PV_RTD_RS232_RS485::Factory_Reset() {
  Write_Register( RESET_ADDRESS, 0xFF );
  delay( 500 );
}



byte PV_RTD_RS232_RS485::Get_Signature() {
	return Read_Register( SIGNATURE_ADDRESS );
}



boolean PV_RTD_RS232_RS485::Is_Signature_Ok( byte signature ) {
	return Get_Signature() == signature;
}
