#include <ntifs.h>
#include <ntddk.h>

typedef struct _MM_COPY_ADDRESS {
	union {
		PVOID            VirtualAddress;
		PHYSICAL_ADDRESS PhysicalAddress;
	};
} MM_COPY_ADDRESS, *PMMCOPY_ADDRESS;

extern NTSTATUS MmCopyMemory(
	PVOID           TargetAddress,
	MM_COPY_ADDRESS SourceAddress,
	SIZE_T          NumberOfBytes,
	ULONG           Flags,
	PSIZE_T         NumberOfBytesTransferred
);
//------------------Amd64VtoP-------------------
// ͨ��5��ӳ�䣬ʵ��VA��PA��ת��
// 1��ȥ��VA�ĸ�16λ
// 2����9 9 9 9 12 ��ʣ�µ�48λ�������������index��offset/8��������cr3����ǵ�һ�ű������base_address
// 3��n�������n+1���base_address����n���n+1��ļ��㷽���ǣ�base_address + offset��offsetΪ�ڶ���ͨ��VA�������offset
//		����cr3Ϊ47003000,�������indexΪ1f0����õ�ַ��Ӧ��PML4E����һ�ű��������ڵ������ַΪ��0x47003000 + 1f0 * 8 = 47003f80
//		���б�������һ�ű��base_address������Ȩ�޿��Ƶ�flags��table_data = base_address || flags, flagsռǰ12λ ��ȡbase_address�ķ���Ϊ  (table_data >> 3) << 3
//		��������ű����PA����Ϊ��base_address + index * 8
//		ѭ��5�κ�ʵ��VA -> PA��ת��



#define PML4E_INDEX(x) ((x << 16) >> 55)
#define PDPE_INDEX(x) ((x << 25) >> 55)
#define PDE_INDEX(x) ((x << 34) >> 55)
#define PTE_INDEX(x) ((x << 43) >> 55)
#define P_INDEX(x) ((x << 48) >> 48)


NTSTATUS unload(PDRIVER_OBJECT p_driver) {
	
	return STATUS_SUCCESS;
}

DWORD64 getProcessBaseDir() {
	return 0x47003000;
}


NTSTATUS DriverEntry(PDRIVER_OBJECT p_driver, PUNICODE_STRING pu_regpath) {
	p_driver->DriverUnload = unload;
	DWORD64 address = 0xfffff80003e80490;
	DWORD64 base_dir = getProcessBaseDir();
	PDWORD64 buffer = (PDWORD64)ExAllocatePool(NonPagedPool, sizeof(DWORD64));
	 
	MM_COPY_ADDRESS pml4e_address;
	pml4e_address.PhysicalAddress.QuadPart = base_dir + PML4E_INDEX(address);
	SIZE_T size;
	MmCopyMemory(buffer, pml4e_address, sizeof(buffer), 1, &size);

	DbgPrint("buffer:%p", buffer);

	DbgPrint("PML4E: %p PDPE: %p PDE: %p PTE: %p P: %p \n", PML4E_INDEX(address), PDPE_INDEX(address), PDE_INDEX(address), PTE_INDEX(address), P_INDEX(address));

	return STATUS_SUCCESS;
}