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
// 通过5层映射，实现VA到PA的转换
// 1、去掉VA的高16位
// 2、按9 9 9 9 12 分剩下的48位，这五个数都是index（offset/8），进程cr3存的是第一张表的物理base_address
// 3、n层表保存了n+1层的base_address，从n层表到n+1层的计算方法是：base_address + offset，offset为第二步通过VA计算出的offset
//		例如cr3为47003000,计算出的index为1f0，则该地址对应的PML4E（第一张表）表项所在的物理地址为：0x47003000 + 1f0 * 8 = 47003f80
//		表中保存了下一张表的base_address和用于权限控制的flags，table_data = base_address || flags, flags占前12位 获取base_address的方法为  (table_data >> 3) << 3
//		计算第三张表项的PA方法为：base_address + index * 8
//		循环5次后实现VA -> PA的转换



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