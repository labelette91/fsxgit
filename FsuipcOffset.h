#ifndef FSUIPCOFFSET 
#define FSUIPCOFFSET

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "fctlib.h"
#include <map>

#include "PMDG_NGX_SDK.h"

#define FIRST_OFFSET 0x6420

// definition of a map between event name and number 
typedef map< std::string ,int > T_Map_Int_String ;

static enum FIELD_TYPE {
	F_UNDEF  ,
	F_BYTE   ,
  F_WORD  ,
  F_SHORT ,
  F_DWORD , 
  F_FLT32 ,
  F_STR   
};

const int FieldSize[]={
  0,
	1,
  2,
  2,
  4,
  4,
  1,
};


int GetFieldType (std::string T )
{
  int ftype=0;
           if (T=="BYTE"  )ftype = F_BYTE ;
      else if (T=="WORD"  )ftype = F_WORD ;
      else if (T=="SHORT" )ftype = F_SHORT;
      else if (T=="DWORD" )ftype = F_DWORD;
      else if (T=="FLT32" )ftype = F_FLT32;
      else if (T=="STR"   )ftype = F_STR  ;  
  return ftype;
}

int GetFieldSize (int ftype )
{
	return FieldSize[ftype];
}

class  TFsuipc {
public:	
typedef struct				
{
  int Type ; //1 si float
  std::string Name;
	int Offset ;
	int Length ;
	int Len ;			//len Type
  double Value ;
  int Variable ;
} TFsuipcOffset  ;

#define MAXOFFSET 65535 

TFsuipcOffset FsuipcOffset[MAXOFFSET];

T_Map_Int_String Map_Offset ;

private:

struct PMDG_NGX_Data Ngx ;

std::vector <int> WatchedOffset;

void (*RefreshOutput) (int Variable , double Value ) ;

public:

const char * GetName(int var)
{
  return FsuipcOffset[var].Name.c_str() ;
}
bool Defined(int var)
{
  return (FsuipcOffset[var].Offset !=0) ;
}
int GetOffset(int var)
{
  return (FsuipcOffset[var].Offset ) ;
}
int GetLen(int var)
{
	return (FsuipcOffset[var].Len  ) ;
}

double GetValue(int offset, char * Buffer)
{
	int len = FsuipcOffset[offset].Length;
	int ofs = FsuipcOffset[offset].Offset; 
	int typ = FsuipcOffset[offset].Type ; 
	if (ofs==0)
		return 0;
	double value;
	if (typ==F_BYTE)
	{
		char * ptv  = (char*)&Buffer[ofs];
		value = * ptv;
	}
	else if ( (typ==F_WORD)|| (typ==F_SHORT) )
	{
		short  * ptv  = (short*)&Buffer[ofs];
		value = * ptv;
	}
	else 	if (typ==F_DWORD)
	{
		int  * ptv  = (int*)&Buffer[ofs];
		value = * ptv;
	}
	else 	if (typ==F_FLT32)
	{
		float  * ptv  = (float*)&Buffer[ofs];
		value = * ptv;
	}
	else 	if (typ==F_STR)
		value=Buffer[ofs];
	else 	
		value=0;

		
	return value;
}
void SetValue(int offset, double Value)
{
	FsuipcOffset[offset].Value =  Value;
}
double GetValue(int offset)
{
	return FsuipcOffset[offset].Value ;
}
void ReadFromFile(const char * fileName)
{
	std::string line;
	std::vector<std::string> FileArray;
	std::ifstream myfile(fileName);

	if(!myfile) //Always test the file open.
	{
		printf("Error opening input file %s \n",fileName);
		return ;
	}
	while (std::getline(myfile, line))
	{
      T_StringList * args = Split ( (char*)line.c_str() , (char*)";," , (char*)"" , true );
			if (args->Count() < 4)
				continue;
			int ofs = strToUInt((*args)[0].c_str(),16) /*- FIRST_OFFSET */;
			int len = strToUInt((*args)[1].c_str(),10) ;
			int ftype = -1 ; 
      std::string T = (*args)[2]  ;
      
      ftype = GetFieldType(T);
      
    	if (ftype==0)
				continue;

      int fsize =   FieldSize[ftype];
          
			std::string Name =  (*args)[3] ;
      for (int j=0;j<len/fsize ; j++)
      {      
  			for (int i=0;i<fsize;i++)
  			{
					char Is = ' ';
					if(len/fsize>1) 
          {
            Is = '1'+j;
            Name[Name.length()-3]='_';
            Name[Name.length()-2]=Is ;
            Name[Name.length()-1]=0  ;

          }
  				FsuipcOffset[ofs+i].Offset =ofs;
  				FsuipcOffset[ofs+i].Length = len;
  				FsuipcOffset[ofs+i].Len    = fsize;
  				FsuipcOffset[ofs+i].Name =  Name  ;
  				FsuipcOffset[ofs+i].Type =  ftype;

  			}
        Map_Offset[Name] = ofs;
        ofs+=fsize;
      }
			delete args ;
	}
}
void Print()
{
	int last = 0 ;
	printf ("Dump  FsuipcOffset\n");  
	for (int var=0;var<MAXOFFSET;var++)
	{
		if ( (FsuipcOffset[var].Offset !=0)&& (FsuipcOffset[var].Offset !=last) )
		{
//			Console->debugPrintf (  TRACE_SIOC , 
			printf (   
				"Var:%5d  len:%d offset:%04X (%5d) name:%-14s \n",  
				var,
				FsuipcOffset[var].Len,
				FsuipcOffset[var].Offset,
				FsuipcOffset[var].Offset,
				FsuipcOffset[var].Name.c_str() 
				);
			last = FsuipcOffset[var].Offset  ;
		}
	}

}

void DetectChanged (PMDG_NGX_Data *pS  )
{
//	char *   psByte    = (char*)pS;
//	char *   pNgxByte  = (char*)&Ngx;

	char *   psByte    = (char*)pS-FIRST_OFFSET;
	char *   pNgxByte  = (char*)&Ngx-FIRST_OFFSET;

//	for (int i=0;i<sizeof(struct PMDG_NGX_Data);i++)
//	for (int i=FIRST_OFFSET;i<FIRST_OFFSET+0x1b5;i++)
		for (int i=FIRST_OFFSET;i<FIRST_OFFSET+sizeof(struct PMDG_NGX_Data);i++)
	{
		if (Defined(i) )
			if (psByte[i]!=pNgxByte[i])
			{
        double Value = GetValue(i,psByte);
				printf("dif:%4d = %4d Name:%-20s, Value:%f\n",i,psByte[i],GetName(i),Value );
				SetValue(i , Value) ;
				i = GetOffset(i)+ GetLen(i); 
			}
	}
	Ngx = *pS ;
}

void WatchDetectChanged (PMDG_NGX_Data *pS  )
{
	char *   psByte    = (char*)pS-FIRST_OFFSET;
	char *   pNgxByte  = (char*)&Ngx-FIRST_OFFSET;

	for (unsigned int j=0;j<WatchedOffset.size();j++)
	{
    //get offset
      int offset = WatchedOffset[j];
  		if (Defined(offset) ) {
        int len = FsuipcOffset[offset].Len;
        for (int n=0;n<len;n++){
			    if (psByte[offset+n]!=pNgxByte[offset+n])
			    {
            double Value = GetValue(offset,psByte);
				    //printf("dif:%4d = %3d Name:%-20s, Value:%f\n",offset,psByte[offset],GetName(offset),Value );
				    SetValue(offset , Value) ;
						int variable = FsuipcOffset[offset].Variable ;
						if (variable)
							if (RefreshOutput)
								(*RefreshOutput)(variable,Value);
            break;
          }
        }
      }
	}
	Ngx = *pS ;
}

void SetRefreshOutputFct ( void (*RefreshOutputFct) (int variable,double Value) )
{
	RefreshOutput = RefreshOutputFct;
}
void RegisterToVariableChanged ( int offset , int Len , int variable )
{
  if (offset)
	{
		WatchedOffset.push_back(offset);
		FsuipcOffset[offset].Variable = variable ;
    Console->debugPrintf(TRACE_FSX_SEND,"FSX : register to offset %4x : %s \n",offset,GetOffsetName(offset).c_str() );

	}
}

int GetOffsetNum(std::string offsetName)
{
  return Map_Offset[offsetName] ;
}
std::string  GetOffsetName(int offsetNum )
{
  return FsuipcOffset[offsetNum].Name ;
}


};


//#define THIRD_PARTY_EVENT_ID_MIN 0x11000

class  TControlClass  {
public:	

typedef struct				
{
  std::string Name;
	int Offset ;

} TControl ;

#define MAXOFFSET 65535 

TControl Control[MAXOFFSET];

T_Map_Int_String Map_Control ;


const char * GetName(int var)
{
  return Control[var].Name.c_str() ;
}
bool Defined(int var)
{
  return (Control[var].Offset !=0) ;
}
void ReadFromFile(const char * fileName)
{
	std::string line;
	std::vector<std::string> FileArray;
	std::ifstream myfile(fileName);


	if(!myfile) //Always test the file open.
	{
		printf("Error opening input file %s \n",fileName);
		return ;
	}
	printf ("Dump  Control Event List\n");  

	while (std::getline(myfile, line))
	{
      T_StringList * args = Split ( (char*)line.c_str() , (char*)";," , (char*)"" , true );
			if (args->Count() < 2)
				continue;
      if ((*args)[0][0]=='/' )
				continue;

			int ofs = strToUInt((*args)[1].c_str(),10) ;
      int i = ofs- THIRD_PARTY_EVENT_ID_MIN;
          
      if ( i<MAXOFFSET )
      {
      std::string Name =  (*args)[0] ;

      Control[i].Offset =ofs;
      Control[i].Name =  Name  ;
      Map_Control[Name]=ofs;
      }
      else
	  		printf ("invalid offset : %s\n",line.c_str());  

      delete args ;
	}
}
void Print()
{
	int last = 0 ;
	for (int var=0;var<MAXOFFSET;var++)
	{
		if ( (Control[var].Offset !=0)&& (Control[var].Offset !=last) )
		{
//			Console->debugPrintf (  TRACE_SIOC , 
			printf (   
				"Var:%5d  offset:%04X (%5d) name:%-14s \n",  
				var,
				Control[var].Offset,
				Control[var].Offset,
				Control[var].Name.c_str() 
				);
			last = Control[var].Offset  ;
		}
	}
}

int GetEventNum(std::string eventName)
{
  return Map_Control[eventName] ;
}
std::string  GetEventName(int eventNum )
{
  if (eventNum>0)
    return Control[eventNum-THIRD_PARTY_EVENT_ID_MIN].Name ;
  else 
    return "";
}

};

extern TFsuipc Fsuipc;

extern TControlClass Event ;

#endif