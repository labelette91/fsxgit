class ClientTcp {

public:
	SOCKET               SendingSocket;
	std::string ServerIp;
	int Port;
  bool Connected;
  static void WinsockInit()
  {
		WSADATA              wsaData;
		// Initialize Winsock version 2.2
		WSAStartup(MAKEWORD(2,2), &wsaData);
		printf("Client: Winsock DLL status is %s.\n", wsaData.szSystemStatus);
  }
  static void WinsockCleanup()
  {
			// Do the clean up
			WSACleanup();
  }
	ClientTcp()
	{
	}
  bool Open()
  {
		// Create a new socket to make a client connection.
		// AF_INET = 2, The Internet Protocol version 4 (IPv4) address family, TCP protocol
		SendingSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(SendingSocket == INVALID_SOCKET)
		{
			 Console->printf("Client: socket() failed! Error code: %ld\n", WSAGetLastError());
       return true;
		}
    Connected=false;
    return false;
  }

	bool Connect  (char * pServerIp, int pPort )
	{
		// Server/receiver address
		SOCKADDR_IN          ServerAddr ;

		ServerIp   = pServerIp   ;
		Port =  pPort ;
		ServerAddr.sin_family = AF_INET;
		ServerAddr.sin_port = htons(pPort);
		ServerAddr.sin_addr.s_addr = inet_addr(pServerIp);

		// Make a connection to the server with socket SendingSocket.
		int RetCode = connect(SendingSocket, (SOCKADDR *) &ServerAddr, sizeof(ServerAddr));
		if(RetCode != 0)
		{
			 Console->printf("Client: connect() failed! Error code: %ld\n", WSAGetLastError());
      Connected=false;
			return false;
		}
		else
		{
      Connected=true;

			
			setTCPNoDelay(true);
		}
		return true;
	}
	void Close()
	{
		closesocket(SendingSocket);
	}
	int Send(char*buf , int len)
	{
		return   send(SendingSocket, buf, len, 0);
	}
	int SendString(char*buf )
	{
	  if (Connected )
    {
      Console->debugPrintf ( TRACE_SIOC_SEND , "SIOC :Send: %s",buf);
		  int Nb = Send(buf,strlen(buf));
			if (Nb==SOCKET_ERROR )
				Console->printf ("SIOC :tcp send error\n");

			return Nb ;
    }
    return 0;
	}

	int Recv (char*buf , int len)
	{
    int nb=0;
		memset(buf,0,len);
 	  if (Connected )
		  nb =  recv(SendingSocket, buf, len , 0);
		return nb;
	}

  bool setTCPNoDelay(bool enabled)
{

    int flag = (enabled ? 1 : 0);

    if(setsockopt(SendingSocket,IPPROTO_TCP,TCP_NODELAY,(char *)&flag,sizeof(flag)) == -1)
    {
         return false;
    }

    return true;
}
};