#include<ntifs.h>
#include<ntddk.h>
UCHAR *PsGetProcessImageFileName(PEPROCESS EProcess);
NTSTATUS processRoutine(HANDLE h_parent_id,HANDLE h_process_id,BOOLEAN create) {
	
	DbgPrint("1\n");
	if (create) {
		DbgPrint("2\n");
		PEPROCESS process = NULL;
		NTSTATUS stat = PsLookupProcessByProcessId(h_process_id, &process);
		if (NT_SUCCESS(stat)) {
			DbgPrint("%s", PsGetProcessImageFileName(process));
			/*DbgPrint("3\n");
			for (int i = 0; i < 8 * PAGE_SIZE; i++) {
				if (!strncmp("notepad.exe", (PCHAR)process + i, strlen("notepad.exe"))) {
					DbgPrint("Process name :%s", (PCHAR)((ULONG)process + i));
					break;
				}
			}*/
		}

	}

}

NTSTATUS unload(PDRIVER_OBJECT p_driver) {


	return PsSetCreateProcessNotifyRoutine(processRoutine, TRUE);

}

NTSTATUS DriverEntry(PDRIVER_OBJECT p_driver, PUNICODE_STRING pu_regpath) {
	DbgPrint("driver start\n");
	p_driver->DriverUnload = unload;

	return PsSetCreateProcessNotifyRoutine(processRoutine, FALSE);

}