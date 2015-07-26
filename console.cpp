//---------------------------------------------------------------------------
//#include <vcl.h>
#include <stdio.h>
#include <stdarg.h>
#pragma hdrstop

#include "console.h"
//#include <dos.h>
#include <time.h>  

#pragma package(smart_init)
//
//
// données privées pour le mode muli-thread
//-------------------------------------------
TConsole * Console; // console par défaut
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
// Initialisation traitement commun des constructeurs
//-----------------------------------------------------
 TConsole::TConsole()
{
    Suspendu = false;
    errorList = new T_StringList();
    Synthese  = new T_StringList();
   consoleList= new T_StringList();

    EnableLogTimeStamp=true;
    ConsoleEnable=true;
    DisplayEnable=true;
    EnableDebugOutput = 0;  // variable pour dumper dans le fichier rs232.out les commandes gene


//    Buffer="";
    memset (  ThreadBuffer    , 0 , sizeof(ThreadBuffer) );
    memset (  ThreadBufferLen , 0 , sizeof(ThreadBufferLen) );
}
// ~TConsole
//-----------------------------------------------------

 TConsole::~TConsole()
{
    delete errorList;
    delete Synthese;
    delete consoleList;
}
//
// effacement de toute la console
//-------------------------------------------------------
void TConsole::Clear()
{
    consoleList->Clear();
    errorList->Clear();
    Synthese->Clear();
}

T_StringList * converToRtf(T_StringList * inp)
{
    std::string  St;
    T_StringList *rftList = new T_StringList();
    rftList->Add("{\\rtf1\\ansi\\ansicpg1252\\deff0\\deflang1036{\\fonttbl{\\f0\\fnil\\fcharset0 Courier New;}}");
    rftList->Add("{\\colortbl ;\\red0\\green0\\blue0;\\red255\\green0\\blue0;}");
    rftList->Add("\\viewkind4\\uc1\\pard\\cf1\\b\\f0\\fs20");
    
    for (int i=0;i<inp->Count();i++)
    {
        St = inp->Strings(i);
//        char *line = inp->Strings[i].c_str() ;
        char *line = (char*)St.c_str() ;
        while (*line )
        {
             if (*line=='\\')
                *line='/';
             line++;
        }
        //delete \n
        line--;
        if (*line=='\n')
            *line=' ' ;

        if (St.size()>3)
        {
//            line = inp->Strings[i].c_str() ;
            if ( Console->isErrorLine(St.c_str()) )
                rftList->Add( std::string ("\\par \\cf2")+ St ) ;
            else
                rftList->Add( std::string ("\\par \\cf1")+ St ) ;
         }
    }
    rftList->Add("\\par }");

    return (rftList);

}
void TConsole::Save(std::string  fileName)
{
  int ok=0 ;
  do 
  {
    try {

      T_StringList *rtfList = converToRtf ( getConsoleContent() );
      rtfList->SaveToFile(fileName+".rtf");
      delete rtfList ;
      rtfList = converToRtf ( Synthese );
      rtfList->SaveToFile(fileName+".res");
      delete rtfList ;
      ok=0;
    }
    catch (std::exception  &e)
    {
      Console->errorPrintf( 0 , "File %s is already open or protected\n" , fileName.c_str() );
      ok++;
      fileName = fileName+to_string(ok);
    }
  }while ((ok!=0)&&(ok<10));

}

//defini le header de ligne a ajouter au debut
void TConsole::setHeaderLine (std::string  header)
{
    HeaderLine = header ;
}

int    TConsole::GetBufferSize(int bufferNb) 
{
  int len = MAXCHARBUFFER - ThreadBufferLen[bufferNb];
  if (len<0) len=0;
  return ( len ) ;

}
int    TConsole::GetBufferLen(int bufferNb) 
{
  return ( ThreadBufferLen[bufferNb] ) ;

}
void  TConsole::SetBufferLen(int bufferNb , int len ) 
{
  if (len<MAXCHARBUFFER)
    ThreadBufferLen[bufferNb]=len ;
}
char * TConsole::GetBufferAddr  (int bufferNb) 
{
  return ThreadBuffer[bufferNb] ;
}


int  TConsole::printf(TConsoleLevel level ,const char *fmt,va_list argptr)
{
//    struct  time t;
    int ret;
    int len ;

DWORD hd = GetCurrentThreadId()  ;
      hd = hd & 0xf;
      char * Buffer = GetBufferAddr(hd);

  time_t rawtime;
  struct tm * t;


    //ajout de l'heure
    len = GetBufferLen(hd);
    if (len ==0 )
    {
        //pas d'heure
        if (EnableLogTimeStamp)
        {
          time (&rawtime);
          t = localtime (&rawtime);
        }
        else
           memset(&t,0,sizeof(t) );

        len = _snprintf(Buffer,GetBufferSize(hd),"  %02d:%02d:%02d ",t->tm_hour, t->tm_min, t->tm_sec );
        SetBufferLen ( hd,len ) ;
    }
    if (len<MAXCHARBUFFER){
      len += vsnprintf(&Buffer[len],GetBufferSize(hd), fmt, argptr) ;
      SetBufferLen ( hd,len ) ;
    }
    len=GetBufferLen(hd);
    //si pas retour a la ligne
    if ( Buffer[len-1] !='\n' ) 
    {
        return (0);
    }
    Buffer[len-1] = 0 ; 

    //set display color to red
    if ( level == ERRORS )
        setErrorLine(Buffer);
    if ( level == WARNING )
        setWarningLine(Buffer);

    //si erreur on la memorise
    if ( level == ERRORS)
    {
      //errorList->Add( FileName + std::string(": line ") + to_string(LineNumber) + std::string(" ERROR:") + std::string(Buffer) );
     }


    if (ConsoleEnable)
        ret = Printf(Buffer,len );
    //consoleList->Add(Buffer);

    Buffer[0]=0;
    SetBufferLen ( hd,0 ) ;
    return ret;

}

int  TConsole::printf(const char *fmt,...)
{
    va_list argptr;
    va_start(argptr, fmt);

    int ret = printf(LOG,fmt,argptr);
    return ret ;

}
//return true if debug ouput messages is enable
bool TConsole::isDebugEnable()
{
    if(EnableDebugOutput & ENABLE_DEBUG)
      return true;
    else
      return false;
}
bool TConsole::isDebugEnable(int TraceLevel)
{
    if(EnableDebugOutput&TraceLevel)
      return true;
    else
      return false;
}
int  TConsole::debugPrintf(const char *fmt,...)
{
    va_list argptr;
    if (isDebugEnable())
    {
        va_start(argptr, fmt);
        return  printf(LOG,fmt,argptr);
    }
        
    return 0 ;
}
int  TConsole::debugPrintf(int TraceLevel , const char *fmt,...)
{
    va_list argptr;
    if (isDebugEnable(TraceLevel))
    {
        va_start(argptr, fmt);
        return  printf(LOG,fmt,argptr);
    }
        
    return 0 ;
}

int  TConsole::warningPrintf(const char *fmt,...)
{
    va_list argptr;
    va_start(argptr, fmt);
    return  printf(WARNING,fmt,argptr);
}

int  TConsole::errorPrintf(int errorCode, const char *fmt,...)
{
    va_list argptr;
    va_start(argptr, fmt);
    setError(errorCode);
    return  printf( ERRORS,fmt,argptr);
}

void TConsole::Flush()
{
DWORD hd = GetCurrentThreadId() ;
        hd = hd & 0xf;

    if (GetBufferLen(hd) !=0)
        printf("\n");
}
   /******************
     affiche date et heure   *
     *****************/
//------------------------------------------------------
void TConsole::DatePrint(char *commentaire,unsigned short date)
{
   printf("%s %d/%d/%d ", commentaire,
        date&0x1f,(date>>5)&0xf,((date>>9)&0x3f)+80);
}

void TConsole::HeurePrint(char *commentaire, unsigned short time)
{
    printf("%s %d:%d:%d ", commentaire,
        (time>>11)&0x1f,(time>>5)&0x3f,(time&0x1f)*2);
}

//
// Suspendre les affichages
//
void TConsole::Suspend()
{
    Suspendu = true;    // mémoriser la suspension
}
//
// Reprendre l'affichage
void TConsole::Resume()
{
    Suspendu = false;
}
void TConsole::ScrollBottom()
{
}

//disable the console output
void TConsole::Disable()
{
    ConsoleEnable=false;
}
//ebnable the console output
void TConsole::Enable()
{
    ConsoleEnable=true;
}

//enable/desable display  output
void TConsole::EnableDisplay(bool PEnableDisplay)
{
    DisplayEnable=PEnableDisplay;
}

bool TConsole::SetMultiThread(        // mode multi-thread
           bool modePipe ,         // mailslot si faux
           int priorite  ,             // -2..+2 les #define standards sont aussi utilisables
           unsigned long size         // 0= taille standard 4000 pour les mailslots 15000 pour les pipe
           )
{
        return true;
};

void TConsole::DisplayMessage(const char *fmt,...)
{
}
void TConsole::DisplayWait(const char *fmt,...)
{
}

//attente d'un caractere console
char TConsole::getCh()
{
    return (0);
}
int  TConsole::Printf(char * line , int len ) 
{
    //sortie standart
    if (line!=0){
        fprintf (stdout,line);
        fprintf (stdout,"\n");
		}
    return 0 ;
}

//print des erreurs
void TConsole::printErrorList()
{
            for (int i=0;i< errorList->Count() ;i++)
              printf("%s\n",errorList->Strings(i).substr (1,errorList->Strings(i).size()-1).c_str() );
}
//get du nombres des erreurs
int TConsole::getErrorCount()
{
            return  errorList->Count() ;
}


//print de la syntheses des tests
void TConsole::printSynthese(std::string  line )
{
        Synthese->Add ( line );
}

//print de la syntheses des tests
void TConsole::printSyntheseList()
{
            for (int i=0;i< Synthese->Count() ;i++)
            {
              if ( Synthese->Strings(i).substr(0,1) == "K" )
                errorPrintf(0,"%s\n",Synthese->Strings(i).c_str()  );
              else
                printf("%s\n",Synthese->Strings(i).c_str()  );

            }
}
//get du nombres des syntheses
int TConsole::getSyntheseCount()
{
            return  Synthese->Count() ;
}

void TConsole::setErrorLine(char * line) 
//indique que la ligne est une erreur
{
  line[1]='E' ;
}
void TConsole::setWarningLine(char * line) 
//indique que la ligne est une warning
{
  line[1]='W' ;
}
bool  TConsole::isErrorLine(const char * line) 
 //return true si  la ligne est une erreur
{
  return (line[1]=='E');
 
}
bool  TConsole::isWarningLine(const char * line) 
 //return true si  la ligne est une erreur
{
  return (line[1]=='W');
 
}

void  TConsole::clearErrorLine(char * line)
{
  line[1]=' ';
}

T_StringList * TConsole::getConsoleContent()
{
  return consoleList;
}
