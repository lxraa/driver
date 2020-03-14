#include<ntddk.h>

NTSTATUS unload(PDRIVER_OBJECT p_driver) {
	return STATUS_SUCCESS;
}

NTSTATUS mdlWriteMem(PVOID p_base_address, PVOID p_write_data, SIZE_T size) {
	PMDL p_mdl = NULL;
	PVOID new_address = NULL;
	PMDL p_mdl = IoAllocateMdl(p_base_address, size, FALSE, TRUE, NULL);

	//p_mdl = MmCreateMdl(NULL, p_base_address, size);
	if (NULL == p_mdl) {
		return STATUS_UNSUCCESSFUL;
	}
	MmBuildMdlForNonPagedPool(p_mdl);
	new_address = MmMapLockedPages(p_mdl, KernelMode);
	DbgPrint("new address %p\n", new_address);

	if (NULL == new_address) {
		IoFreeMdl(p_mdl);
		return STATUS_UNSUCCESSFUL;
	}
	

	RtlCopyMemory(new_address, p_write_data, size);
	MmUnmapLockedPages(new_address, p_mdl);
	IoFreeMdl(p_mdl);
	return STATUS_SUCCESS;
}


NTSTATUS DriverEntry(PDRIVER_OBJECT p_driver, PUNICODE_STRING pu_regpath) {
	p_driver->DriverUnload = unload;
	DWORD64 gdt = 0xfffff80006d25000;
	PVOID data = ExAllocatePool(NonPagedPool, 10);
	memcpy(data, "12345678", 8);


	mdlWriteMem((PVOID)gdt, data, 8);


	return STATUS_SUCCESS;
}