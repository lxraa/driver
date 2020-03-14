#include <ntifs.h>
#include <ntddk.h>
PDEVICE_OBJECT p_device = NULL;
UNICODE_STRING symlink_name;
#define IOCTL_PRT CTL_CODE(FILE_DEVICE_UNKNOWN,0x800,METHOD_BUFFERED,FILE_ANY_ACCESS)

typedef struct {
	PHANDLE h[10];
	BOOLEAN f[10];
	ULONG number;
} ThreadVector,*PThreadVector;

typedef struct {
	ULONG process_id;
	ULONG number;
} Context,*PContext;

ThreadVector *pthread_vector;

NTSTATUS protectThread(PVOID context) {
	ULONG process_id = ((Context *)context)->process_id;
	ULONG number = ((Context *)context)->number;
	
	pthread_vector->f[number] = TRUE;
	LARGE_INTEGER inteval;
	inteval.QuadPart = -200000000;
	while (TRUE) {
		//+0x170 ExitTime         : _LARGE_INTEGER
		
		//+0x200 ObjectTable      : Ptr64 _HANDLE_TABLE
		//+0x328 ActiveThreads    : Uint4B

		DbgPrint("protecting:     %d    %d\n", number, process_id);
		KeDelayExecutionThread(KernelMode, FALSE, &inteval);
		PEPROCESS process = NULL;
		if (!NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)process_id, &process))) {
			DbgPrint("Open process failed \n");
			continue;
		}
		
		DbgPrint("******************%d\n", *((PUINT32)((DWORD64)process + 0x328)));
		if (*((PUINT32)((DWORD64)process + 0x328)) == 0) {
			DbgPrint("process %d has existed\n", process_id);
		}
		// +0x1f0 DebugPort        : Ptr64 Void
		DbgPrint("dbg port:%x", *((PUCHAR)process + 0x1f0));

		
		if (*((PUCHAR)process + 0x1f0) != 0) {
			DbgPrint("%d is debugged \n",process_id);
		}



		if (!pthread_vector->f[number]) {
			break;
		}
		
	}

	ExFreePool(context);
	return STATUS_SUCCESS;
}


NTSTATUS protector(ULONG process_id) {
	if (pthread_vector->number >= 10) {
		DbgPrint("Protected Process should not more than 10\n");
		return STATUS_UNSUCCESSFUL;
	}

	OBJECT_ATTRIBUTES obj = { 0 };
	
	PHANDLE ph_thread = (PHANDLE)ExAllocatePool(NonPagedPool,sizeof(HANDLE));
	InitializeObjectAttributes(&obj, NULL, OBJ_KERNEL_HANDLE, 0, NULL);

	Context *c = (Context *)ExAllocatePool(NonPagedPool,sizeof(Context));
	c->process_id = process_id;
	c->number = pthread_vector->number;

	if (NT_SUCCESS(PsCreateSystemThread(ph_thread, THREAD_ALL_ACCESS, &obj, NULL, NULL, protectThread, c))) {
		//此处需修改为原子操作
		//保存文件句柄
		pthread_vector->h[pthread_vector->number] = ph_thread;
		pthread_vector->number = pthread_vector->number + 1;
		return STATUS_SUCCESS;
	}
	else {
		return STATUS_UNSUCCESSFUL;
	}	
}


NTSTATUS defaultApi(PDEVICE_OBJECT p_device, PIRP p_irp) {
	p_irp->IoStatus.Status = STATUS_SUCCESS;
	p_irp->IoStatus.Information = 0;

	IoCompleteRequest(p_irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS ioControl(PDEVICE_OBJECT p_device, PIRP p_irp) {
	

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(p_irp);
	ULONG in_len = stack->Parameters.DeviceIoControl.InputBufferLength;
	ULONG out_len = stack->Parameters.DeviceIoControl.OutputBufferLength;
	ULONG code = stack->Parameters.DeviceIoControl.IoControlCode;
	switch (code) {
	case IOCTL_PRT: {
		DbgPrint("Got IOCTL_PRT message\n");
		ULONG process_id;
		memcpy(&process_id, p_irp->AssociatedIrp.SystemBuffer, sizeof(ULONG));
		DbgPrint("process id:%d try to protect it\n", process_id);
		protector(process_id);
		break;
	}
		
	default:
		break;
	}



	p_irp->IoStatus.Status = STATUS_SUCCESS;
	p_irp->IoStatus.Information = 0;
	IoCompleteRequest(p_irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS unload(PDRIVER_OBJECT p_driver) {
	if (pthread_vector->number != 0) {
		for (ULONG i = 0; i < pthread_vector->number; i++) {
			PHANDLE p = pthread_vector->h[i];
			PETHREAD pthread = NULL;
			//这里应该有错误处理
			DbgPrint("unloading %d\n", i);
			pthread_vector->f[i] = FALSE;

			ObReferenceObjectByHandle(*p, THREAD_ALL_ACCESS, *PsThreadType, KernelMode, &pthread, NULL);
			KeWaitForSingleObject(pthread, Executive, KernelMode, FALSE, NULL);
			ObDereferenceObject(pthread);
			ZwClose(*pthread_vector->h[i]);
			ExFreePool(pthread_vector->h[i]);
		}
	}
	
	
	IoDeleteSymbolicLink(&symlink_name);
	IoDeleteDevice(p_device);
	
	ExFreePool(pthread_vector);
	return STATUS_SUCCESS;
}


NTSTATUS DriverEntry(PDRIVER_OBJECT p_driver, PUNICODE_STRING pu_regpath) {

	pthread_vector = (struct ThreadVector *)ExAllocatePool(NonPagedPool,sizeof(ThreadVector));
	pthread_vector->number = 0;
	for (int i = 0; i < 10; i++) {
		pthread_vector->f[i] = FALSE;
	}

	p_driver->DriverUnload = unload;
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) {
		p_driver->MajorFunction[i] = defaultApi;
	}
	p_driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ioControl;
	UNICODE_STRING device_name;

	RtlInitUnicodeString(&device_name, L"\\Device\\ProcessProtector");
	if (!NT_SUCCESS(IoCreateDevice(p_driver, 0, &device_name, FILE_DEVICE_UNKNOWN, 0, TRUE, &p_device))) {
		DbgPrint("Create device failed\n");
		return STATUS_UNSUCCESSFUL;
	}
	
	RtlInitUnicodeString(&symlink_name, L"\\??\\ProcessProtector");
	if (!NT_SUCCESS(IoCreateSymbolicLink(&symlink_name, &device_name))) {
		DbgPrint("Create symlink failed\n");
		IoDeleteDevice(p_device);
		return STATUS_UNSUCCESSFUL;
	}


	return STATUS_SUCCESS;
}

