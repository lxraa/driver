#include<ntddk.h>

extern POBJECT_TYPE *IoDriverObjectType;
PDRIVER_OBJECT kbddriver = NULL;


typedef NTSTATUS(*POriginRead)(PDEVICE_OBJECT p_device, PIRP pirp);
POriginRead origin_read = NULL;
//NTSTATUS ObReferenceObjectByName(PUNICODE_STRING object_name, ULONG attribute, PACCESS_STATE access_state, ACCESS_MASK desired_access, POBJECT_TYPE pobject_type,
//	KPROCESSOR_MODE access_mode, PVOID parse_context, PVOID *object);
NTKERNELAPI
NTSTATUS
ObReferenceObjectByName(
	__in PUNICODE_STRING ObjectName,
	__in ULONG Attributes,
	__in_opt PACCESS_STATE AccessState,
	__in_opt ACCESS_MASK DesiredAccess,
	__in POBJECT_TYPE ObjectType,
	__in KPROCESSOR_MODE AccessMode,
	__inout_opt PVOID ParseContext,
	__out PVOID *Object
);



NTSTATUS unload(PDRIVER_OBJECT p_driver) {
	DbgPrint("Unloaded");
	if (kbddriver != NULL) {
		p_driver->MajorFunction[IRP_MJ_READ] = origin_read;
	}
	
	return STATUS_SUCCESS;
}

NTSTATUS readDispatch(PDEVICE_OBJECT p_device,PIRP p_irp) {


	DbgPrint("-------read--------\n");
	return origin_read(p_device, p_irp);
}

//NTSTATUS majorApi(PDEVICE_OBJECT p_device, PIRP p_irp) {
//	return STATUS_SUCCESS;
//}
NTSTATUS DriverEntry(PDRIVER_OBJECT p_driver, UNICODE_STRING us_regpath) {
	
	
	//p_driver->DriverUnload = unload;
	UNICODE_STRING us_kbdname = RTL_CONSTANT_STRING(L"\\Driver\\Kbdclass");
	DbgPrint("start");
	/*for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) {
		p_driver->MajorFunction[i] = majorApi;
	}*/
	NTSTATUS stat = ObReferenceObjectByName(&us_kbdname,OBJ_CASE_INSENSITIVE,NULL,0,*IoDriverObjectType,KernelMode,NULL,(PVOID)&kbddriver);
	if (NT_SUCCESS(stat)) {
		ObDereferenceObject(kbddriver);
	} 
	else{
		DbgPrint("open driver failed\n");
		return STATUS_UNSUCCESSFUL;

	}
	
	origin_read = (POriginRead)p_driver->MajorFunction[IRP_MJ_READ];
	p_driver->MajorFunction[IRP_MJ_READ] = readDispatch;
	return STATUS_SUCCESS;
}
