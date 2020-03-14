#include <ntddk.h>
NTSTATUS unload(PDRIVER_OBJECT p_driver,PUNICODE_STRING pu_regpath) {
	
	
	return STATUS_SUCCESS;
}

NTSTATUS defaultDispatch(PDEVICE_OBJECT p_device,PIRP p_irp) {

	return STATUS_SUCCESS;
}

NTSTATUS deviceControl(PDEVICE_OBJECT p_device, PIRP p_irp) {
	return STATUS_SUCCESS;
}
NTSTATUS readDispatch(PDEVICE_OBJECT p_device,PIRP p_irp) {

}

NTSTATUS DriverEntry(PDRIVER_OBJECT p_driver, PUNICODE_STRING pu_regpath) {
	p_driver->DriverUnload = unload;
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) {
		p_driver->MajorFunction[i] = defaultDispatch;
	}

	p_driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = deviceControl;

	p_driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = readDispatch;
	
}