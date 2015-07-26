//---------------------------------------------------------------------------
#include<windows.h>

#include "as2.h"
#include <stdio.h>

#define MXCOM 255
HANDLE ComHd[MXCOM] ;
char  ComName[10] ;


//---------------------------------------------------------------------------
//#pragma package(smart_init)
//open com
//return 0 = OK
//      <>0=KO 
int openas2 ( char * comStr )
{
char *  p;
        /* efface ":" */
        if (comStr==0) return AS2_ERROR;
        if (strlen(comStr)<3) return AS2_ERROR;
        p = strchr ( comStr,':');
        if (p) *p = 0;

        int num =   atoi (&comStr[3])   ;
        return(openas2(num));

}
//open com
//return 0 = OK
//      <>0=KO
int openas2 ( int numas2)
{

HANDLE hd ;
//DCB dcb ;	;
//int res ;
//sprintf ( ComName , "COM%d:", numas2 );

sprintf ( ComName , "\\\\.\\com%d", numas2 );



if (numas2>MXCOM)
    return (AS2_ERROR);

hd  = CreateFile(
    ComName ,	                    // pointer to name of the file
    GENERIC_READ |GENERIC_WRITE, 	// access (read-write) mode
    0 ,                             // share mode
    (LPSECURITY_ATTRIBUTES)0 ,	    // pointer to security attributes
    OPEN_EXISTING ,	                // how to create
    FILE_ATTRIBUTE_NORMAL,	        // file attributes
    NULL  	                        // handle to file with attributes to copy
   );

   if (hd == INVALID_HANDLE_VALUE) return (AS2_ERROR);
   ComHd[numas2] = hd ;

   setspeedas2 (numas2 , 9600 , 'N' , 8 ,  1 ) ;

   setCommTimeouts( numas2 ,  1000  ,    1000   );

//   Numas2 = numas2 ;
   return (AS2_OK) ;
}
int closeas2 ( int numas2)
{

if (numas2>MXCOM)
    return (AS2_ERROR);

int ret = CloseHandle ( ComHd[numas2] )  ;
   ComHd[numas2] = 0  ;
   return ret;

}
/* emission d'un buffer*/
//return le nombre d'octets emis
int sdbufas2 ( int Num_COM , int Nb_char ,  char * ptbuff )
{
DWORD nb  = 0 ;
byte * b = (byte*)ptbuff;
if (Num_COM>MXCOM)
    return (0);

 BOOL ok = WriteFile(

    ComHd[Num_COM],	    // handle to file to write to
    ptbuff,	            // pointer to data to write to file
    Nb_char,	        // number of bytes to write
    &nb ,	            // pointer to number of bytes written
    (LPOVERLAPPED) 0   	// pointer to structure needed for overlapped I/O
   );
 if (ok==0) 
   nb=0;
 return (nb) ;

}

/* emission d'un string */
//return le nombre d'octets emis
int sdstras2 ( int Num_COM , char * str )
{
  return (  sdbufas2( Num_COM , strlen(str) , str )  );
}

int sdstrwaitas2 ( int Num_COM , char * str , int wait )
{
    int len = strlen(str);
    int sent = 0 ;
    for (int i=0;i<len;i++)
    {
        sent += sdbufas2( Num_COM , 1 , &str[i] );
        Sleep(wait);
    }
    return (sent);
}
/* reception d'un buffer*/
//return le nombre d'octets recus
int readbufas2 ( int Num_COM , int Nb_char ,  char * ptbuff)
{
DWORD nb  = 0 ;

if (Num_COM>MXCOM)
    return (0);

 BOOL ok = ReadFile(

    ComHd[Num_COM],	    // handle to file to write to
    ptbuff,	            // pointer to data to write to file
    Nb_char,	        // number of bytes to read max
    &nb ,	            // pointer to number of bytes written
    (LPOVERLAPPED) 0   	// pointer to structure needed for overlapped I/O
   );
 if (ok==0)
   nb=0;
 return (nb) ;
}

/* lecture d'un caractere */
//return le caractere recu ou 0 si timeout
char readcaras2 ( int Num_COM )
{
  char buf[2] ;
  if ( readbufas2 ( Num_COM , 1  ,  buf ) )
    return (buf[0] );
  else
    return (0 ) ;
}
/* clear buffer de reception */
void clearrecas2 ( int Num_COM )
{
char buf [256] ;

if (!isopenas2(Num_COM))
    return  ;

//COMMTIMEOUTS CommTimeoutsold ;	//   comm. time-outs structure
//COMMTIMEOUTS CommTimeouts    ;	//   comm. time-outs structure

/*    GetCommTimeouts(  ComHd[Num_COM],	&CommTimeoutsold   );
    CommTimeouts.ReadIntervalTimeout        = MAXDWORD ;
    CommTimeouts.ReadTotalTimeoutMultiplier = 0 ;
    CommTimeouts.ReadTotalTimeoutConstant   = 0 ;
    SetCommTimeouts(   ComHd[Num_COM], &CommTimeouts   );

    while ( ( readbufas2 ( Num_COM , 10  ,  buf))==10 );

    SetCommTimeouts(   ComHd[Num_COM], &CommTimeoutsold   );
    */

    int nbCar = getcaras2 (Num_COM);

    if (nbCar>0)
      for ( int i=0;i<nbCar;i++)
           if ( readbufas2 ( Num_COM , 1  ,  buf)==0 )
               break;

}

/* lecture de nbcar carateres ou aret sur caractere recu = carend */
// nbcar : nombre max de car
// carend : caractere de fin de lecture si <> 0
// Apt_string : buffer de reception
int rdstras2 ( int Ano_port , int nbcar , char carend , char  *Apt_string )
{

  char car = (char) 0xff ; /* caractäre lu sur COMi */
  int  cpt_car,nb ;  /* nb de caractäres reáus sur COMi  */

  nb = 1;
  cpt_car = 0 ;
  if (carend == 0 )
  {
    cpt_car = readbufas2 ( Ano_port , nbcar  , Apt_string  );
  }
  else
//  while ( ( cpt_car < nbcar ) && (car != carend ) && (nb == 1))
  do
  {
//    if (abortas2 () ) longjmp(jumpabortas2,1) ;
    nb = readbufas2 ( Ano_port , 1  ,  &car ) ;
    if ( nb > 0 )
    {
       Apt_string[cpt_car] = car ;
       cpt_car ++ ;
    }
  }
  while ( ( cpt_car < nbcar ) && (car != carend ) && (nb == 1));

#ifdef ECHO
  sdmesas2('\n' );
#endif

  Apt_string[cpt_car] = '\0' ;
  return (cpt_car);

} /* FIN fct rdstras2 () */


char * test1 = "ATZ\r\n" ;
void testAs2 ()
{
  char buf[200] ;
int r ;
long Speedas2  = 9600 ;
int  Numas2  = 1 ;


r = openas2 ( Numas2) ;
clearrecas2  (Numas2) ;
r = sdstras2 ( Numas2 ,test1  ) ;
r = sdstras2 ( Numas2 ,test1  ) ;
r = sdstras2 ( Numas2 ,test1  ) ;
r = readbufas2 ( Numas2 , 6  ,  buf);
if ( strncmp ( test1,buf, strlen(test1)) == 0 )  printf ( "OK\n"); else   printf ( "KO\n");

clearrecas2  (Numas2) ;
r = sdstras2 ( Numas2 ,test1  ) ;
r = rdstras2 ( Numas2 , 100  , 0  , buf )  ;
if ( strncmp ( test1,buf, strlen(test1)) == 0 )  printf ( "OK\n"); else   printf ( "KO\n");

clearrecas2  (Numas2) ;
r = sdstras2 ( Numas2 ,test1  ) ;
r = rdstras2 ( Numas2 , 8  , 10  , buf )  ;
if ( strncmp ( test1,buf, strlen(test1)) == 0 )  printf ( "OK\n"); else   printf ( "KO\n");
r = rdstras2 ( Numas2 , 8  , 13  , buf )  ;

if (r)
 r=0;
closeas2 (Numas2);


}

// set speed
//speed : vitesse : 1900
// parite : 'O','E,'N'
// nbdata : nombre data bit
// nbstop : nombre stop bit
void setspeedas2 ( int com , long speed , char parite , int nbdata ,int nbstop  )
{
 DCB dcb ;
 char para[132] ;

 if (com>MXCOM)
    return ;

    GetCommState(ComHd[com]  , &dcb  );
    sprintf( para ,  "COM%d: baud=%ld parity=%c data=%d stop=%d" , com , speed , parite , nbdata , nbstop   );
    BuildCommDCB( para , &dcb );

    if (parite != 'N')
        dcb.fParity = TRUE ;

    dcb.fDtrControl = DTR_CONTROL_DISABLE    ;
    dcb.fRtsControl = RTS_CONTROL_DISABLE   ;
    dcb.fOutxDsrFlow=0;
    dcb.fOutxCtsFlow = 0 ;
    dcb.fTXContinueOnXoff = 1 ;

    SetCommState(     ComHd[com] , &dcb);
    GetCommState(ComHd[com]  , &dcb  );
}

//return le nombre de carateres recu dans le buffer
int  getcaras2 (int com)
{
 DWORD lpErrors ;
 COMSTAT lpStat ;

 //si pas ouvert
 if (!isopenas2(com))
    return 0 ;

 ClearCommError(

        ComHd[com] ,	// handle to communications device
    &lpErrors,	  	// pointer to variable to receive error codes
    &lpStat		// pointer to buffer for communications status
   );

return (lpStat.cbInQue) ;

}
// if cts = 1 cts flow control
void setctsflowas2 ( int com , bool CTS , bool DSR , int RTS , int DTR )
{
 DCB dcb ;
  if (com>MXCOM)
    return ;

  GetCommState(
    ComHd[com]  ,	    // handle of communications device
    &dcb 	    // address of device-control block structure
    );

    dcb.fOutxCtsFlow = CTS ;
    dcb.fOutxDsrFlow = DSR ;
    dcb.fDtrControl = DTR    ;
    dcb.fRtsControl = RTS    ;

    SetCommState(     ComHd[com] , &dcb);

}

// parametre le timeout en emmission / reception
// readTimeOut : timeout lecture en ms
// writeTimeOut : timeout ecriture  en ms
void setCommTimeouts (int numas2, int readTimeOut ,int writeTimeOut)
{
COMMTIMEOUTS CommTimeouts ;	//   comm. time-outs structure

  if (numas2>MXCOM)
    return ;

HANDLE hd = ComHd[numas2] ;

     GetCommTimeouts(   hd  ,    &CommTimeouts   );

// ReadIntervalTimeout
// 
//     The maximum time allowed to elapse between the arrival of two bytes on the 
//     communications line, in milliseconds. During a ReadFile operation, the time 
//     period begins when the first byte is received. If the interval between the 
//     arrival of any two bytes exceeds this amount, the ReadFile operation is 
//     completed and any buffered data is returned. A value of zero indicates that
//      interval time-outs are not used.
// 
//     A value of MAXDWORD, combined with zero values for both the ReadTotalTimeoutConstant
//      and ReadTotalTimeoutMultiplier members, specifies that the read operation is to 
//      return immediately with the bytes that have already been received, even if 
//      no bytes have been received.
// ReadTotalTimeoutMultiplier
// 
//     The multiplier used to calculate the total time-out period for read operations,
//      in milliseconds. For each read operation, this value is multiplied by the
//       requested number of bytes to be read.
// ReadTotalTimeoutConstant
// 
//     A constant used to calculate the total time-out period for read operations,
//      in milliseconds. For each read operation, this value is added to the product
//       of the ReadTotalTimeoutMultiplier member and the requested number of bytes.
// 
//     A value of zero for both the ReadTotalTimeoutMultiplier and ReadTotalTimeoutConstant
//      members indicates that total time-outs are not used for read operations.

     CommTimeouts.ReadIntervalTimeout       = 0 ;
     CommTimeouts.ReadTotalTimeoutMultiplier= 0 ;
     CommTimeouts.ReadTotalTimeoutConstant  = readTimeOut ;

     CommTimeouts.WriteTotalTimeoutMultiplier = writeTimeOut;
     CommTimeouts.WriteTotalTimeoutConstant   = writeTimeOut;
     CommTimeouts.WriteTotalTimeoutMultiplier = 0 ;
     CommTimeouts.WriteTotalTimeoutConstant   = 0 ;

     SetCommTimeouts(  hd  ,    &CommTimeouts   );


}
//return true if is open */
bool isopenas2(int com)
{
   if (com>MXCOM)
    return false;
   return (ComHd[com]>0 ) ;
}