#include <ntddk.h>



NTSTATUS unload(PDRIVER_OBJECT p_driver) {

	return STATUS_SUCCESS;
}



NTSTATUS DriverEntry(PDRIVER_OBJECT p_driver, PUNICODE_STRING pu_regpath) {
	p_driver->DriverUnload = unload;
	UNICODE_STRING func_name = RTL_CONSTANT_STRING(L"KeUpdateSystemTime");
	PVOID func_address = MmGetSystemRoutineAddress(&func_name);

	

	//e9 短跳 +相对地址
	//ff 25 大跳 +绝对地址

	return STATUS_SUCCESS;
}