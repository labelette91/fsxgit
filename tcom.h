#ifndef tcommH
#define tcommH

#include <string>

class  TComm 
{

public:

  std::string  DeviceName ;
  int DeviceNum ;
  int  BaudRate ;
  char Parity ;
  int Stopbits  ;
  int Databits ;

	
public:
virtual 	bool __fastcall Enabled(void){return (false);};
virtual 	int __fastcall Open(void){return 0 ;};
virtual 	void __fastcall Close(void){};
virtual 	int __fastcall Write(const void *Buf, int Count){return 0 ;};
virtual 	int __fastcall Read(void *Buf, int Count){return 0 ;};
virtual     int __fastcall ReadStr ( int nbcar , char carend , char  *Apt_string ){return 0 ;};
virtual     void __fastcall Clear ()=0;
virtual     void setTimeouts ( int readTimeOut ,int writeTimeOut)=0 ;

};

#endif
