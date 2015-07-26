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

#define TRACE_RS232_SEND     (1l<<1)
#define TRACE_RS232_RECV     (1l<<2)

#define TRACE_SIOC_SEND      (1l<<3)
#define TRACE_SIOC_RECV      (1l<<4)

#define TRACE_FSX_SEND      (1l<<3)

#define TRACE_SIOC      (1l<<5)
#define TRACE_EVENT     (1l<<6)
#define TRACE_OFFSET    (1l<<7)

#define TRACE_RS232_RECV_PRESENCE     (1l<<8)

TCommAs2 Com ;

#include "thread.h"

#include "ClientTcp.h"

ClientTcp * Tcp ;

#include "FsuipcOffset.h"

#include "SiocVar.h"

void RefreshOutput (int Variable , double value );

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
  if (typ==IOCARD_SW)
  {
    //get event
    int event = GetVariable(Var)->Event ;
	  Console->debugPrintf ( TRACE_FSX_SEND,"FSX  :Send Event:%3d Value:%d SiocVar:%d\n",event,SwValue, Var );
    
    SendControl( event , SwValue );

  }
  else if (typ==IOCARD_ENCODER)
  {
    int event  = GetVariable(Var)->Event ;
    int offset = GetVariable(Var)->Offset ;
    double min = GetVariable(Var)->Min ;
    double max = GetVariable(Var)->Max ;
    double inc = GetVariable(Var)->Inc ;

    if (inc==0) inc=1;

    double value = Fsuipc.GetValue(offset);
    if ((event!=0) &&(offset!=0))
    {
			 if (SwValue==1) 
				 value = value + SwValue * inc ;
			 else
				 value = value - SwValue * inc ;

      if (value>max) value = min  ;
      if (value<min) value = max  ;
      SendControl( event , (int)value );
	    Console->debugPrintf ( TRACE_FSX_SEND,"FSX  :Send Event:%3d %-20s Val:%d Var:%d\n",event,Event.GetEventName(event).c_str(), (int)value , Var );
    }
    else
	    Console->errorPrintf ( 0 ,"FSX  :event or offset not defined for variable %d %s \n", Var, GetVarName(Var) );

  }
  else if (typ==SELECTOR)
  {
    int event  = GetVariable(Var)->Event ;
    int input  = GetVariable(Var)->Input ;
    int number = GetVariable(Var)->Numbers ;
    if((number==0)||(event==0)||(input==0))
	    Console->errorPrintf ( 0 ,"FSX  :event or offset or input not defined for variable %d : S\n", Var , GetVarName(Var)  );

    //selector value 
    for (int value=0; value<number;value++)
    {
        if ( Switch.get ( input + value) == 0 )
        {
          SendControl( event , (int)value );
	        Console->debugPrintf ( TRACE_FSX_SEND,"FSX  :Send Event:%3d Value:%d SiocVar:%d\n",event,(int)value , Var );
          break;
        }
    }

  }

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

void RefreshOutput (int Variable , double value )
{
	int Value = (int)value;
  if (GetVarIoType(Variable) == IOCARD_OUT)
  {
    int OutputNumber = GetVarOutput(Variable);
	  Console->debugPrintf (  TRACE_SIOC_RECV , "Dig:%d \n", OutputNumber );

    SendOutputCmd ( IOCARD_OUT_CMD , OutputNumber ,  Value ) ;

  }
  else
  if (GetVarIoType(Variable) == IOCARD_DISPLAY)
  {
    int Digit = GetVarDigit(Variable);
    int Number= GetVarNumbers(Variable);
		Console->debugPrintf (  TRACE_SIOC_RECV , "Dig:%d Nb:%d\n", Digit ,Number );

    for (int i=0;i<Number;i++)
    {
      int digitValue = Value % 10 ;
      Value /=10;
			int digitNb =  Digit+i ;
			if (Digits[digitNb]!=digitValue)
			{
				SendOutputCmd ( IOCARD_DISPLAY_CMD , digitNb ,  digitValue ) ;
				Digits[digitNb]=digitValue ;
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

//refister to Pmdg variable offset
void PmdgRegister()
{
	T_VARLIST varsOut ;
	GetSiocVar(IOCARD_OUT ,  varsOut ) ;
	GetSiocVar(IOCARD_DISPLAY ,  varsOut ) ;
	for (unsigned int i=0;i<varsOut.size();i++)
	{
		int var = varsOut[i]   ;
		Fsuipc.RegisterToVariableChanged (GetVariable(var)->Offset,GetVariable(var)->Length,var);
	}
}


//tcp ComPort SiocFileName debug ServerIp ServerPort
int main(int argc, char **argv)
{
  Console = new TConsole ();
  Console->EnableDebugOutput = 0xFFFFFFFF;
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

	Fsuipc.ReadFromFile("FsuipcOffset1.csv");
  if (Console->isDebugEnable(TRACE_OFFSET))
  		Fsuipc.Print();
	Event.ReadFromFile("event.csv");
  if (Console->isDebugEnable(TRACE_EVENT))
  		Event.Print();

	ReadSioc(SiocFilename.c_str());
  PrintVars();

  T_THREAD thAs2 (ThreadAs2,(LPVOID)numas2 );
//	InitRs232=true;
	while (InitRs232==false)
		Sleep(1000);
	Sleep(1000); 

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