#include <vector>


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
  int Aceleration ;
  std::string Type ;
  std::string name;
	int Offset ;
	int Length ;
	int Event[NBEVENT]  ;
  double Max;
  double Min;
  double Inc;
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

#define POSITIV 'P'
#define NEGATIV 'N'

bool AsFsxVariableDefinition(int var)
{
	return Var[var].FsxVariableName.size()!=0 ;
}
TVariable *  GetVariable(int var)
{
  return & Var[var] ;
}

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
  Var[var].Event[0]  = EVENTNOTFOUND ;
  Var[var].Event[1]  = EVENTNOTFOUND ;

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
        std::string val1 = (*args)[i];
        std::string val2 = (*args)[i+1];
//        toLowerString(val1);
//        toLowerString(val2);
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
        else if (val1=="Link")
        {
          i+=1;
        }
			  //Var 0400, name I_CO, Link IOCARD_SW, Input 13, Type P
        else if (val1=="IOCARD_SW")
        {
          Var[var].IOType = IOCARD_SW;
          i+=1;
        }
        else if (val1=="IOCARD_PUSH_BTN")
        {
          Var[var].IOType = IOCARD_PUSH_BTN;
          i+=1;
        }
        else if (val1=="IOCARD_SW_3P")
        {
          Var[var].IOType = IOCARD_SW_3P;
          Var[var].Codage = "1023";
          Var[var].Numbers= 2 ;
          i+=1;
        }
        else if (val1=="Coding")
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
        else if (val1=="Input")
        {
          Var[var].Input = atoi(val2.c_str()) ;
          SetInputVar( var,  Var[var].Input ) ;
          if ( (Var[var].IOType == IOCARD_ENCODER) || (Var[var].IOType == IOCARD_SW_3P) )
            SetInputVar( var,  Var[var].Input+1 ) ;

          i+=2;
        }
        else if (val1=="Type")
        {
          Var[var].Type = val2.c_str() ;
          i+=2;
        }
        //Var 0200, name O_DECIMAL, Link IOCARD_OUT, Output 20
        else if (val1=="IOCARD_OUT")
        {
          Var[var].IOType = IOCARD_OUT;

          i+=1;
        }
        else if (val1=="Output")
        {
          Var[var].Output = atoi(val2.c_str()) ;
          i+=2;
        }
			  //Var 0110, name D_COURSE2, Link IOCARD_DISPLAY, Digit 19, Numbers 3
        else if (val1=="IOCARD_DISPLAY")
        {
          Var[var].IOType = IOCARD_DISPLAY;
          i+=1;
        }
        else if (val1=="Digit")
        {
          Var[var].Digit = atoi(val2.c_str()) ;
          i+=2;
        }
        else if (val1=="Numbers")
        {
          Var[var].Numbers = atoi(val2.c_str()) ;
          //rotary selector : 
          if (Var[var].IOType == IOCARD_SELECTOR)
            for (int i=0;i<Var[var].Numbers;i++)
              SetInputVar( var,  Var[var].Input+i ) ;

          i+=2;
        }
        //Var 0310, name E_VS, Link IOCARD_ENCODER, Input 11, Aceleration 4, Type 2
        else if (val1=="IOCARD_ENCODER")
        {
          Var[var].IOType = IOCARD_ENCODER;
          Var[var].Numbers = 2 ;
          i+=1;
        }
        else if (val1=="IOCARD_SELECTOR")
        {
          Var[var].IOType = IOCARD_SELECTOR;
          Var[var].Numbers = 1 ;
          i+=1;
        }
        
        else if (val1=="IOCARD_SWAPER")
        {
          Var[var].IOType = IOCARD_SWAPER;
          Var[var].Numbers = 1 ;
          i+=1;
        }
        else if (val1=="Var1")
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
        else if (val1=="Var2")
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


        else if (val1=="Aceleration")
        {
          Var[var].Aceleration = atoi(val2.c_str()) ;
          i+=2;
        }
				//Var 0002, Link FSUIPC_INOUT, Offset $07C0, Length 4     // AP_LVL
        else if (val1=="FSUIPC_INOUT")
        {
          Var[var].IOType = FSUIPC;
          i+=1;
        }
        //pmdg offset
        else if (val1=="Offset")
        {
          if ( isalpha ( val2[0] ) )
            Var[var].Offset = Fsuipc.GetOffsetNum(val2);
          else
            Var[var].Offset = strToUInt(val2.c_str(),16) ;
          i+=2;
          int offset = Var[var].Offset ;
          //init default min/max / could be redefined by Min/Max/Inc
          Var[var].Min = Fsuipc.GetMin(offset);
          Var[var].Max = Fsuipc.GetMax(offset);
          Var[var].Inc = Fsuipc.GetInc(offset);

        }
        else if (val1=="Fsx")
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
        else if (val1=="Unit")
        {
          Var[var].Unit =val2;
          i+=2;
        }
        else if (val1=="Length")
        {
          Var[var].Length = strToUInt(val2.c_str(),16) ;
          i+=2;
        }
        else if (val1=="Max")
        {
          Var[var].Max = strToDouble(val2.c_str(),10 ) ;
          i+=2;
        }
        else if (val1=="Min")
        {
          Var[var].Min = strToDouble(val2.c_str(),10 ) ;
          i+=2;
        }
        else if (val1=="Event")
        {
          SetVarEventName(var,val2,0);
          i+=2;
        }
        else if (val1=="Event_Inc")
        {
          SetVarEventName(var,val2,0);
          i+=2;
        }
        else if (val1=="Event_Dec")
        {
          SetVarEventName(var,val2,1);
          i+=2;
        }
        else if (val1=="Inc")
        {
          Var[var].Inc = strToDouble(val2.c_str(),10 ) ;
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
}
void PrintVars()
{
	Console->debugPrintf (  TRACE_SIOC ,"Dump  Variable List\n");  

  for (int var=0;var<MAXVAR;var++)
  {
    if (Var[var].IOType != UNDEF)
    Console->debugPrintf (  TRACE_SIOC , 
"Var:%4d IO:%s In:%3d Out:%3d Dig:%3d Nb:%1d Type:%1s name:%-12s len:%d offs:%04X(%5d) %s evt0:%5d %s evt1:%5d %s\n",  
var,
GetIOTypeStr(Var[var].IOType)     ,
Var[var].Input      ,
Var[var].Output     ,
Var[var].Digit      ,
Var[var].Numbers    ,
/*Var[var].Aceleration,*/
Var[var].Type.c_str()       ,
Var[var].name.c_str() ,
Var[var].Length,
Var[var].Offset,
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
// Bit 7   = switch value
//bit 6..0 : switch number 0..127
#if SWITCH_NB_CAR == 1
int GetSwitchValue(byte * Buffer)
{
	return ((Buffer[0] & 0x80)!=0) ;
}
int GetSwitchNumber(byte * Buffer)
{
	return (Buffer[0] & 0x7F) ;
}
#else

int GetSwitchValue(byte * Buffer)
{
	return ((char)Buffer[1] ) ;
}
int GetSwitchNumber(byte * Buffer)
{
	return (Buffer[0] ) ;
}
#endif


//SwNumber : 0..127
//SwValue 0..1
void SetSwitchValue(byte * Buffer , byte SwNumber , byte SwValue  )
{
  if (SwValue)
	  Buffer[0] = (byte)0x80 ;
  else
	  Buffer[0] = 0   ;
  Buffer[0] |= SwNumber ;

}

//cmd   :  commande bit 7..4
//value :  value    bit 3..1
//OutputNumber 0..127

#define PRESENCE_CMD        (0x00|0x80)
//Led output
#define IOCARD_OUT_CMD      (0x10|0x80)
//display output
#define IOCARD_DISPLAY_CMD  (0x20|0x80)
//encoder define cmd
#define IOCARD_ENCODER_CMD  (0x30|0x80)


void SetOutputCmd ( byte * Buffer , byte cmd,byte OutputNumber , byte Value )
{
  Buffer[0] = cmd | Value ;
  Buffer[1] = OutputNumber & 0x7f ;
}
//return command IOCARD_OUT_CMD or IOCARD_DISPLAY_CMD
byte GetOutputCmd ( byte * Buffer)
{
  return (Buffer[0] & 0xF0 );
}
//return command value 0..15 
byte GetOutputValue ( byte * Buffer)
{
  return ( Buffer[0] & 0xF ) ;
}
//return output display number 0..127
byte GetOutputNumber ( byte * Buffer)
{
  return ( Buffer[1] );
}


void SendOutputCmd ( byte cmd,byte OutputNumber , byte Value )
{
    byte Buffer[16];
    SetOutputCmd ( Buffer , cmd , OutputNumber ,  Value ) ;
    int NbWr = Com.WriteAsync(Buffer, 2) ;
		if (cmd==IOCARD_OUT_CMD)
			Console->debugPrintf (  TRACE_RS232_SEND , "RS232:Send IOCARD_OUT     Cmd:%02x Output:%3d(0x%2X) Value:%d Buf:%02x%02x\n",cmd,OutputNumber,OutputNumber,Value, Buffer[0] , Buffer[1]  );
		else if (cmd==IOCARD_DISPLAY_CMD)
			Console->debugPrintf (  TRACE_RS232_SEND , "RS232:Send IOCARD_DISPLAY Cmd:%02x Output:%3d(0x%2X) Value:%d Buf:%02x%02x\n",cmd,OutputNumber,OutputNumber,Value, Buffer[0] , Buffer[1]  );
		else if (cmd==PRESENCE_CMD)
			Console->debugPrintf (  TRACE_RS232_SEND , "RS232:Send PRESENCE_CMD   Cmd:%02x Output:%3d(0x%2X) Value:%d Buf:%02x%02x\n",cmd,OutputNumber,OutputNumber,Value, Buffer[0] , Buffer[1]  );
		else
			Console->debugPrintf (  TRACE_RS232_SEND , "RS232:Send IOCARD_ENCODER Cmd:%02x Output:%3d(0x%2X) Value:%d Buf:%02x%02x\n",cmd,OutputNumber,OutputNumber,Value, Buffer[0] , Buffer[1]  );

}


byte GetOutputCmd ( char * Buffer  )
{
  return (Buffer[0] & 0xF0 );
}
byte GetOutputValue( char * Buffer  )
{
  return (Buffer[0] & 0xF );
}
byte GetOutputNumber( char * Buffer  )
{
  return (Buffer[1] & 0x7F );
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
