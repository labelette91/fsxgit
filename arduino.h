#ifndef ARDUINO_H 
#define ARDUINO_H

#define SWITCH_NB_CAR 2 

//current digit values 
#define MAX_DIGIT 255
byte Digits[MAX_DIGIT];


// Bit 7   = switch value
//bit 6..0 : switch number 0..127
#if SWITCH_NB_CAR == 1
int GetSwitchValue(byte * Buffer)
{
	return ((Buffer[0] & 0x80)!=0) ;
}
int GetSwitchNumber(byte * Buffer)
{
	return (Buffer[0] & 0x7F) ;
}
#else

int GetSwitchValue(byte * Buffer)
{
	return ((char)Buffer[1] ) ;
}
int GetSwitchNumber(byte * Buffer)
{
	return (Buffer[0] ) ;
}
#endif

//SwNumber : 0..127
//SwValue 0..1
void SetSwitchValue(byte * Buffer , byte SwNumber , byte SwValue  )
{
  if (SwValue)
	  Buffer[0] = (byte)0x80 ;
  else
	  Buffer[0] = 0   ;
  Buffer[0] |= SwNumber ;

}
//cmd   :  commande bit 7..5
//DP    :  draw point bit 4
//value :  value    bit 3..1
//OutputNumber 0..127

#define PRESENCE_CMD        (0x00|0x80)
//Led output
#define IOCARD_OUT_CMD      ((1<<5)|0x80)
//display output
#define IOCARD_DISPLAY_CMD  ((2<<5)|0x80)
//encoder define cmd
#define IOCARD_ENCODER_CMD  ((3<<5)|0x80)

#define Segment_DP   0x10 

void SetOutputCmd ( byte * Buffer , byte cmd,byte OutputNumber , byte Value )
{
  Buffer[0] = cmd | Value ;
  Buffer[1] = OutputNumber & 0x7f ;
}
void SendOutputCmd ( byte cmd,byte OutputNumber , byte Value )
{
    byte Buffer[16];
    SetOutputCmd ( Buffer , cmd , OutputNumber ,  Value ) ;
    int NbWr = Com.WriteAsync(Buffer, 2) ;
		if (cmd==IOCARD_OUT_CMD)
			Console->debugPrintf (  TRACE_RS232_SEND , "RS232:Send IOCARD_OUT     Cmd:%02x Output:%3d(0x%2X) Value:%x Buf:%02x%02x\n",cmd,OutputNumber,OutputNumber,Value, Buffer[0] , Buffer[1]  );
		else if (cmd==IOCARD_DISPLAY_CMD)
			Console->debugPrintf (  TRACE_RS232_SEND , "RS232:Send IOCARD_DISPLAY Cmd:%02x Output:%3d(0x%2X) Value:%x Buf:%02x%02x\n",cmd,OutputNumber,OutputNumber,Value, Buffer[0] , Buffer[1]  );
		else if (cmd==PRESENCE_CMD)
			Console->debugPrintf (  TRACE_RS232_SEND , "RS232:Send PRESENCE_CMD   Cmd:%02x Output:%3d(0x%2X) Value:%x Buf:%02x%02x\n",cmd,OutputNumber,OutputNumber,Value, Buffer[0] , Buffer[1]  );
		else
			Console->debugPrintf (  TRACE_RS232_SEND , "RS232:Send IOCARD_ENCODER Cmd:%02x Output:%3d(0x%2X) Value:%x Buf:%02x%02x\n",cmd,OutputNumber,OutputNumber,Value, Buffer[0] , Buffer[1]  );

}

void SendDigitCmd (byte digitNb , byte digitValue , bool DrawPopint=false )
{
  if (DrawPopint)
    digitValue|= Segment_DP;
	if (Digits[digitNb]!=digitValue)
	{
		SendOutputCmd ( IOCARD_DISPLAY_CMD , digitNb ,  digitValue ) ;

		Digits[digitNb]=digitValue ;
	}
}

void ArduinoInit()
{
  	memset(Digits,-1,sizeof(Digits));
}
#endif