#include<ntddk.h>
#include<intrin.h>
#include "vtsystem.h"

NTSTATUS unload(PDRIVER_OBJECT p_driver) {

	return STATUS_SUCCESS;
}




NTSTATUS DriverEntry(PDRIVER_OBJECT p_driver,PUNICODE_STRING pu_regpath) {
	
	p_driver->DriverUnload = unload;
	if (!NT_SUCCESS(checkContext())) {
		return STATUS_UNSUCCESSFUL;
	}
	if (!NT_SUCCESS(startVt())) {
		return STATUS_UNSUCCESSFUL;
	}
	createVmcs();
	setupVmcs();
	
	stopVt();
	
	return STATUS_SUCCESS;
}