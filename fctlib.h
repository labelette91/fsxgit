/***************************************************************************
 * INTERFACE COMPOSANT : FCTLIB
 ***************************************************************************/
/* réutilisabilité : C
 *
 *                                                                         *
 *                                                                         *
 * description : diverses fonctions en C
	- decoupe : permet de découper une ligne mot à mot, les séparateurs sont
		supprimés
 * historique :                                                            *
 *                                                                         *
 *   date  | auteur | fiche  | nature de la modification                   *
 * --------+--------+--------+----------------------------                 *
 * 01/03/99|        |          création                                    *
 *                                                                         *
 ***************************************************************************/
//----------------------------------------------------------------------------//
#ifndef fctlibH
#define fctlibH

//#include <dir.h>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include<iostream>
#include<fstream>
#include<iomanip>
#include <algorithm>    // std::sort
#include<windows.h>


class T_StringList ;

#define AnsiString  std::string

// découpage d'une ligne, retourne la position suivante
extern char *decoupe(char *ligne,	// ligne analysée
	 char *mot,			// mot récupérer
	 char *sep);			// liste des séparateurs
extern char *decoupecsv(char *ligne,	// ligne analysée
	 char *mot,			// mot récupérer
     int MotLen ,       //nombre max caratere mot
	 char *sep);			// liste des séparateurs

char *decoupe(char *ligne, char *mot, int MotLen , char *separator, char * concatenator , bool AllowDuplicateSeparator);


// conversion de chaine vers décimale ou hexa (ex 0x123)      
extern unsigned int xatoi(char *str);
extern int ishexa(char *mot);
extern int fileexist(char *name);
extern int cvU08Bbin(char *mot);
extern int cvascbin(char v);
extern int cpdate(char *name1, char *name2);
extern int readdate(char *name, struct ffblk *pdate);
extern unsigned int swap_u32b(unsigned int  vi);
extern void swap_buf_u32b(unsigned int *bui, int len);

// Convert byte to two ascii-hex characters
extern void ctohex(char *buf, int c);
extern void fmtline(int addr, char *buf, int len, FILE*stream);
extern void fmtline1(int addr, char *buf, int len, int modblanc, int acsii, FILE*stream);

// modeblanc = nombre de caracteres
// ascii = 1 : dump ascii part
extern void dump_bytes(unsigned char *bytes, int count, int modblanc, int acsii, FILE*stream);
extern void CvHexaBinaire(unsigned long Val, char *message,int decimalPos);
extern void CvHexaBinaire(unsigned long Val, char *message, int decimalPos , int nbBits);

extern void intoalib (  unsigned long vaconv ,  char *ptres, int nbchar,int base );

#ifndef SWAPLWORD
#define SWAPLWORD(src,dest) *(U32B *)(&dest) = swap_u32b(*(U32B *)(&src));
#endif

extern int dscanf ( char *  Ligne , char * fmt , ... );

extern int CreateMultimediaTimer (void*  procedure , int periodeInMs);

extern HANDLE createWaitableTimer ( int periodeMs )              ;
extern HANDLE createWaitableTimer ( int periodeMs , int NextDelayInMs );

extern void waitWaitableTimer ( HANDLE numTimer , int periodeMs);

extern T_StringList * Split ( char * ptligne , char * separateur , char * concatenator , bool AllowDuplicateSeparator );

extern void Split ( char * ptligne , char * separateur , char * concatenator , bool AllowDuplicateSeparator , T_StringList * StrL);

int  GetEnumValueFromeName ( char * enumerate , char * name );

AnsiString GetEnumName ( char * enumerate , int value );

/* recherche la valeur d'un enum a partir de la chaine nom */
/* l'enum est de la forme OPEN:1,CLOSE:3,.., */
// GetEnumName ( "OPEN:1,CLOSE:3,", "CLOSE" ) renvoie 3
AnsiString  GetEnumString ( char * enumerate , char * name );

void __fastcall  launchProcess(char * cmdLine , long timeOut /*in ms or INFINITE */)  ;

extern unsigned int  cvU32Bintel_motorola(unsigned int  val);

extern int Atoi(const char *s);

extern double Atof(const char *s);
//la procedure met a jour la derniere erreur
// 0 = pas d'erreur
void setError ( int error );

//la procedure lit la derniere erreur et la remet a zero
// 0 = pas d'erreur
int getLastError (  );

extern void ctohex(char *buf, int c);

extern int  StrToIntEx  (const char *str , int base );
extern int  strToInt  (const char *str , int base );
extern bool StrToInt (const char *str , int base , int * result );

bool         StrToUInt  (const char *str , int base , unsigned int * result );
unsigned int StrToUIntEx  (const char *str , int base );
unsigned int strToUInt  (const char *str , int base );

bool     StrToDouble   (const char *str , int base , double * result );
double   StrToDoubleEx   (const char *str , int base );
double   strToDouble   (const char *str , int base );


extern unsigned long FileSize(char * name);



HANDLE createEvent (char * name) ;
void closeEvent ( HANDLE Event ) ;
bool waitEvent ( HANDLE Event , int TimeOut );
bool pulseEvent(HANDLE Event);

std::string BuildFilePath(char * CurDir , char * fileName );

typedef std::vector<std::string> T_String_Vector ;

class T_StringList {

protected:

        void __fastcall SetStrings ( unsigned int index  , char *  val );
        const char *  __fastcall GetStrings ( unsigned int index ){return list[index].c_str(); };
        int GetCount() {return list.size();};

public:

T_String_Vector list ;

T_StringList();

std::string  __fastcall operator [](const int i)
{            return list[i];
};

std::string Strings(unsigned int index)  {return GetStrings(index); };

int Count()  {return GetCount(); };


int  __fastcall Add(char * S) ;
int __fastcall  Add(std::string S);
void __fastcall LoadFromFile(char *  FileName);
void __fastcall SaveToFile(char *  FileName);
void __fastcall SaveToFile(std::string FileName)
{
SaveToFile((char*)FileName.c_str());
};
void __fastcall Sort();
bool __fastcall find(std::string & value) ;
void Clear() {list.clear();};
void erase(int position) {list.erase(list.begin()+position);}

};


std::string to_string(int val);

void toUpperString( std::string & Str );
void toLowerString( std::string & Str );
std::string ToLowerString( std::string  Str );
std::string ToUpperString( std::string  Str );

#endif

