/***************************************************************************
 * INTERFACE COMPOSANT : CONSOLE
 ***************************************************************************/
/* réutilisabilité : application VCL sous c++ builder
 *
 *                                                                         *
 *                                                                         *
 * description :
 *          Classe ajoutant à l'objet VCL la fonction printf
 *
 * utilisation :
 *          - créer un objet de type Tconsole, ou utiliser la console
 *               par défaut Console.
 *          - lui affecter un objet de type Tmemo (Console = memo;)
 *          - utiliser directement la fonction Console.printf(...)
 *          - si plusieurs thread accèdent à la console, sélectionner le mode
              multithread par la fonction SetMultiThread deux modes sont
              disponibles :
                 . mailslot : le thread appelant ne sera jamais bloqué mais
                      des messages peuvent se perdre s'il y en a de trop
                 . pipe : Tous les messages sont délivré, mais le thread appelant
                     peut être bloqué si la file est saturée


 *       Note : le module utilisateur peut ne pas importer la vcl
 *
 * Limitation :
        - Le mode multi-thread ne fonctionne qu'avec 128 consoles max
        - Les fonctions suspend et resume pour figer l'écran
            . a un bug avec le caractère \r qui permet d'effacer
              la ligne courante pour la remplacer
        - Ne pas utiliser le mode wordwrap (passage à la ligne automatique)
        sous windows 95/98, il y a risque de perte de ligne.      

 *
 * historique :                                                            *
 *                                                                         *
 *   date  | auteur | fiche  | nature de la modification                   *
 * --------+--------+--------+----------------------------                 *
 * 01/03/99|        |          création                                    *
 * 27/03/2000   correction d'un bug sur la fonction clear
 * 4/4/2000     Ajout du /r pour effacer une ligne
 * 20/11/2000   Extension du mode suspendu en monotâche
		Protection d'accès à la vcl en mode multithread
 ***************************************************************************/
//----------------------------------------------------------------------------//---------------------------------------------------------------------------
#ifndef consoleH
#define consoleH
//#include <stdarg.h>
//
#include <string>
#include "fctlib.h" 

typedef enum {LOG=0,WARNING=1,ERRORS } TConsoleLevel ;

#define MAXCONSOLEBUFFER 16
#define MAXCHARBUFFER    1024

class TConsole
{


public:
    bool Suspendu;                  // pour figer les affichages, actif uniquement en mode multi
    int ConsoleEnable;
    bool DisplayEnable;             //enable display message and wait
    int EnableDebugOutput ;  // variable pour dumper dans le fichier rs232.out les commandes gene
    bool EnableLogTimeStamp  ;

    TConsole() ;
    virtual ~TConsole();            // destructeur

    virtual void ScrollBottom();
    virtual void Suspend();                 // figer l'affichage
    virtual void Resume();                  // le relancer
    virtual void Flush();
    virtual void Clear();            // effacement de toutes les lignes
    virtual void Save(std::string  fileName);
    //disable the console output
    virtual void Disable();
    //enable the console output
    virtual void Enable();
    //return true if debug ouput messages is enable
    bool isDebugEnable();
    bool isDebugEnable(int TraceLevel);

    //enable/desable display  output
    void    EnableDisplay(bool PEnableDisplay);


    virtual  bool SetMultiThread(        // mode multi-thread
           bool modePipe=true,         // mailslot si faux
           int priorite=0,             // -2..+2 les #define standards sont aussi utilisables
           unsigned long size=0        // 0= taille standard 4000 pour les mailslots 15000 pour les pipe
           );

    virtual void DisplayMessage(const char *fmt,...);
    virtual void DisplayWait(const char *fmt,...);
    virtual char getCh();


    int printf(const char *format  // \n seulement en fin de chaîne
           ,...);   // retourne le nombre d'arguments

    int debugPrintf(const char *format  // \n seulement en fin de chaîne
           ,...);   // retourne le nombre d'arguments

    int  TConsole::debugPrintf(int TraceLevel , const char *fmt,...);

    int  errorPrintf(int errorCode, const char *fmt,...);

    int  warningPrintf(const char *fmt,...);


    void DatePrint(          // affichage de la date
        char *commentaire,   // commentaire préalable
        unsigned short date);

    void HeurePrint(         // affichage de l'heure
        char *commentaire,   // commentaire préalable
        unsigned short time);// affichage de l'heure


    void setHeaderLine (std::string  header);

    void printErrorList();

    void printSyntheseList()  ;

    int getErrorCount();

    int getSyntheseCount();

    void printSynthese(std::string  line );

    int printf(TConsoleLevel level , const char *fmt,va_list argptr);

    void setErrorLine(char * buffer) ; //indique que la ligne est une erreur
    bool  isErrorLine(const char * buffer) ; //return true si  la ligne est une erreur
    void  clearErrorLine(char * buffer) ; //return true si  la ligne est une erreur
    void  setWarningLine(char * line) ;
    bool  isWarningLine(const char * line) ;


    virtual T_StringList * getConsoleContent();
    
    void setLineNumber(int ln) {LineNumber=ln;};
    int  getLineNumber()       {return LineNumber;};
    void setFilename(char*fn){FileName=fn;};
    const char* getFilename()      {return FileName.c_str();};

    void EnableSetLogTimeStamp(bool  p ) {  EnableLogTimeStamp=p;};

protected:

    virtual int Printf(char * line , int len ) ;


    T_StringList *errorList;   // memorisation des erreurs
    T_StringList *Synthese;           // memorisation des synthese de tests

    char       ThreadBuffer [MAXCONSOLEBUFFER][MAXCHARBUFFER];
    int        ThreadBufferLen[MAXCONSOLEBUFFER];
    T_StringList *consoleList;   // memorisation des liogne consoles

    std::string  HeaderLine;
    std::string  FileName ;      //file name
    int LineNumber;

    int    TConsole::GetBufferSize(int bufferNb); 
    int    GetBufferLen(int bufferNb) ;
    void    SetBufferLen(int bufferNb, int len) ;
    char * GetBufferAddr  (int bufferNb) ;

};

extern TConsole *Console; // console par défaut



#define ConsoledebugPrintf1(arg1)                          if (Console->isDebugEnable()) Console->debugPrintf(arg1)
#define ConsoledebugPrintf2(arg1,arg2)                     if (Console->isDebugEnable()) Console->debugPrintf(arg1,arg2)
#define ConsoledebugPrintf3(arg1,arg2,arg3)                if (Console->isDebugEnable()) Console->debugPrintf(arg1,arg2,arg3)
#define ConsoledebugPrintf4(arg1,arg2,arg3,arg4)           if (Console->isDebugEnable()) Console->debugPrintf(arg1,arg2,arg3,arg4)
#define ConsoledebugPrintf5(arg1,arg2,arg3,arg4,arg5)      if (Console->isDebugEnable()) Console->debugPrintf(arg1,arg2,arg3,arg4,arg5)
#define ConsoledebugPrintf6(arg1,arg2,arg3,arg4,arg5,arg6) if (Console->isDebugEnable()) Console->debugPrintf(arg1,arg2,arg3,arg4,arg5,arg6)

//definition des trace level
//trace tout
#define ENABLE_DEBUG         (1l<<0)
//trace des control word des protocol ARINC 429 Acars/Mcdu/Printer
#define TRACE_CTRL_WORD      (1l<<1)
//trace des data word des protocol ARINC 429 Acars/Mcdu/Printer
#define TRACE_DATA_WORD      (1l<<2)
//trace des drivers Plateza/Stimuli
#define TRACE_DRIVER         (1l<<3)
//trace du contenu de la database de parametres
#define TRACE_DATABASE       (1l<<4)

//trace les DATA envoyées dans les fichiers DAR/QAR/AIRBUS .csv
#define TRACE_DAR_SEND_DATA  (1l<<5)
//trace les DATA lues  dans les fichiers DAR/QAR/AIRBUS .csv
#define TRACE_DAR_DATA       (1l<<6)

//---------------------------------------------------------------------------
#endif
