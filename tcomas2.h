#ifndef tcommas2H
#define tcommas2H
#include "tcom.h"
#include<windows.h>

class  TCommAs2 : public TComm
{

public:
HANDLE g_Mutex; //Mutex pour protection l'acces com 
HANDLE hCom ;

public:
	bool __fastcall Enabled(void);
	int __fastcall Open(void);
	int __fastcall Open( char * comStr );
	int __fastcall Open( int numas2);
    int  __fastcall Open( int pPortcom , int pBaudRate ,int pDatabits, int pStopbits, int  pParity );
    void __fastcall setspeed ( long speed , char parite , int nbdata ,int nbstop  );
	void __fastcall Close(void);
	int __fastcall Write(const void *Buf, int Count);
	int __fastcall Read(void *Buf, int Count);
    int __fastcall ReadStr ( int nbcar , char carend , char  *Apt_string );
    void __fastcall TCommAs2::Clear ();
    void  setTimeouts ( int readTimeOut ,int writeTimeOut) ;
    int __fastcall TCommAs2::OpenAsync( int numas2);
    bool TCommAs2::WriteAsync(void * lpBuf, int dwToWrite);
    int TCommAs2::ReadAsync(void * Buf, int dwToRead );

};
#endif
