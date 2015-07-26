#include "tcomas2.h"
#include "as2.h"
#include <stdio.h>

extern HANDLE ComHd[] ;

bool __fastcall TCommAs2::Enabled(void)
{
        return (isopenas2(DeviceNum));
}

int  __fastcall TCommAs2::Open( int pPortcom , int pBaudRate ,int pDatabits, int pStopbits, int  pParity )
{
 DeviceNum  = pPortcom ;
 BaudRate   = pBaudRate ;
 Databits   = pDatabits ;
 Stopbits   = pStopbits ;
 Parity     = pParity ;

 return (Open());
}

int __fastcall TCommAs2::Open(void)
{
        // Lecture et affectation du numéro de port Comm à utiliser
        //DeviceName = COMX
        DeviceNum =   atoi (&DeviceName.c_str()[3])   ;

        int ret = Open (DeviceNum);
        if (ret==0)
           setspeed (   BaudRate ,Parity , Databits ,Stopbits  );

        return (ret==0);
}
int __fastcall TCommAs2::Open( char * comStr )
{

char *  p;
        /* efface ":" */
        p = strchr ( comStr,':');
        if (p) *p = 0;

        int num =   atoi (&comStr[3])   ;
        return Open(num) ;
}

int __fastcall TCommAs2::Open( int numas2)
{
   DeviceNum = numas2;
   int ret = openas2 (numas2) ;

   g_Mutex = CreateMutex( NULL, TRUE, 0 );

    if (g_Mutex == NULL)
        return 0;
    else
        return ret;

}
int __fastcall TCommAs2::OpenAsync( int numas2)
{
    char  ComName[100] ;

   DeviceNum = numas2;
  
   sprintf ( ComName , "\\\\.\\com%d", numas2 );

  hCom  = CreateFile(
    ComName ,	                    // pointer to name of the file
    GENERIC_READ |GENERIC_WRITE, 	// access (read-write) mode
    0 ,                           // share mode
    (LPSECURITY_ATTRIBUTES)0 ,	    // pointer to security attributes
    OPEN_EXISTING ,	                // how to create
    FILE_FLAG_OVERLAPPED,	          // file attributes

    NULL  	                        // handle to file with attributes to copy
   );

   if (hCom == INVALID_HANDLE_VALUE) return (AS2_ERROR);
   ComHd[numas2] = hCom ;

   g_Mutex = CreateMutex( NULL, TRUE, 0 );

    if (g_Mutex == NULL)
        return AS2_ERROR;
    else
        return AS2_OK;

}

void __fastcall TCommAs2::setspeed ( long speed , char parite , int nbdata ,int nbstop  )
{
        setspeedas2 ( DeviceNum , speed , parite , nbdata ,nbstop  ) ;
}


void __fastcall TCommAs2::Close(void)
{
        closeas2(DeviceNum);
        CloseHandle(g_Mutex);
}
int __fastcall TCommAs2::Write(const void *Buf, int Count)
{

        return sdbufas2 ( DeviceNum ,  Count ,  (char*) Buf ) ;

}
int __fastcall TCommAs2::Read(void *Buf, int Count)
{
        return readbufas2 ( DeviceNum , Count ,   (char*)Buf) ;
}

/* lecture de nbcar carateres ou aret sur caractere recu = carend */
int __fastcall TCommAs2::ReadStr ( int nbcar , char carend , char  *Apt_string )
{
        return rdstras2 ( DeviceNum , nbcar , carend , Apt_string ) ;
}
//clear du buffer de reception
void __fastcall TCommAs2::Clear ()
{
        return clearrecas2 ( DeviceNum ) ;
}
// parametre le timeout en emmission / reception
// readTimeOut : timeout lecture en ms
// writeTimeOut : timeout ecriture  en ms
void  TCommAs2::setTimeouts ( int readTimeOut ,int writeTimeOut) 
{
    setCommTimeouts (DeviceNum,readTimeOut,writeTimeOut);
}

//return true if ok
bool TCommAs2::WriteAsync(void * lpBuf, int dwToWrite)
{
   OVERLAPPED osWrite = {0};
   DWORD dwWritten;
   DWORD dwRes;
   bool fRes;

   // Create this write operation's OVERLAPPED structure's hEvent.
   osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
   if (osWrite.hEvent == NULL)
      // error creating overlapped event handle
      return FALSE;

   // Issue write.
   if (!WriteFile(hCom, lpBuf, dwToWrite, &dwWritten, &osWrite)) 
   {
      if (GetLastError() != ERROR_IO_PENDING) 
      { 
         // WriteFile failed, but isn't delayed. Report error and abort.
         fRes = false;
      }
      else
      {
         // Write is pending.
         dwRes = WaitForSingleObject(osWrite.hEvent, INFINITE);
         switch(dwRes)
         {
            // OVERLAPPED structure's event has been signaled. 
            case WAIT_OBJECT_0:
                 if (!GetOverlappedResult(hCom, &osWrite, &dwWritten, FALSE))
                       fRes = false;
                 else
                  // Write operation completed successfully.
                  fRes = true;
                 break;
            
            default:
                 // An error has occurred in WaitForSingleObject.
                 // This usually indicates a problem with the
                // OVERLAPPED structure's event handle.
                 fRes = false;
                 break;
         }
      }
   }
   else
      // WriteFile completed immediately.
      fRes = true;

   CloseHandle(osWrite.hEvent);
   return fRes;
}

int TCommAs2::ReadAsync(void * Buf, int dwToRead )
{
  OVERLAPPED osReader = {0};
  DWORD dwRead=0;
  DWORD dwRes;

  // Create the overlapped event. Must be closed before exiting
  // to avoid a handle leak.
  osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

		//waitr char
    if (!ReadFile(hCom, Buf, dwToRead , &dwRead, &osReader))
      if (GetLastError() != ERROR_IO_PENDING)     // read not delayed?
      {
				CloseHandle(osReader.hEvent);
        return 0;
      }
      dwRes = WaitForSingleObject(osReader.hEvent, INFINITE);
      switch(dwRes)
      {
        // Read completed.
        case WAIT_OBJECT_0:
            if (!GetOverlappedResult(hCom, &osReader, &dwRead, FALSE)){
                // Error in communications; report it.
            }
            else{
                // Read completed successfully.
                 }
            break;

        case WAIT_TIMEOUT:
            // Operation isn't complete yet. fWaitingOnRead flag isn't
            // changed since I'll loop back around, and I don't want
            // to issue another read until the first one finishes.
            //
            // This is a good time to do some background work.
            break;

        default:
            // Error in the WaitForSingleObject; abort.
            // This indicates a problem with the OVERLAPPED structure's
            // event handle.
            break;
      }
      CloseHandle(osReader.hEvent);
      return dwRead;

}
