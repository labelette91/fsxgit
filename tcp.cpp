//---------------------------------------------------------------------------

#pragma hdrstop

//---------------------------------------------------------------------------

 #include <winsock2.h>
// #include <windows.h>
#include <conio.h>

#include <stdio.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "fctlib.h"
#include "tcomas2.h"
#include "as2.h"
#include "console.h"
using namespace std;


//utilisation de sioc
//#define USE_SIOC 1

#ifndef __TURBOC__
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
//#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "Winmm.lib")

#endif

#define TRACE_RS232_SEND     (1l<<0)
#define TRACE_RS232_RECV     (1l<<1)

#define TRACE_SIOC_SEND      (1l<<2)
#define TRACE_SIOC_RECV      (1l<<3)

#define TRACE_FSX_SEND      (1l<<2)
#define TRACE_FSX_RECV      (1l<<3)

#define TRACE_SIOC      (1l<<4)
#define TRACE_EVENT     (1l<<5)
#define TRACE_OFFSET    (1l<<6)

#define TRACE_RS232_RECV_PRESENCE     (1l<<8)

TCommAs2 Com ;

#include "thread.h"

#include "ClientTcp.h"

ClientTcp * Tcp ;

#include "FsuipcOffset.h"

#include "SiocVar.h"

extern void RefreshOutput (int Variable , double value );
extern void SendSwitchValuesToFsx();

#include "pmdg.h"

std::string ServerIp = "127.0.0.1";
int ServerPort = 8092;
int numas2 = 2 ;
std::string SiocFilename = "fsx1.txt";
bool InitRs232=false;

#define SWITCH_NB_CAR 2 

#define MAX_DIGIT 255
byte Digits[MAX_DIGIT];

bool		SendToFsx ( int Var , byte SwValue )
{
  EnumIoType typ = GetVarIoType(Var);
  int evt = GetVarEventId(Var, 0 ); ;
//	Console->debugPrintf ( TRACE_FSX_SEND,"FSX  :Send Event:%3d (%s)  Value:%d Var:%d ",evt,Event.GetEventName(evt).c_str(),SwValue, Var );

	if ( (typ==IOCARD_SW) )  
  {
    //get event
    if(GetVarType(Var)==NEGATIV)
				SwValue = !SwValue;
    SendControl( evt , SwValue );
  }
	else if (typ==IOCARD_PUSH_BTN) 
  {
    //get event
    if(GetVarType(Var)==NEGATIV)
				SwValue = !SwValue;
		if (SwValue)
			SendControl( evt , MOUSE_FLAG_LEFTSINGLE );
		else
			SendControl( evt , MOUSE_FLAG_LEFTRELEASE );
  }
	else if (typ==IOCARD_SW_3P) 
  {
    int input  = GetVariable(Var)->Input ;
    int number = GetVariable(Var)->Numbers ;
    if((number==0)||(evt==EVENTNOTFOUND)||(input==0))
	    Console->errorPrintf ( 0 ,"FSX  :event or offset or input not defined for variable %d : %s\n", Var , GetVarName(Var)  );
    int code=0;
    for (int i=0;i<number;i++)
    {
      code *= 2 ;
      int sw = Switch.get ( input + i ) ;
      sw &=1 ;
      if(GetVarType(Var)==NEGATIV)
				sw = !sw;
      code += sw ;
    }
    //tranlaste value ;
    if (GetVariable(Var)->Codage.size()>=4)
      code = GetVariable(Var)->Codage.c_str()[code]-'0';
//    Console->debugPrintf ( TRACE_FSX_SEND,"Coding:%d ",code );
    SendControl( evt , code );
  }
  else if (typ==IOCARD_ENCODER)
  {
    int offset = GetVariable(Var)->Offset ;
    double min = GetVarMin(Var);
    double max = GetVarMax(Var);
    double inc = GetVarInc(Var);

    if (inc==0) inc=1;

    //case increment calculer
    if ((evt!=EVENTNOTFOUND) &&(offset!=0))
    {
      double value = Fsuipc.GetValue(offset);
      char SwV = (char)SwValue;
			value = value + SwV * inc ;
      if (value>max) value = min  ;
      if (value<min) value = max  ;
	    Fsuipc.SetValue(offset , value) ;

			value = Event.Get(evt)->a * value + Event.Get(evt)->b;
      SendControl( evt , (int)value );
    }
    else if ((evt!=EVENTNOTFOUND) &&(offset==0))
    {
      char SwV = (char)SwValue;
      int IncId = GetVarEventId(Var, 0 ); 
      int DecId = GetVarEventId(Var, 1 ); 
      if((DecId==EVENTNOTFOUND))
	      Console->errorPrintf ( 0 ,"FSX  : decrement event not defined for variable %d : %s\n", Var , GetVarName(Var)  );

      if (SwV>0)
        SendControl( IncId , 0 );
      else
        SendControl( DecId , 0 );
    }
    else
	    Console->errorPrintf ( 0 ,"FSX  :event or offset not defined for variable %d %s \n", Var, GetVarName(Var) );
  }
  else if (typ==IOCARD_SELECTOR)
  {
    int input  = GetVariable(Var)->Input ;
    int number = GetVariable(Var)->Numbers ;
    if((number==0)||(evt==EVENTNOTFOUND)||(input==0))
	    Console->errorPrintf ( 0 ,"FSX  :event or offset or input not defined for variable %d : S\n", Var , GetVarName(Var)  );
    int SwitchActifValue = (GetVarType(Var)!=NEGATIV) ;
    //selector value 
    for (int value=0; value<number;value++)
    {
        if ( Switch.get ( input + value) == SwitchActifValue )
        {
//          Console->debugPrintf ( TRACE_FSX_SEND," EventValue:%d ",value );
          SendControl( evt , (int)value );
          break;
        }
    }
  }

    else if (typ==IOCARD_SWAPER)
  {
    int Var1  = GetVariable(Var)->Var1 ;
    int Var2  = GetVariable(Var)->Var2 ;

    int Input = GetVariable(Var1)->Input ;
    int number= GetVariable(Var1)->Numbers ;

    int SwitchActifValue = (GetVarType(Var)!=NEGATIV) ;

    //ingnore pasive balue 
    if (SwitchActifValue != SwValue)
      return true;

    if((Var1<=0)||(Var2<=0))
	    Console->errorPrintf ( 0 ,"FSX  :Variable 1 or 2 not defined for variable %d : S\n", Var , GetVarName(Var)  );

    //selector value 
    for (int inp=Input; inp<Input+number;inp++)
    {
        //get current variable input 
        int v = GetInputVar(inp) ;
        //swap var
        if ( v==Var1)
          SetInputVar(Var2 , inp );
        else
          SetInputVar(Var1 , inp );
        int  SiocVar = GetInputVar(inp) ;
        Console->debugPrintf (  TRACE_FSX_SEND ,  "FSX  :  Swaping Input:%3d Var:%4d IO:%s Name:%-14s\n",inp,SiocVar, GetIOTypeStr(GetVarIoType(SiocVar)), GetVarName(SiocVar)  );

    }
  }

  


//  Console->Flush();
  return true;
}
DWORD WINAPI ThreadAs2(LPVOID lpArg)
{
  int dwRead;
  byte Buf[256];
	Console->printf ( "Rs232 Thread launched\n");
	while (1==1)
	{
		InitRs232=false;

		int Num = (int)lpArg;
		if (Com.OpenAsync(Num)!= AS2_OK)
		{
			Console->errorPrintf(1,"error opening com %d\n",Num);
		}
		else
		{
		Com.Clear ();
		Console->printf("opening com %d\n",Num);
		Com.setspeed ( 115200, 'N' , 8  ,1  ) ;
		Com.setTimeouts ( 0 ,1000 ) ;
    
		while(1==1)
		{
			//waitr char
			Buf[0]=0;
			dwRead = Com.ReadAsync(Buf,SWITCH_NB_CAR);

			if (dwRead==SWITCH_NB_CAR)
			{

				int SwNumber = GetSwitchNumber( Buf) ;
				int SwValue  = GetSwitchValue ( Buf) ;
        Switch.set(SwNumber, SwValue) ;

				//get Sioc Var Number
				int SiocVar  = GetInputVar(SwNumber);
				if (SwNumber!= 255)
					Console->debugPrintf ( TRACE_RS232_RECV,"RS232:Recv :%02x%02x Sw:%3d Value:%d SiocVar:%d\n",Buf[0],Buf[1],SwNumber,SwValue, SiocVar );
				else
					Console->debugPrintf ( TRACE_RS232_RECV_PRESENCE,"RS232:Recv :%02x%02x Sw:%3d Value:%d SiocVar:%d\n",Buf[0],Buf[1],SwNumber,SwValue, SiocVar );

#ifdef USE_SIOC
				//if Var defined , send value to Sioc
				if (SiocVar)
					SiocSend ( SiocVar , SwValue );
#else
        //
				if (SiocVar)
					SendToFsx ( SiocVar , SwValue );
#endif
			}
			else
				break;
			if (InitRs232==false)
			{
				EncoderArduinoRegister();
				InitRs232 = true;
			}

		}
		Com.Close();
		}

		Sleep(10000);
	}
	
	Console->printf ( "RS232 Thread stopped\n");

  return 0;
}

void SendDigitCmd (byte digitNb , byte digitValue )
{
	if (Digits[digitNb]!=digitValue)
	{
		SendOutputCmd ( IOCARD_DISPLAY_CMD , digitNb ,  digitValue ) ;
		Digits[digitNb]=digitValue ;
	}
}


void RefreshOutput (int Variable , double value )
{
	int Value = (int)value;
  int digitValue;
	int digitNb;

  if (GetVarIoType(Variable) == IOCARD_OUT)
  {
    int OutputNumber = GetVarOutput(Variable);
	  Console->debugPrintf (  TRACE_SIOC_RECV , "REFR : Out:%d Value:%d\n", OutputNumber , (int)value);

    SendOutputCmd ( IOCARD_OUT_CMD , OutputNumber ,  Value ) ;

  }
  else
  if (GetVarIoType(Variable) == IOCARD_DISPLAY)
  {
    int Digit = GetVarDigit(Variable);
    int Number= GetVarNumbers(Variable);
    int offset = GetVariable(Variable)->Offset ;
    
    double min = GetVarMin(Variable);
    double max = GetVarMax(Variable);
    double inc = GetVarInc(Variable);
		
		Console->debugPrintf (  TRACE_SIOC_RECV , "REFR :Dig:%d Nb:%d Value:%d Max:%f Min:%f Inc:%f \n", Digit ,Number, (int)value,max,min,inc );
		bool displaySigne = false;
		if (min<0)
			displaySigne=true;

		if (value<min || value>max)
		{
			//blank for invalid value
			for (int i=0;i<Number;i++)
			{
				SendDigitCmd (  Digit+i ,  0xf ) ;
			}
		}
		else
		{
			if (displaySigne)
			{
				if (Value<0)
				{
					Value=-Value;
					//display signe -
					digitValue=0xA;
				}
				else
					//display black
					digitValue=0xF;
				Number--;
				digitNb = Digit+Number;
				SendDigitCmd (  digitNb ,  digitValue ) ;
			}
			for (int i=0;i<Number;i++)
			{
				digitValue = Value % 10 ;
				Value /=10;
				digitNb =  Digit+i ;
				SendDigitCmd (  digitNb ,  digitValue ) ;
			}
		}
		
  }

}
DWORD WINAPI ThreadSioc(LPVOID lpArg)
{
	char recvbuf[1024]  ;
  bool Terminated=false;

  ClientTcp:: WinsockInit();
 	Tcp = new ClientTcp();

  Console->printf ( "SIOC Thread launched\n");
  while(1==1)
  {
    Tcp->Open();
    Console->printf ( "Connecting to server %s Port %d\n",ServerIp.c_str(),ServerPort);
    while (Tcp->Connect((char*)ServerIp.c_str(),ServerPort)==false) {Sleep(10000);};
    Console->printf ( "Connectied to server %s Port %d\n",ServerIp.c_str(),ServerPort);
    SiocRegister();
    Terminated=false;
    while(!Terminated)
    {
      //wait receive from sioc
      memset(recvbuf,0,sizeof(recvbuf));
      if (Tcp->Recv( recvbuf, sizeof(recvbuf))==0)
      {
        Console->errorPrintf ( 0, "SIOC :Error reading SIOC server socket \n");
        Terminated=true;
      };
      Console->debugPrintf ( TRACE_SIOC_RECV , "SIOC :Recv: %s",recvbuf);

      T_StringList * lines = Split ( recvbuf , (char*)"\n\r" , (char*)"" , true );
      for (int i=0;i<lines->Count();i++)
      {
        std::string line  = (*lines)[i];
        if (strstr (line.c_str(),"Arn.Vivo")!=0)
        {
        }
        else
          if (strstr (line.c_str(),"Arn.Resp")!=0)
          {
            T_StringList * values = Split ( (char*)line.c_str() , (char*)":=" , (char*)"" , true );
            for (int i=1;i<values->Count();i++)
            {
              int Variable = atoi((*values)[i].c_str());
              int Value = atoi((*values)[i+1].c_str());
              i++;
              Console->debugPrintf (  TRACE_SIOC_RECV , "SIOC :Var :%3d = %-15s Value:%d ",Variable,GetVarName(Variable),Value );
              RefreshOutput ( Variable ,  Value ) ;

							Sleep(1);
            }
            delete values ;

          }

      }
      delete lines;
    }
    Tcp->Close();
  }
  Console->printf ( "SIOC Thread stopped\n");
  ClientTcp:: WinsockCleanup();

}

//register to Pmdg variable offset
void PmdgRegister()
{
	T_VARLIST varsOut ;
	GetSiocVar(IOCARD_OUT ,  varsOut ) ;
	GetSiocVar(IOCARD_DISPLAY ,  varsOut ) ;
	GetSiocVar(IOCARD_ENCODER ,  varsOut ) ;
	for (unsigned int i=0;i<varsOut.size();i++)
	{
		int var = varsOut[i]   ;
		if (!AsFsxVariableDefinition(var) )
			Fsuipc.RegisterToVariableChanged (GetVariable(var)->Offset,GetVariable(var)->Length,var);
	}
}

//
void RefreshDisplay()
{
	T_VARLIST varsOut ;
	GetSiocVar(IOCARD_OUT ,  varsOut ) ;
	GetSiocVar(IOCARD_DISPLAY ,  varsOut ) ;
	for (unsigned int i=0;i<varsOut.size();i++)
	{
		int var = varsOut[i]   ;
		int offset = GetVariable(var)->Offset;
    double Value = Fsuipc.GetValue(offset);
		if (offset)
				RefreshOutput(var,Value);
	}
}

//update switch value to fsx 
void SendSwitchValuesToFsx()
{
	T_VARLIST varsOut ;
	GetSiocVar(IOCARD_SW       ,  varsOut ) ;
	GetSiocVar(IOCARD_SELECTOR ,  varsOut ) ;
	GetSiocVar(IOCARD_PUSH_BTN ,  varsOut ) ;
	GetSiocVar(IOCARD_SW_3P    ,  varsOut ) ;
	for (unsigned int i=0;i<varsOut.size();i++)
	{
    //get var number
		int var     = varsOut[i]   ;
    int input   = GetVariable(var)->Input ;
    int swValue = Switch.get ( input ) ;
    SendToFsx(var,swValue);
	}
}
//tcp ComPort SiocFileName debug ServerIp ServerPort
int main(int argc, char **argv)
{
  Console = new TConsole ();
  Console->EnableDebugOutput = 0xFFFFFFFF;
  Console->EnableLogTimeStamp = false;
  if (argc==1) 
  Console->printf("Syntaxe:tcp ComPort SiocFileName ServerIp ServerPort\n");

  if (argc>=2) 
    numas2 = atoi(argv[1]);
  if (argc>=3) 
    SiocFilename = (argv[2]);
  if (argc>=4) 
    Console->EnableDebugOutput =strToInt (argv[3],10);
  if (argc>=5) 
    ServerIp = (argv[4]);
  if (argc>=6) 
    ServerPort = atoi(argv[5]);

	memset(Digits,-1,sizeof(Digits));
	Console->printf("ComPort:%d  SiocFileName:%s ServerIp:%s  ServerPort:%d Trace:%X\n",numas2,SiocFilename.c_str(),ServerIp.c_str(),ServerPort, Console->EnableDebugOutput );

	Fsuipc.ReadFromFile("FsuipcOffset3.csv");
  if (Console->isDebugEnable(TRACE_OFFSET))
  		Fsuipc.Print();
	Event.ReadFromFile("event.csv");
  if (Console->isDebugEnable(TRACE_EVENT))
  		Event.Print();

	ReadSioc(SiocFilename.c_str());
  PrintVars();

  T_THREAD thAs2 (ThreadAs2,(LPVOID)numas2 );
	//wait for rs232 init
  while (InitRs232==false)
		Sleep(100);

#ifdef USE_SIOC
  T_THREAD thSioc (ThreadSioc,(LPVOID)0 );
  thSioc.WaitTerminate();
#else
  //use simconnect
	PmdgRegister();
	T_THREAD thPmdg  (ThreadPMDG,(LPVOID)0 );
	thPmdg.WaitTerminate();
#endif
//	test1();
}