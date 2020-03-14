#include <ntifs.h>
#include <ntddk.h>

UCHAR *PsGetProcessImageFileName(PEPROCESS EProcess);

static ULONG imagefile_offset = 0xb8;

VOID processCallback(HANDLE parent_id, HANDLE process_id, BOOLEAN create) {
	if (create) {
		// +0x1f0 DebugPort        : Ptr64 Void
		PEPROCESS process = NULL;
		NTSTATUS stat = PsLookupProcessByProcessId(process_id, &process);
		UCHAR *process_image_filename = PsGetProcessImageFileName(process);
		//ULONG *process_image_filename = ((ULONG *)process) + imagefile_offset;
		ULONG offset = (ULONG *)process_image_filename - (ULONG *)process;
		if (NT_SUCCESS(stat)) {
			DbgPrint("create process\n");
			DbgPrint("offset %x\n", offset);
			DbgPrint("imagefilename:%s\n", (UCHAR *)((ULONG*)process + 0xb8));//(UCHAR)((ULONG)process)+imagefile_offset);
			DbgPrint("process DebugPort:%x\n", *((PUCHAR)process + 0x1f0));

			if (*((PUCHAR)process + 0x1f0) != 0) {
				DbgPrint("debuging.........");
			}
		}

	}
}


NTSTATUS unload(PDRIVER_OBJECT p_driver) {


	return PsSetCreateProcessNotifyRoutine(processCallback, TRUE);
}
NTSTATUS DriverEntry(PDRIVER_OBJECT p_driver, PUNICODE_STRING pu_regpath) {
	p_driver->DriverUnload = unload;
	


	return PsSetCreateProcessNotifyRoutine(processCallback, FALSE);;
}