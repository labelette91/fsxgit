//------------------------------------------------------------------------------
//
//  PMDG 737NGX external connection sample 
// 
//------------------------------------------------------------------------------
#include "PMDG_NGX_SDK.h"

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <string>
#include <stdlib.h>
#include "simconnect.h"

int     quit = 0;
HANDLE  hSimConnect = NULL;
bool    AircraftRunning = false;		
PMDG_NGX_Control Control; 

static enum DATA_REQUEST_ID {
	DATA_REQUEST,
	CONTROL_REQUEST,
	AIR_PATH_REQUEST
};

static enum EVENT_ID {
	EVENT_SIM_START,	// used to track the loaded aircraft
};

// this variable keeps the state of one of the NGX switches
// NOTE - add these lines to <FSX>\PMDG\PMDG 737 NGX\737NGX_Options.ini: 
//
//[SDK]
//EnableDataBroadcast=1
//
// to enable the data sending from the NGX.

#include "FsuipcOffset.h"
TFsuipc Fsuipc;

TControlClass Event ;

// This function is called when NGX data changes
void ProcessNGXData (PMDG_NGX_Data *pS)
{
//	Fsuipc.DetectChanged (pS );
	Fsuipc.WatchDetectChanged (pS );
}

void CALLBACK MyDispatchProc(SIMCONNECT_RECV* pData, DWORD cbData, void *pContext)
{
	switch(pData->dwID)
	{
	case SIMCONNECT_RECV_ID_CLIENT_DATA: // Receive and process the NGX data block
		{
			SIMCONNECT_RECV_CLIENT_DATA *pObjData = (SIMCONNECT_RECV_CLIENT_DATA*)pData;

			switch(pObjData->dwRequestID)
			{
			case DATA_REQUEST:
				{
					PMDG_NGX_Data *pS = (PMDG_NGX_Data*)&pObjData->dwData;
					ProcessNGXData(pS);
					break;
				}
			case CONTROL_REQUEST:
				{
					// keep the present state of Control area to know if the server had received and reset the command
					PMDG_NGX_Control *pS = (PMDG_NGX_Control*)&pObjData->dwData;
					Control = *pS;
					break;
				}
			}
			break;
		}

	case SIMCONNECT_RECV_ID_EVENT:	
		{
			SIMCONNECT_RECV_EVENT *evt = (SIMCONNECT_RECV_EVENT*)pData;
			switch (evt->uEventID)
			{
			case EVENT_SIM_START:	// Track aircraft changes
				{
					HRESULT hr = SimConnect_RequestSystemState(hSimConnect, AIR_PATH_REQUEST, "AircraftLoaded");
					break;
				}
			}
			break;
		}

	case SIMCONNECT_RECV_ID_SYSTEM_STATE: // Track aircraft changes
		{
			SIMCONNECT_RECV_SYSTEM_STATE *evt = (SIMCONNECT_RECV_SYSTEM_STATE*)pData;
			if (evt->dwRequestID == AIR_PATH_REQUEST)
			{
				if (strstr(evt->szString, "PMDG 737") != NULL)
					AircraftRunning = true;
				else
					AircraftRunning = false;
			}
			break;
		}

	case SIMCONNECT_RECV_ID_QUIT:
		{
			quit = 1;
			break;
		}

	default:
		printf("\nReceived:%d",pData->dwID);
		break;
	}
}

void SendControl(int event , int pparameter )
{
	// Test the first control method: use the control data area.
	if (event!=0)
  if (AircraftRunning)
	{
		// Send a command only if there is no active command request and previous command has been processed by the NGX
		if (Control.Event != 0)
		{
			while(Control.Event != 0)
			{
				Sleep(1);
				SimConnect_CallDispatch(hSimConnect, MyDispatchProc, NULL);
			}
		}
		Control.Event = event;		
		Control.Parameter = pparameter;
		SimConnect_SetClientData (hSimConnect, PMDG_NGX_CONTROL_ID,	PMDG_NGX_CONTROL_DEFINITION, 
			0, 0, sizeof(PMDG_NGX_Control), &Control);
	}
}

std::string GetIdName(int id )
{
	char Id[128] ;
	_itoa_s(id,&Id[1],64,10);
	Id[0] = '#';	
	return std::string(Id); 
}
int GetIdOffset(int id )
{
	return id - THIRD_PARTY_EVENT_ID_MIN ;
}

void RefreshOutputFct (int variable,double Value)
{
	 printf("var:%4d  Value:%f\n",variable,Value );
}
DWORD WINAPI ThreadPMDG(LPVOID lpArg)
{
    HRESULT hr;
    printf("PMDG Thread running\n");   

//		Fsuipc.ReadFromFile("E:\\fsx\\tcp\\FsuipcOffset1.csv");
//		Fsuipc.Print();

// 		Event.ReadFromFile("E:\\fsx\\tcp\\event.csv");

		Fsuipc.SetRefreshOutputFct (RefreshOutput) ;

//		Fsuipc.RegisterToVariableChanged (268+FIRST_OFFSET,2,1); //MCP_Heading
		Fsuipc.RegisterToVariableChanged (275+FIRST_OFFSET,1,2); //MCP_FDSw[2]
		Fsuipc.RegisterToVariableChanged (280+FIRST_OFFSET,1,3); //MCP_annunFD[2]

    if (SUCCEEDED(SimConnect_Open(&hSimConnect, "PMDG NGX Test", NULL, 0, 0, 0)))
    {
        printf("Connected to Flight Simulator!\n");   
        
		// 1) Set up data connection

        // Associate an ID with the PMDG data area name
		hr = SimConnect_MapClientDataNameToID (hSimConnect, PMDG_NGX_DATA_NAME, PMDG_NGX_DATA_ID);

        // Define the data area structure - this is a required step
		hr = SimConnect_AddToClientDataDefinition (hSimConnect, PMDG_NGX_DATA_DEFINITION, 0, sizeof(PMDG_NGX_Data), 0, 0);

        // Sign up for notification of data change.  
		// SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_CHANGED flag asks for the data to be sent only when some of the data is changed.
		hr = SimConnect_RequestClientData(hSimConnect, PMDG_NGX_DATA_ID, DATA_REQUEST, PMDG_NGX_DATA_DEFINITION,SIMCONNECT_CLIENT_DATA_PERIOD_ON_SET, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_CHANGED, 0, 0, 0);
		
		// 2) Set up control connection

		// First method: control data area
		Control.Event = 0;
		Control.Parameter = 0;

		// Associate an ID with the PMDG control area name
		hr = SimConnect_MapClientDataNameToID (hSimConnect, PMDG_NGX_CONTROL_NAME, PMDG_NGX_CONTROL_ID);

		// Define the control area structure - this is a required step
		hr = SimConnect_AddToClientDataDefinition (hSimConnect, PMDG_NGX_CONTROL_DEFINITION, 0, sizeof(PMDG_NGX_Control), 0, 0);
        
		// Sign up for notification of control change.  
		hr = SimConnect_RequestClientData(hSimConnect, PMDG_NGX_CONTROL_ID, CONTROL_REQUEST, PMDG_NGX_CONTROL_DEFINITION, 
			SIMCONNECT_CLIENT_DATA_PERIOD_ON_SET, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_CHANGED, 0, 0, 0);

		// Second method: Create event IDs for controls that we are going to operate
//		hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVT_MCP_HDG_SEL_SWITCH,	GetIdName(EVT_MCP_HDG_SEL_SWITCH).c_str());	 

		// 3) Request current aircraft .air file path
		hr = SimConnect_RequestSystemState(hSimConnect, AIR_PATH_REQUEST, "AircraftLoaded");

    // also request notifications on sim start and aircraft change
		hr = SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_START, "SimStart");

		// 4) Assign keyboard shortcuts

		// 5) Main loop
		int t=0;
		int fd=0;
    while( quit == 0 )
    {
	// receive and process the NGX data
        SimConnect_CallDispatch(hSimConnect, MyDispatchProc, NULL);
        Sleep(10);
				t++;
				if ((t % 500)==0){
//							slewHeadingSelector();
//					SendControl(EVT_MCP_FD_SWITCH_L , fd );
//					SendControl(EVT_MPM_AUTOBRAKE_SELECTOR , fd );
					if (fd == 0 )
							fd=1;
					else
						fd=0;						
				}
    } 
    hr = SimConnect_Close(hSimConnect);
  }
	else
		printf("Unable to connect!\n");
  return 0;
}
