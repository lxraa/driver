#include <ntddk.h>


extern POBJECT_TYPE *PsThreadType;

PETHREAD p_thread = NULL;
BOOLEAN thread_flag = FALSE;
NTSTATUS unload(PDRIVER_OBJECT p_object) {

	DbgPrint("unloading.......");
	thread_flag = TRUE;
	KeWaitForSingleObject(p_thread, Executive, KernelMode, FALSE, NULL);
	ObDereferenceObject(p_thread);
	return STATUS_SUCCESS;
}

NTSTATUS thread(PVOID p_context) {

	LARGE_INTEGER inteval;

	inteval.QuadPart = -2000000;
	while (TRUE) {
		DbgPrint("---------thread active---------\n");
		if (thread_flag) {
			break;
		}
		KeDelayExecutionThread(KernelMode, FALSE, &inteval);

	}
	DbgPrint("thread stoped\n");
	PsTerminateSystemThread(STATUS_SUCCESS);
	return STATUS_SUCCESS;
}

NTSTATUS createThread(PVOID target) {
	OBJECT_ATTRIBUTES obj = { 0 };
	HANDLE h_thread = { 0 };
	NTSTATUS status = STATUS_SUCCESS;
	InitializeObjectAttributes(&obj, NULL, OBJ_KERNEL_HANDLE, 0, NULL);
	status = PsCreateSystemThread(&h_thread, THREAD_ALL_ACCESS, &obj, NULL, NULL, thread, NULL);
	if (NT_SUCCESS(status)) {
		DbgPrint("thread created\n");
		status = ObReferenceObjectByHandle(h_thread, THREAD_ALL_ACCESS, *PsThreadType, KernelMode, &p_thread, NULL);
		ZwClose(h_thread);
		if (!NT_SUCCESS(status)) {
			thread_flag = TRUE;
		}
	}
	
	return status;

}

NTSTATUS DriverEntry(PDRIVER_OBJECT p_driver, PUNICODE_STRING pu_regpath) {
	p_driver->DriverUnload = unload;
	PVOID target = { 0 };
	createThread(target);

	return STATUS_SUCCESS;
}