class T_THREAD {
private:
	HANDLE hHandle;
	DWORD ThreadId;

public :
  T_THREAD( LPTHREAD_START_ROUTINE ThreadRoutine , LPVOID lpArgPtr )
  {
		hHandle = CreateThread(NULL,0,ThreadRoutine,lpArgPtr,0,&ThreadId);
		if (hHandle == NULL) {
			fprintf(stderr,"Could not create Thread\n");
		}
  }
  void WaitTerminate()
  {
    	WaitForSingleObject(hHandle,INFINITE);
  }

};