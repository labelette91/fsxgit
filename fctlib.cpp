//=========================================================
//===                                                   ===
//===                F C T L I B . C P P                ===
//===                                                   ===
//===   Avril 1999, L. MONFRAY                          ===
//=========================================================

//  Include de Turbo
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
//#include <dir.h>
#include <dos.h>
#include <fcntl.h>
#include <string.h>
//#include <alloc.h>
#include <ctype.h>
//#include <mem.h>
#include <stdarg.h>
#include <stdexcept>

#include "console.h"

//  Include de décalibration
//#include "dftypgen.h"
#include "fctlib.h"
#ifndef U32B
#define U32B unsigned int
#endif
//  Conversion chaine -> valeur numérique en base 10 ou hexadécimal
//  c'est une fonction issue de NELSON's LAND
U32B xatoi(char *str)
{
#define   DECI   1
#define   HEXA   2
#define   NODEF  10
#define   DEF    11
#define   END    12

    U32B i, k;
    int base;
    int status;

    status = NODEF;
    base = DECI;
    for(i=0; strlen(str); str++)
    {
        if(status == NODEF)
            if((*str == 'x') || (*str == 'X') || (*str == '$'))
            {
                base = HEXA;
                status = DEF;
                continue;
            }
        switch(base)
        {
        case DECI : if((*str >= '0') && (*str <= '9'))
            {
                i = 10 * i + *str - '0';
                if(i)
                    status = DEF;
            }
            else
                status = END;
            break;

        case HEXA : if((*str >= 'A') && (*str <= 'F'))
                k = *str - 'A' + 10;
            else
                if((*str >= 'a') && (*str <= 'f'))
                    k = *str - 'a' + 10;
                else
                    if((*str >= '0') && (*str <= '9'))
                        k = *str - '0';
                    else
                    {
                        status = END;
                        break;
                    }
            i = 16 * i + k;
            break;
        }
        if(status == END)
            return(i);
    }
    return(i);
}

//conversion d'un string en U32B
//la base est passer dans le parametre base
// si le string est de la forme:
//0x : base = 16
//0o : base = 8
//0x : base = 16
//0# : base = 2
//return true si erreur dans le string
//convertion char en unsigned int
//genere une exception runtime_error si erreur de conversion
//return true if error
bool StrToUInt  (const char *str , int base , unsigned int * result )
{

    U32B k=0, val=0;
    bool err=false;

//base 10 par defaut
   if (base==0)
        base=10;
   while ( *str )
    {
        if((*str == 'x') || (*str == '$'))
        {
            base=16;
            str++;
            continue;
        }
        else if(*str == '#')
        {
            base=2;
            str++;
            continue;
        }
        else if(*str == 'o')
        {
            base=8;
            str++;
            continue;
        }
        else
         if((*str >= 'A') && (*str <= 'F'))
            k = *str - 'A' + 10;
         else if((*str >= 'a') && (*str <= 'f'))
            k = *str - 'a' + 10;
         else if((*str >= '0') && (*str <= '9'))
            k = *str - '0';
         else
             err=true;
         if ( k >= (U32B)base )
             return true;
         val = base * val + k;
         str++;
    }
    *result = val ;
    return(err);
}

//return true si caractere non decimal
//convertion char en float
//genere une exception runtime_error si erreur de conversion
//return true if error
bool StrToDouble  (const char *Str , int base , double * result )
{
#define READ_PARTIE_ENTIERE  0
#define READ_PARTIE_DECIMALE 1
#define READ_EXPOSANT        2

    char *endptr;


    double k=0, val=0;
    bool err=false;
    int PartieEntiere=READ_PARTIE_ENTIERE ;
    double signe=1.0;
//    double signe_mantise=1.0;
    double decimale;
    char * str=( char * )Str;

//base 10 par defaut
   if (base==0)
        base=10;
   while ( *str )
    {
        if((*str == 'x') || (*str == '$'))
        {
            base=16;
            str++;
            continue;
        }
        else if(*str == '#')
        {
            base=2;
            str++;
            continue;
        }
        else if(*str == 'o')
        {
            base=8;
            str++;
            continue;
        }
        else if( (*str == '.') || (*str == ',') )
        {
            //base 10
            decimale=0.1 ;
            str++;
            PartieEntiere=READ_PARTIE_DECIMALE ;
            continue;
        }
        else if(*str == '+')
        {
            str++;
            continue;
        }
        else if(*str == '-')
        {
            str++;
            signe=-1.0;
            continue;
        }
        else  if((*str >= 'A') && (*str <= 'F') && (base==16) )
            k = *str - 'A' + 10;
        else if((*str >= 'a') && (*str <= 'f'))
            k = *str - 'a' + 10;
        else if((*str >= '0') && (*str <= '9'))
            k = *str - '0';
        else  if((*str == 'E') || (*str == 'e') && (base==10) )
        {
            //exposant E-10 : appel fonction normal
            *result= strtod(Str , &endptr);
            if (*endptr==0)
                return false;
            else
                return false;
            
        }

        else
            err=true;
        if ( k >= base )
            return true;
        //si partie entiere
        if (PartieEntiere==READ_PARTIE_ENTIERE)
            val = base * val + k;
        else if (PartieEntiere==READ_PARTIE_DECIMALE)
        {   
            //si partie decimale : base  10 
            val = val + k * decimale;
            decimale = decimale *.1;
        }
        str++;
   }
   *result = val*signe ;
   return(err);
}



//
// découpage de ligne
//
char *decoupe(char *ligne, char *mot, char *sep)
{
    /* saute le separateur */
    while((*ligne!=0) && (strchr(sep, *ligne)))
        ligne++;
    /* recupere le mot */
    while((*ligne!=0) && ( !strchr(sep, *ligne)))
        *mot++ = *ligne++;
    *mot= 0;
    /* saute le separateur */
    while((*ligne!=0) && (strchr(sep, *ligne)))
        ligne++;
    return(ligne);
}
//
// découpage de ligne
//
//ligne ligne a decoupe
//mot   : mot decoupe
//separator : liste des carateres separateurs
//concatenator : liste des carateres qui concatene example 
//  si = ''   pour le decoupage de  ;"un;deux";;  retourne '','"un',deux"',''
//  si = '"'  pour le decoupage de  ;"un;deux";;  retourne '','un;deux'   ,''
//  si = '[]' pour le decoupage de  ;[un;deux];;  retourne '','un;deux'   ,''

// AllowDuplicateSeparator = true : un seul carateres separateurs est pris en compte,utiliser
//       si=true       pour le decoupage d'une ligne csv : ;un;deux;;  retourne "","un","deux",""
//       si=false      pour le decoupage d'une ligne csv : un;deux;;  retourne    "un","deux"
//MotLen : taille  max de mot

char *decoupe(char *ligne, char *mot, int MotLen , char *separator, char * concatenator , bool AllowDuplicateSeparator)
{
    char * sep = separator ;
    int nbCar=0;


    /* saute les separateurs de debut de ligne */
    if (AllowDuplicateSeparator)
            while((*ligne!=0) && (strchr(sep, *ligne)))
                ligne++;

    /* recupere le mot */
    while((*ligne!=0) && ( !strchr(sep, *ligne)))
    {
        //si concatenator carateres
        if ( strchr(concatenator, *ligne) )
        {
            sep = concatenator;
            ligne++;
        }
        else
        {
            //test si la taiile max a ete atteinte
            if (nbCar<MotLen)
            {
                *mot++ = *ligne++;
                nbCar++;
            }
        }
    }
    *mot= 0;

    //si concatenator carateres
    if (*ligne!=0)
        if ( strchr(concatenator, *ligne) )
        {
            sep = separator;
            ligne++;
        }

    if (AllowDuplicateSeparator)
    {
            /* saut de tous les separateur */
            while((*ligne!=0) && (strchr(sep, *ligne)))
                ligne++;
    }
    else
         /* saut du separateur */
         ligne++;


    return(ligne);
}
//
// découpage de ligne d'un csv
//
char *decoupecsv(char *ligne, char *mot, int MotLen , char *sep)
{
    int nbCar=0;
    MotLen--;
    /* test si fin de ligne \r \n 0*/
    if ( ( *ligne ==  '\r' ) || ( *ligne ==  '\n' ) )
    {
        *mot= 0;
        /* force fin de ligne ci \r \n */
        *ligne=0;
    }
    else
    /* test si pas de valeur : separateur */
    if (strchr(sep, *ligne))
    {
        *mot= 0;
        /* saute le separateur */
        ligne++;
    }
    else
    {
        /* recupere le mot */
        while((*ligne!=0) && ( !strchr(sep, *ligne)) && (nbCar<MotLen) )
        {
            *mot++ = *ligne++;
            nbCar++;
        }
        *mot= 0;
        /* saute le separateur */
        if (*ligne)
           ligne++;
    }
    return(ligne);
}


// renvoie 0 si chaine hexa
int ishexa(char *mot)
{
    int i, len;
    len =  strlen(mot);
    for(i=0; i<len; i++)
        if(! isxdigit(mot[i]))
            return(1);
    return(0);
}


int cvascbin(char v)
{
    if((v>= 'A') && (v<= 'F'))
        return(v- 'A' + 10);
    else
        if((v >= 'a') && (v <= 'f'))
            return(v- 'a' + 10);
        else
            if((v >= '0') && (v <= '9'))
                return(v- '0');
            else
                return(0);
}

int cvU08Bbin(char *mot)
{
    return(cvascbin(mot[1]) + 16 *cvascbin(mot[0]));
}



U32B swap_u32b(U32B vi)
{
    U32B vo;
    char *pti = (char*)&vi;
    char *pto = (char*)&vo;

    pto[3] = pti[0];
    pto[2] = pti[1];
    pto[1] = pti[2];
    pto[0] = pti[3];
    return(vo);
}

void swap_buf_u32b(U32B * bui, int len)
{
    int i;
    for(i=0; i<len; i++)
        bui[i] = swap_u32b(bui[i]);
}

// Convert byte to two ascii-hex characters
void ctohex(char *buf, int c)
{
    static char hex[] = "0123456789ABCDEF";

    *buf++ = hex[c >> 4];
    *buf = hex[c & 0xf];
}

// Print a buffer up to 16 bytes long in formatted hex with ascii
// translation, e.g.,
// 0000: 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f  0123456789:;<=>?
void fmtline(int addr, char *buf, int len, FILE*stream)
{
    char line[80];
    register char *aptr, *cptr;
    unsigned register char c;

    memset(line, ' ', sizeof(line));
    ctohex(line, addr >> 8);
    ctohex(line+2, addr & 0xff);
    aptr = &line[6];
    cptr = &line[55];
    while(len-- != 0)
    {
        c = *buf++;
        ctohex(aptr, c);
        aptr += 3;
        c &= 0x7f;
        *cptr++ = isprint(c) ? c : '.';
    }
    *cptr++ = '\n';
    fwrite(line, 1, (unsigned)(cptr-line), stream);
}

void fmtline1(int addr, char *buf, int len, int modblanc, int ascii, FILE*stream)
{
    char line[80];
    register char *aptr, *cptr;
    unsigned register char c;
    int n;

    n = 0;

    memset(line, ' ', sizeof(line));
    ctohex(line, addr >> 8);
    ctohex(line+2, addr & 0xff);
    aptr = &line[6];
    cptr = &line[55];
    while(len-- != 0)
    {
        c = *buf++;
        ctohex(aptr, c);
        aptr += 2;
        n++;
        if((n % modblanc) == 0)
            aptr ++;
        c &= 0x7f;
        if(ascii == 0)
            *cptr++ = isprint(c) ? c : '.';
    }
    if(ascii != 0)
    {
        *aptr++ = '\n';
        cptr = aptr;
    }
    else
        *cptr++ = '\n';
    fwrite(line, 1, (unsigned)(cptr-line), stream);
}

void dump_bytes(unsigned char *bytes, int count, int modblanc, int ascii, FILE*stream)
{
    int n;
    int address;
    int nbbytebyline;

    nbbytebyline = 16;

    if(ascii)
        nbbytebyline = ascii;
    if((modblanc != 1) && (modblanc != 2) && (modblanc != 4))
        modblanc = 1;
    address = 0;
    while(count)
    {
        if(count > nbbytebyline)
            n = nbbytebyline;
        else
            n = count;
        fmtline1(address,(char *) bytes, n, modblanc, ascii, stream);
        address += n;
        count -= n;
        bytes += n;
    }
}

//========================================
//==    Affichage d'un U32B en binaire  ==
//========================================
//nbBits : nombre de bit 1..32
void CvHexaBinaire(unsigned long Val, char *message, int decimalPos , int nbBits)
{
    int i, j;

    j = 0;
    nbBits--;
    for(i=nbBits; i>=0; i--)
    {
        if((Val >> i) & 1)
            message[j] = '1';
        else
            message[j] = '0';
        j++;
        if(((i % decimalPos ) == 0) && (i != 0))
            message[j++] = '.';
    }
    message[j] = 0;
}

void CvHexaBinaire(unsigned long Val, char *message, int decimalPos )
{
  CvHexaBinaire( Val, message, decimalPos , 32 ) ;

}

/******************************************************************************
*                                                                             *
* FILE NAME               :  INTOALIB                                         *
*                            integer to ascii                                 *
*                                                                             *
* FUNCTION(S) DESCRIPTION :  Conversion of an unsigned decimal integer        *
*                            (32 bits) into a nbchar chars array              *
*                            (ended by \0) with base conversion		      *
*                                                                             *
******************************************************************************/


void intoalib (
  unsigned long vaconv ,         /* value to convert */
  /* char array result (shall have a size greater than nbchar +1) */
  char *ptres,          
  int nbchar,          /* number of convert chars */
  int base             /* base conversion */
  )

{

int i ;
int val ;

for (i = nbchar; i > 0; i--)
{
   val = vaconv % base ;
   if (val<=9)
      val += 0x030;
   else
      val += 0x037;
   ptres[i-1] = val;
   vaconv /= base ;
}
ptres[nbchar] = 0  ;

}

int CreateMultimediaTimer (void*  procedure , int periodeInMs)
{
    // variables pour le multimédia timer
    TIMECAPS tc;                        // périodes possibles
    unsigned int numTimerMM = 0 ;            // pour les multimédia timer


    //
    // sélection de la résolution des timers à 1 ms
    if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) == TIMERR_NOERROR)    // test des limites
        timeBeginPeriod(tc.wPeriodMin);                             // !=TIMERR_NOERROR)


    // se cadencer sur un timer multimédia
    numTimerMM = timeSetEvent( periodeInMs,
                    	        0, // résolution max
                                (LPTIMECALLBACK)procedure,
                                0,
                                TIME_PERIODIC);
    return (numTimerMM);
}


//return true if ok
bool SetTimer (HANDLE numTimer , int periodeMs , int NextDelayInMs  )
{
    TIMECAPS tc;            // périodes possibles
    LARGE_INTEGER next;     // premier réveil

    // sélection de la résolution des timers à 1 ms
    if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) == TIMERR_NOERROR) // test des limites
        timeBeginPeriod(tc.wPeriodMin);             // !=TIMERR_NOERROR)
    // initialiser la cadence
    if(numTimer!=0) 
    {
        next.QuadPart = NextDelayInMs * (-10) ; //10000i64; // *100 ns prochain éveil
        if ( SetWaitableTimer(numTimer,&next,periodeMs,NULL,NULL,0)==0)
        return false ;
    }
    return (true);
}

HANDLE createWaitableTimer ( int periodeMs , int NextDelayInMs )
{
    // variables pour le multimédia timer
    TIMECAPS tc;            // périodes possibles
    HANDLE numTimer;        // pour les multimédia timer
    LARGE_INTEGER next;     // premier réveil

    numTimer = 0;

    // sélection de la résolution des timers à 1 ms
    if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) == TIMERR_NOERROR) // test des limites
        timeBeginPeriod(tc.wPeriodMin);             // !=TIMERR_NOERROR)
    // se cadencer sur un timer waitable
    numTimer =  CreateWaitableTimer(NULL, 0, NULL);
    next.QuadPart = NextDelayInMs * (-10) ; //10000i64; // *100 ns prochain éveil

    // initialiser la cadence
    if((numTimer!=0) && SetWaitableTimer(numTimer,&next,periodeMs,NULL,NULL,0)==0)
    {
        CloseHandle(numTimer);
        numTimer = 0;
    }
    return (numTimer);
}

HANDLE createWaitableTimer ( int periodeMs )
{
  return createWaitableTimer(periodeMs,1);
}


void waitWaitableTimer ( HANDLE numTimer , int periodeMs)
{
            if(numTimer)
            {
                WaitForSingleObject(numTimer,INFINITE);
            }
            else
                Sleep(periodeMs);

}

//split une chaine de caracteres avec les separateur.
//si AllowDuplicateSeparator = true  on ignore les separateurs multiples
//si AllowDuplicateSeparator = false : fichier type csv : ";;" renvoie un mot vide
//concatenator : caratere qui concatene , cad qui ignore les separateur entre 2 carateres de concatenation

void Split ( char * ptligne , char * separateur , char * concatenator , bool AllowDuplicateSeparator , T_StringList * StrL)
{
     char mot[1024];
     {
       while (*ptligne)
       {
         if (AllowDuplicateSeparator)
            ptligne = decoupe(ptligne , mot  ,sizeof(mot), separateur , concatenator, true );
         else
            ptligne = decoupecsv(ptligne , mot  , sizeof (mot), separateur );

         if (StrL) StrL->Add (mot);
       }
     }
}

T_StringList * Split ( char * ptligne , char * separateur , char * concatenator , bool AllowDuplicateSeparator )
{
     T_StringList * StrL = new T_StringList();
     Split (  ptligne ,  separateur , concatenator , AllowDuplicateSeparator ,  StrL) ;
      
     return (StrL);
}
/* recherche la chaine correspondant a une valeur d'un enum */
/* l'enum est de la forme OPEN:1,CLOSE:3,.. */
// GetEnumName ( ",OPEN:1,CLOSE:3,", 1 ) renvoie OPEN
AnsiString GetEnumName ( char * enumerate , int value )
{
    char val[1024];
    char * ptc;
    int  l ;
    val[0]=':';
    itoa(  value , &val[1] , 10 );
    val[0]=':';
    l = strlen(val);
    val[l]=',';
    val[l+1]=0;

    /* si enum found */
    ptc = strstr ( enumerate,val ) ;
    val[0]=0;
    if (ptc != 0  )
    {
       /* recherche : */
       while ( (*ptc!=',') && (ptc!=enumerate ) )
        ptc--;
       /* extrait nom */
       ptc = decoupe(ptc , val  , ":," );
    }

    return (AnsiString(val));
}

/* recherche la valeur d'un enum a partir de la chaine nom */
/* l'enum est de la forme OPEN:1,CLOSE:3,.., */
// GetEnumName ( ",OPEN:1,CLOSE:3,", "CLOSE," ) renvoie 3
//pour specifier la valeur d'un champs null : mettre ,:
AnsiString  GetEnumString ( char * enumerate , char * name )
{
    char val[1024];
    char * ptc;
    int  l ;

    /* si enum found */
    val[0] = ',' ;
    strncpy ( &val[1],name,sizeof(val));
    l = strlen(val) ;
    val[l++]=':' ;
    val[l]  = 0  ;

    ptc = strstr ( enumerate,val ) ;
    val[0]=0;
    if (ptc != 0  )
    {
          /* extrait nom si nom vide*/
          if (ptc[1]!=':')
                  ptc = decoupe(ptc , val  , ":," );
          /* extrait valeur */
          ptc = decoupe(ptc , val  , ":," );
    }
    return (AnsiString(val));
}

/* recherche la valeur d'un enum a partir de la chaine nom */
/* l'enum est de la forme OPEN:1,CLOSE:3,.., */
// GetEnumName ( ",OPEN:1,CLOSE:3,", "CLOSE" ) renvoie 3
int  GetEnumValueFromeName ( char * enumerate , char * name )
{
    return (atoi(GetEnumString (enumerate , name).c_str() ));
}

void __fastcall  launchProcess(char * cmdLine , long timeOut /*in ms*/)
{

PROCESS_INFORMATION pi    ;
STARTUPINFO si;

//Console->printf("Launching %s with timeout=%d\n", cmdLine , timeOut);

memset(&si,0,sizeof(STARTUPINFO));
si.dwFlags=STARTF_USEPOSITION | STARTF_USESHOWWINDOW;
si.wShowWindow=SW_SHOW;
si.lpReserved2=NULL;
si.lpReserved=NULL;
CreateProcess(NULL,cmdLine,NULL,NULL,false,CREATE_NEW_CONSOLE,NULL,NULL,&si,&pi);
WaitForSingleObject(pi.hProcess, timeOut );
if (pi.hThread!=NULL) CloseHandle(pi.hThread);
if (pi.hProcess!=NULL) CloseHandle(pi.hProcess);
}

U32B cvU32Bintel_motorola(U32B val)
{
   char *pt, c;
   U32B vret;
   vret = val;
   pt = (char *)&vret;
   c = pt[0];   pt[0] = pt[3];   pt[3] = c;
   c = pt[1];   pt[1] = pt[2];   pt[2] = c;
   return(vret);
}


//conversion ascii to integer avec gestion exception
int Atoi(const char *s)
{
    try
    {
        //si chaine non nulle
        if (*s)
           return strToInt (s,0);
    }
    catch (std::exception  &e)
    {
      //printf( "Exception %s\n",e.what() ); 
             Console->errorPrintf( 2,"Invalid Parameter Value : %s\n",s );
    }
    return 0 ;
}
//conversion ascii to float avec gestion exception
double Atof(const char *s)
{
    try
    {
        //si chaine non nulle
        if (*s)
            return atof (s);
    }
    catch (std::exception &e)
    {
      printf( "Exception %s\n",e.what() ); 
//       Console->errorPrintf( 2,"Invalid Parameter Value : %s\n",s );
    }
    return 0 ;
}

int lastError=0;

//la procedure met a jour la derniere erreur
// 0 = pas d'erreur
void setError ( int error )
{
    lastError = error;
}

//la procedure lit la derniere erreur et la remet a zero
// 0 = pas d'erreur
int getLastError (  )
{
    int err =   lastError;
    lastError = 0 ;
    return(err);
}

//convertit le string  en long avec la base
//return true if OK
bool StrToL ( char * string , int *result, int base  )
{
char *endptr;
long lnumber;

   /* strtol converts string to long integer  */
   lnumber = strtol(string, &endptr, base);
   //pas d'erreur
   if (*endptr==0)
   {
       *result=lnumber;
       return true;
   }
   else
       return false;


}
//convertit le string  en ulong avec la base
//return true if OK
bool StrToUl ( char * string , unsigned  int *result, int base  )
{
char *endptr;
unsigned long lnumber;

   /* strtol converts string to long integer  */
   lnumber = strtoul(string, &endptr, base);
   //pas d'erreur
   if (*endptr==0)
   {
       *result=lnumber;
       return true;
   }
   else
       return false;


}

//convertion char en int
//genere une exception runtime_error si erreur de conversion 
//return true if error
bool StrToInt  (const char *str , int base , int * result )
{
int  signe = 1;
U32B UResult=0  ;
bool res;

    //test du signe
    if (*str=='-')
    {
        signe=-1;
        str++;
    }
    else if (*str=='+')
    {
        str++;
    }
    res = StrToUInt ( str , base , &UResult ) ;
    *result= UResult * signe ;
    return res ;
}



//convertion char en int
//genere une exception runtime_error si erreur de conversion 
int StrToIntEx  (const char *str , int base )
{
int result  ;
bool res;

    res = StrToInt ( str , base , &result ) ;
    if (res)
        throw std::runtime_error ("Conversion error:" + std::string(str) );
    return  result ;
}

//convertion char en int
//ne genere pas d' exception si erreur de conversion 
int strToInt  (const  char *str , int base )
{
    try
    {
        return (StrToIntEx  (str , base ) );
    }
    catch ( std::exception &e )
    {
     // printf( "Exception %s\n",e.what() ); 
         Console->errorPrintf( 1,"Exception: %s" ,e.what() );
    }
    return 0;       
}



//convertion char en unsigned int
//genere une exception runtime_error si erreur de conversion 
unsigned int StrToUIntEx  (const char *str , int base )
{
unsigned int result  ;
bool res;

    res = StrToUInt ( str , base , &result ) ;
    if (res)
        throw std::runtime_error ("Conversion error:" + std::string(str) );
    return  result ;
}

//convertion char en unsigned int
//ne genere pas d' exception si erreur de conversion 
unsigned int strToUInt  (const char *str , int base )
{
    try
    {
        return (StrToUIntEx  (str , base ) );
    }
    catch ( std::exception &e )
    {
     // printf( "Exception %s\n",e.what() ); 
         Console->errorPrintf( 1,"Exception: %s" ,e.what() );
    }
    return 0;       
}


//convertion char en unsigned int
//genere une exception runtime_error si erreur de conversion 
double StrToDoubleEx   (const char *str , int base )
{
double  result  ;
bool res;

    res = StrToDouble  ( str , base , &result ) ;
    if (res)
        throw std::runtime_error ("Conversion error:" + std::string(str) );
    return  result ;
}

//convertion char en unsigned int
//ne genere pas d' exception si erreur de conversion 
double  strToDouble   (const char *str , int base )
{
    try
    {
        return (StrToDoubleEx   (str , base ) );
    }
    catch ( std::exception &e )
    {
     // printf( "Exception %s\n",e.what() ); 
         Console->errorPrintf( 1,"Exception: %s" ,e.what() );
    }
    return 0;
}


unsigned long FileSize(char * name)
{
    int handle;
    unsigned long len=0;

   /* create a file containing 10 bytes */
   handle = open(name, O_RDONLY	);

   /* display the size of the file */
   if (handle!=-1) len =  filelength(handle);

   /* close the file */
   close(handle);
   return len;

}

// Create the auto-reset event
HANDLE createEvent (char * name ) {
HANDLE Event = CreateEvent( 
            NULL,     // no security attributes
            FALSE,    // auto-reset event
            FALSE,   // initial state is not signaled
            name);    // object not named

return Event ;
};

void closeEvent ( HANDLE Event ) {
  CloseHandle(Event);
}
//TimeOut en ms 
//return true si timeout occurted
bool waitEvent ( HANDLE Event , int TimeOut ){
  if ( WaitForSingleObject(Event,TimeOut) == WAIT_TIMEOUT )
    return true;
  else
    return false;
}

bool pulseEvent(HANDLE Event){
  return PulseEvent (Event) ;
}

std::string BuildFilePath(char * CurDir , char * fileName ){
  int i=0;

//fileName = .\xxx\file.tst ou ..\xxx\file.tst
  if (fileName[0]=='.')
  {
      //fileName = ..\xxx\file.tst
      if (fileName[1]=='.')
      {
        char * ptc = &CurDir[strlen(CurDir)-1];
        if ( *ptc=='\\') ptc--;
        //tant que commence par .. 
        while ( (fileName[i]=='.')&& (fileName[i+1]=='.') )
        {
          //remonte dun niveau
          while( *ptc!='\\') ptc--;
          if ( *ptc=='\\') ptc--;
          i = i+3;
        }
        *ptc++;
        *ptc=0;
        std::string f = CurDir ;
        f += '\\' ;
        f += &fileName[i] ;
        return f ;
      }
      //fileName = .\xxx\file.tst 
      std::string f = CurDir ;
      f += &fileName[1] ;
      return f ;
  }
  else
  {
    //si pas de chemin : file.tst
    if (strchr(fileName,'\\')==0)
    {
        std::string f = CurDir ;
        f += '\\' ;
        f += fileName ;
        return f ;
    }
    ////fileName = c:\xxx\file.tst
    return fileName;
  }


}



T_StringList::T_StringList()
{
}

int __fastcall T_StringList::Add(char * S)
{
list.push_back(S);
return ( Count());
}
int __fastcall T_StringList::Add(std::string S)
{
return Add((char*)S.c_str());
}

void __fastcall T_StringList::LoadFromFile(char *  FileName)
{

std::string word; 
std::ifstream infile(FileName); 
while (infile >> word) { 
list.push_back(word); 
};
} 

void __fastcall T_StringList::SaveToFile(char *  FileName)
{

std::string word; 
std::ofstream outfile(FileName); 
for (unsigned int i=0;i<list.size();i++)
{
  outfile << list[i]; 
};
} 
void __fastcall T_StringList::Sort()
{
  std::sort(list.begin(), list.end());
}

bool __fastcall T_StringList::find(std::string & value)
{
if (std::find(list.begin(), list.end(), value ) != list.end())
return true;
else
return false;

}
std::string to_string(int val)
{
  char v[256];
  sprintf(v,"%d",val);
  return ( std::string( v ));
}



void toUpperString( std::string & Str )
{
  for (int i=0;i<Str.size();i++)
    Str[i]= toupper(Str[i]);
}
void toLowerString( std::string & Str )
{
  for (int i=0;i<Str.size();i++)
    Str[i]= tolower(Str[i]);
}
