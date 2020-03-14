#include <ntddk.h>



VOID loadImageCallback(PUNICODE_STRING pu_image_name, HANDLE h_process_id, PIMAGE_INFO p_info) {
	DbgPrint("load image,image name:%wZ\n", pu_image_name);
}


NTSTATUS unload(PDRIVER_OBJECT p_driver) {

	PsRemoveLoadImageNotifyRoutine(loadImageCallback);


	return STATUS_SUCCESS;
}


NTSTATUS DriverEntry(PDRIVER_OBJECT p_driver, PUNICODE_STRING pu_regpath) {
	p_driver->DriverUnload = unload;


	return PsSetLoadImageNotifyRoutine(loadImageCallback);
}