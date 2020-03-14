#include <ntddk.h>
#include <intrin.h>

#pragma pack(push)
#pragma pack(1)

typedef struct _IDTR
{
	USHORT limit;
	ULONG64 Base;
}IDTR, *PIDTR;

typedef union _IDT_ENTRY
{
	struct kidt
	{
		USHORT OffsetLow;
		USHORT Selector;
		USHORT IstIndex : 3;
		USHORT Reserved0 : 5;
		USHORT Type : 5;
		USHORT Dpl : 2;
		USHORT Present : 1;
		USHORT OffsetMiddle;
		ULONG OffsetHigh;
		ULONG Reserved1;
	}idt;
	UINT64 Alignment;
}IDT_ENTRY, *PIDT_ENTRY;



IDTR idtr;
//typedef struct _IDTR{
//	USHORT limit;
//	ULONG base;
//} IDTR,*PIDTR;
//typedef struct _IDT_ENTRY {
//	USHORT offset_low;
//	USHORT selector;
//	UCHAR reserved;
//	UCHAR type : 4;
//	UCHAR always0 : 1;
//	UCHAR dpl : 2;
//	UCHAR present : 1;
//	USHORT offset_high;
//};

KIRQL WPOFFx64(){
	KIRQL irql = KeRaiseIrqlToDpcLevel();
	UINT64 cr0 = __readcr0();
	cr0 &= 0xfffffffffffeffff;
	__writecr0(cr0);
	_disable();
	return irql;
}

KIRQL WPONx64(KIRQL irql) {
	UINT64 cr0 = __readcr0();
	cr0 |= 0x10000;
	_enable();
	__writecr0(cr0);
	KeLowerIrql(irql);
}
ULONG setIdtAddr(ULONG idt_base_address, UCHAR index, ULONG64 new_addr) {
	PIDT_ENTRY p_idt = (PIDT_ENTRY)(idt_base_address);
	p_idt = p_idt + index;
	ULONG64 offset_high, offset_middle, offset_low, ret;
	offset_high = (ULONG64)p_idt->idt.OffsetHigh << 32;
	offset_middle = p_idt->idt.OffsetMiddle << 16;
	offset_low = p_idt->idt.OffsetLow;
	ret = offset_high + offset_middle + offset_low;
	p_idt->idt.OffsetHigh = new_addr >> 32;
	p_idt->idt.OffsetMiddle = new_addr << 32 >> 48;
	p_idt->idt.OffsetLow = new_addr << 48 >> 48;
	return ret;
}

NTSTATUS unload(PDRIVER_OBJECT p_driver) {
	DbgPrint("unload\n");
}


NTSTATUS DriverEntry(PDRIVER_OBJECT p_driver, PUNICODE_STRING pu_regpath) {
	p_driver->DriverUnload = unload;
	for (int i = 0; i < KeNumberProcessors; i++) {
		KeSetSystemAffinityThread((KAFFINITY)(1 << i));
		/*__asm {
			sidt idtr;
		}*/
		__sidt(&idtr);
	
		DbgPrint("CPU[%d] IDT base :%llX\n", i, idtr.Base);
		KIRQL IRQL = WPOFFx64();
		ULONG size = sizeof(DWORD64)*256;
		//PVOID mem = ExAllocatePool(NonPagedPool, size);
		//memcpy(mem, idtr.Base, size);
		//ULONG index = i;
		//ULONG old_address = setIdtAddr(idtr.Base, 0xEA, mem);
		//DbgPrint("CPU[%d] Old IDT[0xEA]:%llX\n", i, old_address);//0x1234567887654321
		WPONx64(IRQL);
		KeRevertToUserAffinityThread();
	}
	return STATUS_SUCCESS;

}
#pragma pack(pop)