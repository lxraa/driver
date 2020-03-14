#include<ntddk.h>

NTSTATUS unload(PDRIVER_OBJECT p_driver) {



}


NTSTATUS DriverEntry(PDRIVER_OBJECT p_driver, PUNICODE_STRING pu_regpath) {
	p_driver->DriverUnload = unload;
	

	return STATUS_SUCCESS;

}