//---------------------------------------------------------------------------
#ifndef as2H
#define as2H

#ifdef __cplusplus
//extern "C" {
#endif

int openas2 ( int numas2)                                      ;
int openas2 ( char * comStr );
int closeas2 ( int numas2)                                         ;
int sdbufas2 ( int Num_COM , int Nb_char ,  char * ptbuff )    ;
int sdstras2 ( int Num_COM , char * str )                      ;
int sdstrwaitas2 ( int Num_COM , char * str , int wait );
int readbufas2 ( int Num_COM , int Nb_char ,  char * ptbuff)   ;
char readcaras2 ( int Num_COM )                                ;
void clearrecas2 ( int Num_COM )                               ;
/* lecture de nbcar carateres ou aret sur caractere recu = carend */
int rdstras2 ( int Ano_port , int nbcar , char carend , char  *Apt_string );
void setspeedas2old (int com , long vit);
int  getcaras2 (int com)       ;
// if cts = 1 cts flow control
void setctsflowas2 ( int com , bool CTS) ;
void setspeedas2 ( int com , long speed , char parite , int nbdata ,int nbstop  );
void setCommTimeouts (int numas2, int readTimeOut ,int writeTimeOut) ;
bool isopenas2(int com);
#ifdef __cplusplus
//}
#endif


enum State_as2 { AS2_OK,AS2_ERROR };


//---------------------------------------------------------------------------
#endif
