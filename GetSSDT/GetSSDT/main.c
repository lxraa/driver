#include <ntddk.h>
#include <ntddstor.h>
#include <mountdev.h>
#include <ntddvol.h>


typedef struct _ServiceDescriptorTable {
	PVOID ServiceTableBase;
	PVOID ServiceCounterTable;
	unsigned int NumberOfServices;
	PVOID ParamTableBase;
}*PServiceDescriptorTable;

extern PServiceDescriptorTable KeServiceDescriptorTable;

LONG GetFunctionAddr(PServiceDescriptorTable p_table, LONG index);




NTSTATUS unload(PDRIVER_OBJECT p_driver) {

	return STATUS_SUCCESS;
}


NTSTATUS DriverEntry(PDRIVER_OBJECT p_driver, PUNICODE_STRING pu_regpath) {

	p_driver->DriverUnload = unload;
	DbgPrint("%d", sizeof(LONG));
	LONG l_ssdt_number = KeServiceDescriptorTable->NumberOfServices;
	KdPrint(("ʹ�÷���2.����SSDT\r\n"));
	for (int i = 0; i < l_ssdt_number; i++)
	{
		DbgPrint("Index:%04X--FunAddr:%08X\r\n", i, GetFunctionAddr(KeServiceDescriptorTable, i));
	}
	
	return STATUS_SUCCESS;
}



LONG GetFunctionAddr(PServiceDescriptorTable KeServiceDescriptorTable, LONG index)
{

	LONG lgSsdtAddr = 0;
	//��ȡSSDT��Ļ�ַ
	lgSsdtAddr = (LONG)KeServiceDescriptorTable->ServiceTableBase;

	PLONG plgSsdtFunAddr = 0;
	//��ȡ�ں˺����ĵ�ַָ��
	plgSsdtFunAddr = (PLONG)(lgSsdtAddr + index * LONG_SIZE);

	//�����ں˺����ĵ�ַ
	return (*plgSsdtFunAddr);
}