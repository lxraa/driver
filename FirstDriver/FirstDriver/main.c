#include <ntddk.h>
#define IOCTL_TEST1 CTL_CODE(FILE_DEVICE_UNKNOWN,0x801,METHOD_BUFFERED,FILE_ANY_ACCESS)


UNICODE_STRING s_symlink_name = RTL_CONSTANT_STRING(L"\\??\\KillProcess");
UNICODE_STRING s_device_name = RTL_CONSTANT_STRING(L"\\Device\\MyReadDevice");
PDEVICE_OBJECT p_device;

PVOID testpool = NULL;
PVOID nontestpool = NULL;

BOOLEAN KillProcess(INT32 pid) {
	HANDLE h_process;
	//NTSTATUS status;
	OBJECT_ATTRIBUTES object_attr;
	CLIENT_ID cid;
	InitializeObjectAttributes(&object_attr, 0, 0, 0, 0);
	cid.UniqueProcess = (HANDLE)pid;
	cid.UniqueThread = 0;
	if (NT_SUCCESS(ZwOpenProcess(&h_process, PROCESS_ALL_ACCESS, &object_attr, &cid))) {
		ZwTerminateProcess(h_process, STATUS_SUCCESS);
		ZwClose(h_process);
		return TRUE;
	}
	return FALSE;

	
	return TRUE;
}

NTSTATUS Unload(PDRIVER_OBJECT driver) {
	IoDeleteSymbolicLink(&s_symlink_name);
	IoDeleteDevice(p_device);
	ExFreePool(testpool);
	DbgPrint(TEXT("unload driver 1988\n"));
	return STATUS_SUCCESS;
}


//NTSTATUS MyDriverRead(PDEVICE_OBJECT p_device, PIRP p_irp) {
//	NTSTATUS status;
//	PIO_STACK_LOCATION p_stack = IoGetCurrentIrpStackLocation(p_irp);
//	ULONG ul_read_length = p_stack->Parameters.Read.Length;
//	p_irp->IoStatus.Status = STATUS_SUCCESS;
//	p_irp->IoStatus.Information = ul_read_length;
//	memset(p_irp->AssociatedIrp.SystemBuffer, 0xAA, ul_read_length);
//	IoCompleteRequest(p_irp, IO_NO_INCREMENT);
//	DbgPrint(TEXT("over\n"));
//	return STATUS_SUCCESS;
//}
//
//NTSTATUS MyDriverCreate(PDEVICE_OBJECT p_device, PIRP p_irp) {
//	p_irp->IoStatus.Status = STATUS_SUCCESS;
//	p_irp->IoStatus.Information = 0;
//	IoCompleteRequest(p_irp, IO_NO_INCREMENT);
//	DbgPrint(TEXT("create this device\n"));
//	return STATUS_SUCCESS;
//}
//
//NTSTATUS MyDriverClose(PDEVICE_OBJECT p_device, PIRP p_irp) {
//
//}

NTSTATUS MajorApi(PDEVICE_OBJECT p_device,PIRP p_irp) {

	p_irp->IoStatus.Status = STATUS_SUCCESS;
	p_irp->IoStatus.Information = 0;
	IoCompleteRequest(p_irp, IO_NO_INCREMENT);
	DbgPrint("-------------------\n");
	return STATUS_SUCCESS;
}

NTSTATUS MajorControl(PDEVICE_OBJECT p_device, PIRP p_irp) {

	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION irps = IoGetCurrentIrpStackLocation(p_irp);
	ULONG in_len = irps->Parameters.DeviceIoControl.InputBufferLength;
	ULONG out_len = irps->Parameters.DeviceIoControl.OutputBufferLength;
	ULONG code = irps->Parameters.DeviceIoControl.IoControlCode;
	ULONG info = 0;
	switch (code) {
	case IOCTL_TEST1: {
		DbgPrint("enther the io\n");
		PINT32 in_buffer = (PINT32)p_irp->AssociatedIrp.SystemBuffer;
		DbgPrint("%d", *in_buffer);
		PUCHAR out_buffer = p_irp->AssociatedIrp.SystemBuffer;
		if (KillProcess(*in_buffer)) {
			memcpy(out_buffer, "success", 10);
		}
		else {
			memcpy(out_buffer, "fail", 10);
		}

	
		info = 10;

		DbgPrint("IOCTL_TEST\n");
		break;
	}
	default: {
		DbgPrint("error\n");
		status = STATUS_UNSUCCESSFUL;
		break;
	
	}
	
	}

	p_irp->IoStatus.Status = status;
	p_irp->IoStatus.Information = info;
	IoCompleteRequest(p_irp, IO_NO_INCREMENT);
	DbgPrint(TEXT("major control\n"));
	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path) {
	
	driver->DriverUnload = Unload;
	//driver->MajorFunction[IRP_MJ_CREATE] = MyDriverCreate;
	//driver->MajorFunction[IRP_MJ_CLOSE] = MyDriverClose;
	//driver->MajorFunction[IRP_MJ_READ] = MyDriverRead;
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) {
		driver->MajorFunction[i] = MajorApi;
	}
	driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MajorControl;

	NTSTATUS status = IoCreateDevice(driver, 0, &s_device_name, FILE_DEVICE_UNKNOWN, 0, TRUE, &p_device);
	if (!NT_SUCCESS(status)) {
		DbgPrint(TEXT("create device failed"));
		return STATUS_SUCCESS;
	}
	
	status = IoCreateSymbolicLink(&s_symlink_name, &s_device_name);
	if (!NT_SUCCESS(status)) {
		DbgPrint(TEXT("create symlink failed"));
		IoDeleteDevice(p_device);
		return STATUS_SUCCESS;
	}

	p_device->Flags |= DO_BUFFERED_IO;

	testpool = ExAllocatePool(PagedPool, 10);
	if (testpool != NULL) {
		memset(testpool, 0xcc, 10);
		for (int i = 0; i < 10; i++) {
			DbgPrint("%x", *((UCHAR *)testpool + i));
		}
	}

	return STATUS_SUCCESS;
}