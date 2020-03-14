#include<ntddk.h>
#include<intrin.h>
//rcx rdx r8 r9

EXTERN_C ULONG64 myAdd(ULONG a, ULONG b);
EXTERN_C VOID hookFunc(DWORD64 object_func, DWORD64 my_func, int n);

EXTERN_C VOID myRoutine(DWORD64 address);
PVOID hook_code;
NTSYSAPI NTSTATUS ZwOpenProcess(
	PHANDLE            ProcessHandle,
	ACCESS_MASK        DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PCLIENT_ID         ClientId
);
NTSTATUS unload(PDRIVER_OBJECT p_driver) {
	
	/*ExFreePool(hook_code);*/
	return STATUS_SUCCESS;
}


NTSTATUS myZwOpenProcess(PHANDLE process_handle,ACCESS_MASK desired_access,POBJECT_ATTRIBUTES object_attributes,PCLIENT_ID p_client_id) {

	DbgPrint("this is myzwopenprocess\n");
	return ZwOpenProcess(process_handle,desired_access,object_attributes,p_client_id);
}


NTSTATUS DriverEntry(PDRIVER_OBJECT p_driver, PUNICODE_STRING pu_string) {
	p_driver->DriverUnload = unload;
	/*ULONG64 b = myAdd(1, 2);
	DbgPrint("%d", b);*/
	UNICODE_STRING func_name;
	RtlInitUnicodeString(&func_name, L"ZwOpenProcess");
	PVOID address = MmGetSystemRoutineAddress(&func_name);
	if (address == NULL) {
		DbgPrint("Can not get the function address\n");
		return STATUS_UNSUCCESSFUL;
	}
	myRoutine((DWORD64)address);

	/*hookFunc((DWORD64)address, (DWORD64)myZwOpenProcess, 5);*/


	//hook_code = ExAllocatePool(NonPagedPool, 200);
	//
	////保存前5字节机器码
	//memcpy(hook_code, address, 5);
	////retn c3
	//char ret_code = { 0xc3 };
	//memcpy(((char *)hook_code) + 5, &ret_code, 1);
	////jmp e9
	//char jmp_code[5] = { 0xe9 ,0x00,0x00,0x00,0x00 };
	//memcpy(address, jmp_code, 5);

	return STATUS_SUCCESS;
}