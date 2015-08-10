#include <vector>

#include "arduino.h"


#define MAXINPUT 256 

class T_Switch {
  int SwitchValue[MAXINPUT];
public:
	T_Switch()
	{
		//all switch = 1 : pullup
		for (int i=0;i<MAXINPUT;i++)
			set(i,1);
	}
  void set(int sw , int value)
  {
    
    if(sw<MAXINPUT) SwitchValue[sw] = value;
  }
  int get(int sw )
  {
    return SwitchValue[sw] ;
  }

};

T_Switch Switch ;

typedef enum  {
 UNDEF          = 0,
 IOCARD_SW      = 1,
 IOCARD_OUT     = 2,
 IOCARD_DISPLAY = 3,
 IOCARD_ENCODER = 4,
 FSUIPC					= 5,
 IOCARD_SELECTOR= 6,
 IOCARD_PUSH_BTN= 7,
 IOCARD_SW_3P   = 8,
 IOCARD_SWAPER  = 9,

 END_TYPE

}EnumIoType;

char * IoTypeStr[] ={
 "IOCARD_UNDEF    ",
 "IOCARD_SW       ",      
 "IOCARD_OUT      ", 
 "IOCARD_DISPLAY  ", 
 "IOCARD_ENCODER  ",
 "       FSUIPC   ",
 "IOCARD_SELECTOR ",
 "IOCARD_PUSH_BTN ",
 "IOCARD_SW_3P    ", 
 "IOCARD_SWAPER   "

};

#define NBEVENT 2

typedef struct				
{
  EnumIoType  IOType ;
  int  Input;
  int  Output;
  int Digit ;
  int Numbers ;
  int Frac    ;
  int Aceleration ;
  std::string Type ;
  std::string name;
	int Offset ;
	int Length ;
	int Event[NBEVENT]  ;
  double Max;
  double Min;
  double Inc;
  double a  ;
  double b  ;
  std::string Codage ;
  int Var1 ;
  int Var2 ;
  std::string  FsxVariableName ;
  std::string  Unit;
  //(std::string  EventName)[NBEVENT] ;
  std::string EventName0 ;
  std::string EventName1 ;
} TVariable ;

#define MAXVAR 1000 

TVariable Var[MAXVAR];

//contain the varariable number of the corresponding input
int InputVar[MAXINPUT];

#define POSITIV 'p'
#define NEGATIV 'n'

EnumIoType GetVarIoType(int var)
{
  return Var[var].IOType ;
}
int GetVarEventId(int var,int evtNb=0)
{
  if (evtNb>=NBEVENT) evtNb=0;
  return Var[var].Event[evtNb] ;
}
void SetVarEventId(int var,int EventId,int evtNb=0)
{
  if (evtNb<NBEVENT) 
    Var[var].Event[evtNb] = EventId;
  
}
const char *  GetVarEventName(int var,int evtNb=0)
{
  if (evtNb>=NBEVENT) evtNb=0;
  if (evtNb==1) 
    return Var[var].EventName1.c_str() ;
  else
    return Var[var].EventName0.c_str() ;

}
void SetVarEventName(int var,std::string EventName,int evtNb=0)
{
  if (evtNb==1) 
    Var[var].EventName1 = EventName;
  else
    Var[var].EventName0 = EventName;

}
char GetVarType(int var)
{
	if (Var[var].Type.size()==0)
		Var[var].Type= POSITIV ;
  return Var[var].Type.c_str()[0] ;
}
int GetVarOutput(int var)
{
  return Var[var].Output ;
}
int GetVarInput(int var)
{
  return Var[var].Input ;
}
int GetVarDigit(int var)
{
  return Var[var].Digit ;
}
int GetVarNumbers(int var)
{
  return Var[var].Numbers ;
}
double GetVarMin(int var)
{
  return Var[var].Min ;
}
double GetVarMax(int var)
{
  return Var[var].Max ;
}
double GetVarInc(int var)
{
  return Var[var].Inc ;
}
double GetVarA(int var)
{
  return Var[var].a ;
}
double GetVarB(int var)
{
  return Var[var].b ;
}
int GetVarFrac(int var)
{
  return Var[var].Frac ;
}
int GetVarOffset(int var)
{
  return Var[var].Offset ;
}
const char * GetVarUnit(int var)
{
return Var[var].Unit.c_str() ;
}
const char * GetVarFsxVariableName(int var)
{
return Var[var].FsxVariableName.c_str() ;
}
std::string  GetVarCodage(int var)
{
return Var[var].Codage ;
}
int GetVarVar1(int var)
{
return Var[var].Var1 ;
}
int GetVarVar2(int var)
{
return Var[var].Var2 ;
}
//inverse value depending on the input type Negativ or Positiv 
int GetSwValue(int var , int SwValue)
{
  //if negativ polarity , inverse value
  if(GetVarType(var)==NEGATIV)
				return  !SwValue;
  else
				return SwValue ;
}
char * GetIOTypeStr(int pio)
{
  if (pio>=END_TYPE) pio=0;
  return &IoTypeStr[pio][7];
}
EnumIoType findIOType (std::string sType)
{
  for (int i=0;i<END_TYPE;i++)
  {
    if (strstr (sType.c_str(),IoTypeStr[i] ) !=0 )
      return (EnumIoType)i;
  }
  return UNDEF;

}
const char * GetVarName(int var)
{
  return Var[var].name.c_str() ;
}
int  GetVarFromName(const char *  varName)
{
  for (int i=0;i<MAXVAR;i++)
  {
    if ( strcmpi ( GetVarName(i),varName)==0)
      return i ;
  }
  return -1  ;
}
void SetInputVar(int Var,int pInput)
{
  if (pInput<MAXINPUT)
    InputVar[pInput] = Var;
}
//return the Sioc Var number from switch Inpiut Number
int GetInputVar ( int pInput )
{
  if (pInput>=MAXINPUT) pInput = 0 ;

  return InputVar[pInput];
 
}
void InitEventId(int var)
{
  Var[var].Event[0]  = NOT_DEFINED ;
  Var[var].Event[1]  = NOT_DEFINED ;
  Var[var].Offset    = NOT_DEFINED ;
  Var[var].a         = 1  ;
  Var[var].b         = 0  ;

}
//alocate an event depending PMDG or FSX event
void AllocEvent ( int var , std::string evtVame , int evtNb=0)
{
 	    const char * EventName = evtVame.c_str() ;
      if (evtVame.size())
		  {
        //recheche si event PMDG 
        int EvtId  = NOT_DEFINED ;
        if ( isalpha ( EventName[0] ) )
            EvtId =  Event.GetEventNum(EventName) ; 
          else
            EvtId = (int)strToInt(EventName,10 ) ;

        //not a pmdg control event
        if ( EvtId<= 0 )
        {
			    EvtId  = Event.Add(EventName,1.0,0.0);
        }
		    SetVarEventId(var, EvtId,evtNb);

		  }
}
void ReadSioc(const char * fileName)
{
	std::string line;
	std::vector<std::string> FileArray;
	std::ifstream myfile(fileName);
  int var=1;


	if(!myfile) //Always test the file open.
	{
		Console->errorPrintf (  0 ,"Error opening input file %s ",fileName);
		return ;
	}
	while (std::getline(myfile, line))
	{ 
		if ( line.c_str()[0]=='*')
			continue;
		if (    (strstr(line.c_str(),"IOCARD")!=0)
			   || (strstr(line.c_str(),"FSUIPC")!=0))
		{
      T_StringList * args = Split ( (char*)line.c_str() , (char*)" ,=" , (char*)"\"" , true );
			args->Add("");
      InitEventId(var) ;

      int i=0;
      while (i<args->Count()-1)
      {
			  //Var 0400, name I_CO, Link IOCARD_SW, Input 13, Type P
        //Var 0200, name O_DECIMAL, Link IOCARD_OUT, Output 20
			  //Var 0110, name D_COURSE2, Link IOCARD_DISPLAY, Digit 19, Numbers 3
        //Var 0310, name E_VS, Link IOCARD_ENCODER, Input 11, Aceleration 4, Type 2
				//Var 0002, Link FSUIPC_INOUT, Offset $07C0, Length 4     // AP_LVL
        std::string val1 = (*args)[i];
        std::string val2 = (*args)[i+1];
        toLowerString(val1);
        if (val1=="Var")
        {
          var = atoi(val2.c_str());
          InitEventId(var) ;
          i+=2;
          if ( Var[var].IOType!=UNDEF)
            			Console->errorPrintf (  0 , "Error : variiable already defined : %d\n", var  );

        }
        else if (val1=="name")
        {
          Var[var].name = val2 ;
          i+=2;
        }
        else if (val1=="link")
        {
          i+=2;
					toLowerString(val2);
					if (val2=="iocard_sw")
					{
						Var[var].IOType = IOCARD_SW;
						Var[var].Numbers = 1 ;
					}
					else if (val1=="iocard_push_btn")
					{
						Var[var].IOType = IOCARD_PUSH_BTN;
						Var[var].Numbers = 1 ;
					}
					else if (val2=="iocard_sw_3p")
					{
						Var[var].IOType = IOCARD_SW_3P;
						Var[var].Codage = "1023";
						Var[var].Numbers= 2 ;
					}
					else if (val2=="iocard_out")
					{
						Var[var].IOType = IOCARD_OUT;
					}
					else if (val2=="iocard_display")
					{
						Var[var].IOType = IOCARD_DISPLAY;
					}
					else if (val2=="iocard_swaper")
					{
						Var[var].IOType = IOCARD_SWAPER;
						Var[var].Numbers = 1 ;
					}
					else if (val2=="iocard_encoder")
					{
						Var[var].IOType = IOCARD_ENCODER;
						Var[var].Numbers = 2 ;
					}
					else if (val2=="iocard_selector")
					{
						Var[var].IOType = IOCARD_SELECTOR;
						Var[var].Numbers = 1 ;
					}
        }
        
				else if (val1=="coding")
        {
          if (val2.size()<4)
             Console->errorPrintf (0 , "error : coding length shall be 4 charaters [0..9] Var:%d Coding:%s\n",var,val2.c_str());
          else
          {
            for (unsigned int i=0;i<val2.size();i++)
            {
              if ( (val2[i]<'0')||(val2[i]>'9'))
              {
                Console->errorPrintf (0 , "error : coding charaters shall be in [0..9] Var:%d Coding:%s\n",var,val2.c_str());
              }
            }
          }
  
          Var[var].Codage = val2;
          i+=2;
        }
        else if (val1=="input")
        {
          Var[var].Input = atoi(val2.c_str()) ;
          SetInputVar( var,  Var[var].Input ) ;
          if ( (Var[var].IOType == IOCARD_ENCODER) || (Var[var].IOType == IOCARD_SW_3P) )
            SetInputVar( var,  Var[var].Input+1 ) ;

          i+=2;
        }
        else if (val1=="output")
        {
          Var[var].Output = atoi(val2.c_str()) ;
          i+=2;
        }
        else if (val1=="digit")
        {
          Var[var].Digit = atoi(val2.c_str()) ;
          i+=2;
        }
        else if (val1=="type")
        {
          Var[var].Type = val2.c_str() ;
          i+=2;
        }
        else if (val1=="numbers")
        {
          Var[var].Numbers = atoi(val2.c_str()) ;
          //rotary selector : 
          if (Var[var].IOType == IOCARD_SELECTOR)
            for (int i=0;i<Var[var].Numbers;i++)
              SetInputVar( var,  Var[var].Input+i ) ;

          i+=2;
        }
        else if (val1=="var1")
        {
          if ( isalpha ( val2[0] ) )
          {
            Var[var].Var1 = GetVarFromName(val2.c_str());
            if (Var[var].Var1<0)
            		Console->errorPrintf (  0 ,"Error: variable not found :%s line:%s\n",val2.c_str() , line.c_str() );
          }
          else
            Var[var].Var1 = (int)strToInt(val2.c_str(),10 ) ;
          i+=2;
        }
        else if (val1=="var2")
        {
          if ( isalpha ( val2[0] ) )
          {
            Var[var].Var2 = GetVarFromName(val2.c_str());
            if (Var[var].Var2<0)
            		Console->errorPrintf (  0 ,"Error: variable not found :%s line:%s\n",val2.c_str() , line.c_str() );
          }
          else
            Var[var].Var2 = (int)strToInt(val2.c_str(),10 ) ;
          i+=2;
        }
        else if (val1=="offset")
        {
          int offset ;
          if ( isalpha ( val2[0] ) )
            //recherche si variable PMDG
            offset = Fsuipc.GetOffsetNum(val2);
          else
            offset = strToUInt(val2.c_str(),16) ;
          i+=2;
          //if not found , variable FSX
          if (offset==NOT_DEFINED)
          {
            Var[var].FsxVariableName =val2;
            Fsuipc.Add (val2.c_str(),var);
            offset = Fsuipc.GetOffsetNum(val2);
          }

          Var[var].Offset = offset ;
          //init default min/max / could be redefined by Min/Max/Inc
          Var[var].Min = Fsuipc.GetMin(offset);
          Var[var].Max = Fsuipc.GetMax(offset);
          Var[var].Inc = Fsuipc.GetInc(offset);

        }
        else if (val1=="fsx")
        {
          Var[var].FsxVariableName =val2;
          Fsuipc.Add (val2.c_str(),var);
          Var[var].Offset = Fsuipc.GetOffsetNum(val2);
          int offset = Var[var].Offset ;
          Var[var].Min = Fsuipc.GetMin(offset);
          Var[var].Max = Fsuipc.GetMax(offset);
          Var[var].Inc = Fsuipc.GetInc(offset);

          i+=2;
        }
        else if (val1=="unit")
        {
          Var[var].Unit =val2;
          i+=2;
        }
        else if (val1=="length")
        {
          Var[var].Length = strToUInt(val2.c_str(),16) ;
          i+=2;
        }
        else if (val1=="max")
        {
          Var[var].Max = strToDouble(val2.c_str(),10 ) ;
          i+=2;
        }
        else if (val1=="min")
        {
          Var[var].Min = strToDouble(val2.c_str(),10 ) ;
          i+=2;
        }
        else if (val1=="event")
        {
          SetVarEventName(var,val2,0);
          AllocEvent ( var , val2  , 0 ) ;
          i+=2;
        }
        else if (val1=="event_inc")
        {
          SetVarEventName(var,val2,0);
          AllocEvent ( var , val2  , 0 ) ;
          i+=2;
        }
        else if (val1=="event_dec")
        {
          SetVarEventName(var,val2,1);
          AllocEvent ( var , val2  , 1 ) ;
          i+=2;
        }
        else if (val1=="inc")
        {
          Var[var].Inc = strToDouble(val2.c_str(),10 ) ;
          i+=2;
        }
        else if (val1=="a")
        {
          Var[var].a = strToDouble(val2.c_str(),10 ) ;
          if (Var[var].a==0) Var[var].a=1;
          i+=2;
        }
        else if (val1=="b")
        {
          Var[var].b = strToDouble(val2.c_str(),10 ) ;
          i+=2;
        }
        else if (val1=="frac")
        {
          Var[var].Frac = strToInt(val2.c_str(),10 ) ;
          i+=2;
        }
				else 
        {
					i++;
           Console->printf ("Warning : %s not decoded \n",val1.c_str() );
        }

      }
			delete args ;
      var++ ;

		}
	}
  myfile.close();

}
void PrintVars()
{
	Console->debugPrintf (  TRACE_SIOC ,"Dump  Variable List\n");  

  for (int var=0;var<MAXVAR;var++)
  {
    if (Var[var].IOType != UNDEF)
    Console->debugPrintf (  TRACE_SIOC , 
"Var:%4d IO:%s Port:%3d Nb:%1d Type:%1s name:%-12s len:%d ofs:%5d %-25s evt0:%5d %-25s evt1:%5d %-25s\n",  
var,
GetIOTypeStr(Var[var].IOType)     ,
Var[var].Input      ,
Var[var].Numbers    ,
/*Var[var].Aceleration,*/
Var[var].Type.c_str()       ,
Var[var].name.c_str() ,
Var[var].Length,
Var[var].Offset,
Fsuipc.GetOffsetName(Var[var].Offset).c_str(),
Var[var].Event[0],
Var[var].EventName0.c_str(),
Var[var].Event[1],
Var[var].EventName1.c_str()


);
  }
  Console->debugPrintf (  TRACE_SIOC , "Input configuration\n");

  for (int input=0;input<MAXINPUT;input++)
  {
		int SiocVar  = GetInputVar(input);

    if (SiocVar !=0)
    Console->debugPrintf (  TRACE_SIOC ,  "Input:%3d Var:%4d IO:%s Name:%-14s\n",input,SiocVar, GetIOTypeStr(GetVarIoType(SiocVar)), GetVarName(SiocVar)  );
  }

}
typedef  std::vector <int> T_VARLIST ;
void GetSiocVar(int IOType ,  T_StringList * vars )
{
	char str[1024];
  for (int var=0;var<MAXVAR;var++)
  {
    if (Var[var].IOType ==IOType)
		{
			itoa(var,str,10);
			vars->Add(str);
		}
  }
}
void GetSiocVar(int IOType ,  T_VARLIST & vars )
{
  for (int var=0;var<MAXVAR;var++)
  {
    if (Var[var].IOType ==IOType)
		{
			vars.push_back(var);
		}
  }
}
void SiocRegister()
{
	T_StringList * vars = new T_StringList();
	GetSiocVar(IOCARD_OUT ,  vars ) ;
	GetSiocVar(IOCARD_DISPLAY ,  vars ) ;
	std::string ArnInit = "Arn.Inicio:";
	for (int i=0;i<vars->Count();i++)
	{
		ArnInit+= (*vars)[i]  + ":" ;
	}
	ArnInit += "\r\n";
 	Tcp->SendString((char*)ArnInit.c_str());

}
bool		SiocSend ( int SiocVar , byte SwValue )
{
	char  ArnValue[1024];
	sprintf(ArnValue, "Arn.Resp:%d=%d\r\n",SiocVar,SwValue);
	int Nb = Tcp->SendString( ArnValue);
	return true;
}
//register encoder to Arduino card
void EncoderArduinoRegister()
{
	T_StringList * vars = new T_StringList();
	GetSiocVar(IOCARD_ENCODER ,  vars ) ;
	std::string ArnInit = "Arn.Inicio:";
	for (int i=0;i<vars->Count();i++)
	{

    //get sioc var number
    int  var = atoi((*vars)[i].c_str());
    //get input for encoder
    int input = GetVarInput(var);
    //define encoder input in arduino
    SendOutputCmd ( IOCARD_ENCODER_CMD , input ,  0 )    		;
	}

}
#define lVar Var[var]
void ReadSiocCsv(const char * fileName)
{
	std::string line;
	std::vector<std::string> FileArray;
	std::ifstream myfile(fileName);
  int var=1;
  std::string val1 ;
  std::string val2 ;

	if(!myfile) //Always test the file open.
	{
		Console->errorPrintf (  0 ,"Error opening input file %s ",fileName);
		return ;
	}
	std::getline(myfile, line) ;
	while (std::getline(myfile, line))
	{ 
		if ( line.c_str()[0]=='*')
			continue;
      T_StringList * args = Split ( (char*)line.c_str() , (char*)";," , (char*)"\"" , false );
			args->erase(0);//delete comentaire
			args->Add("");
      memset(&lVar,0,sizeof(lVar) ) ;
      lVar.Event[0]  = NOT_DEFINED ;
      lVar.Event[1]  = NOT_DEFINED ;
      lVar.Offset    = NOT_DEFINED ;
      lVar.a         = 1  ;
      lVar.b         = 0  ;

        if (val1=="Var")
        {
          var = atoi(val2.c_str());
          InitEventId(var) ;
          if (lVar.IOType!=UNDEF)
            			Console->errorPrintf (  0 , "Error : variiable already defined : %d\n", var  );

        }
        val2 =  ((*args)[0]); // if (val1=="name")
        lVar.name = val2 ;
        val2 = ToLowerString ((*args)[1]); //else if (val1=="link")

				if (val2=="iocard_sw")
				{
					lVar.IOType = IOCARD_SW;
					lVar.Numbers = 1 ;
				}
				else if (val2=="iocard_push_btn")
				{
					lVar.IOType = IOCARD_PUSH_BTN;
					lVar.Numbers = 1 ;
				}
				else if (val2=="iocard_sw_3p")
				{
					lVar.IOType = IOCARD_SW_3P;
					lVar.Codage = "1023";
					lVar.Numbers= 2 ;
				}
				else if (val2=="iocard_out")
				{
					lVar.IOType = IOCARD_OUT;
					lVar.Numbers= 0 ;
				}
				else if (val2=="iocard_display")
				{
					lVar.IOType = IOCARD_DISPLAY;
				}
				else if (val2=="iocard_swaper")
				{
					lVar.IOType = IOCARD_SWAPER;
					lVar.Numbers = 1 ;
				}
				else if (val2=="iocard_encoder")
				{
					lVar.IOType = IOCARD_ENCODER;
					lVar.Numbers = 2 ;
				}
				else if (val2=="iocard_selector")
				{
					lVar.IOType = IOCARD_SELECTOR;
					lVar.Numbers = 1 ;
				}

        val2 =  ((*args)[2]); //  else if (val1=="input")else if (val1=="output")else if (val1=="digit")
        if (val2.size())
        {
         lVar.Input = atoi(val2.c_str()) ;
         lVar.Output = atoi(val2.c_str()) ;
         lVar.Digit = atoi(val2.c_str()) ;
        }
        val2 = ToLowerString ((*args)[3]);  // else if (val1=="type")
        if (val2.size())
        {
         lVar.Type = val2.c_str() ;
        }
        val2 =  ((*args)[4]); //   else if (val1=="numbers")
        if (val2.size())
        {
         lVar.Numbers = atoi(val2.c_str()) ;
        }
        val2 =  ((*args)[5]); //        else if (val1=="frac")
        if (val2.size())
        {
         lVar.Frac = strToInt(val2.c_str(),10 ) ;
        }
        val2 =  ((*args)[6]); // else if (val1=="offset")
        if (val2.size())
        {
          int offset ;
          if ( isalpha ( val2[0] ) )
            //recherche si variable PMDG
            offset = Fsuipc.GetOffsetNum(val2);
          else
            offset = strToUInt(val2.c_str(),16) ;
          //if not found , variable FSX
          if (offset==NOT_DEFINED)
          {
           lVar.FsxVariableName =val2;
            Fsuipc.Add (val2.c_str(),var);
            offset = Fsuipc.GetOffsetNum(val2);
          }

         lVar.Offset = offset ;
          //init default min/max / could be redefined by Min/Max/Inc
         lVar.Min = Fsuipc.GetMin(offset);
         lVar.Max = Fsuipc.GetMax(offset);
         lVar.Inc = Fsuipc.GetInc(offset);

        }
        val2 =  ((*args)[7]); //        else if (val1=="unit")
        if (val2.size())
        {
         lVar.Unit =val2;
        }
        val2 =  ((*args)[8]); //        else if (val1=="event_inc")
        if (val2.size())
        {
          SetVarEventName(var,val2,0);
          AllocEvent ( var , val2  , 0 ) ;
        }
        val2 =  ((*args)[9]); //        else if (val1=="event_dec")
        if (val2.size())
        {
          SetVarEventName(var,val2,1);
          AllocEvent ( var , val2  , 1 ) ;
        }
        val2 =  ((*args)[10]); //else if (val1=="coding")
        if (val2.size())
        {
          if (val2.size()<4)
             Console->errorPrintf (0 , "error : coding length shall be 4 charaters [0..9] Var:%d Coding:%s\n",var,val2.c_str());
          else
          {
            for (unsigned int i=0;i<val2.size();i++)
            {
              if ( (val2[i]<'0')||(val2[i]>'9'))
              {
                Console->errorPrintf (0 , "error : coding charaters shall be in [0..9] Var:%d Coding:%s\n",var,val2.c_str());
              }
            }
          }
  
         lVar.Codage = val2;
        }
        val2 =  ((*args)[11]); //        else if (val1=="min")
        if (val2.size())
        {
         lVar.Min = strToDouble(val2.c_str(),10 ) ;
        }
        val2 =  ((*args)[12]); //        else if (val1=="max")
        if (val2.size())
        {
         lVar.Max = strToDouble(val2.c_str(),10 ) ;
        }
        val2 =  ((*args)[13]); //        else if (val1=="inc")
        if (val2.size())
        {
         lVar.Inc = strToDouble(val2.c_str(),10 ) ;
        }
        val2 =  ((*args)[14]); //        else if (val1=="a")
        if (val2.size())
        {
         lVar.a = strToDouble(val2.c_str(),10 ) ;
          if (lVar.a==0)lVar.a=1;
        }
        val2 =  ((*args)[15]); //        else if (val1=="b")
        if (val2.size())
        {
         lVar.b = strToDouble(val2.c_str(),10 ) ;
        }

        val2 =  ((*args)[16]); //  else if (val1=="var1")
        if (val2.size())
        {
          if ( isalpha ( val2[0] ) )
          {
           lVar.Var1 = GetVarFromName(val2.c_str());
            if (lVar.Var1<0)
            		Console->errorPrintf (  0 ,"Error: variable not found :%s line:%s\n",val2.c_str() , line.c_str() );
          }
          else
           lVar.Var1 = (int)strToInt(val2.c_str(),10 ) ;
        }
        val2 =  ((*args)[17]); //else if (val1=="var2")
        if (val2.size())
        {
          if ( isalpha ( val2[0] ) )
          {
           lVar.Var2 = GetVarFromName(val2.c_str());
            if (lVar.Var2<0)
            		Console->errorPrintf (  0 ,"Error: variable not found :%s line:%s\n",val2.c_str() , line.c_str() );
          }
          else
           lVar.Var2 = (int)strToInt(val2.c_str(),10 ) ;
        }

        //if ewitch type init input Varaible
        if (    (lVar.IOType == IOCARD_SW)
            ||  (lVar.IOType == IOCARD_ENCODER)
            ||  (lVar.IOType == IOCARD_SELECTOR)
            ||  (lVar.IOType == IOCARD_PUSH_BTN)
            ||  (lVar.IOType == IOCARD_SW_3P)
            ||  (lVar.IOType == IOCARD_SWAPER)
            )
            for (int i=0;i<lVar.Numbers;i++)
              SetInputVar( var, lVar.Input+i ) ;
			delete args ;
      var++ ;

	}
  myfile.close();
}