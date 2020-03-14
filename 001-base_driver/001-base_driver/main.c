#include<ntddk.h>

NTSTATUS unload(PDRIVER_OBJECT driver) {
	DbgPrint(TEXT("unloading..."));
	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver,PUNICODE_STRING registry_path) {
	driver->DriverUnload = unload;
	DbgPrint(TEXT("%ws"), registry_path->Buffer);
	return STATUS_SUCCESS;
}