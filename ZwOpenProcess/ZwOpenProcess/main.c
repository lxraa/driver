#include<ntddk.h>
NTSTATUS unload(PDRIVER_OBJECT p_driver) {


	return STATUS_SUCCESS;
}


NTSTATUS DriverEntry(PDRIVER_OBJECT p_driver, PUNICODE_STRING pu_regpath) {
	p_driver->DriverUnload = unload;
	PHANDLE p;
	ZwOpenProcess()


	return STATUS_SUCCESS;
}