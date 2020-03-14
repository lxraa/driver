#include "vtsystem.h"
#include <intrin.h>
//
//ecx置1，调用cpuid，检查ecx第5位

NTSTATUS setCr4Vmxe(ULONG t);
PVOID vmx_region;
PVOID vmcs_region;

ULONG vmxAdjustControls(ULONG ctl,ULONG msr) {
	LARGE_INTEGER msr_value;
	msr_value.QuadPart = Asm_ReadMsr(msr);
	ctl &= msr_value.HighPart;
	ctl |= msr_value.LowPart;
	return ctl;
}

ULONG getBit(ULONG src,ULONG n) {
	ULONG left = sizeof(src) * 8 - n;
	return ((src << left) >> (sizeof(src) * 8 - 1));

}
BOOLEAN isVmxEnabled() {
	//eax ebx ecx edx
	ULONG info[4];
	__cpuid(info, 1);
	ULONG left = sizeof(info[2]) * 8 - 6;
	return ((info[2] << left) >> (sizeof(info[2]) * 8 - 1));

}

BOOLEAN isVtOn() {
	ULONG m = __readmsr(0x3A);
	DbgPrint("%d", m);
	UINT32 left = sizeof(m) * 8 - 1;
	return ((m << left) >> left);
}


NTSTATUS checkContext() {
	if (!isVmxEnabled()){
		DbgPrint("未开启VMX，请打开bios的VT支持\n");
		return STATUS_UNSUCCESSFUL;
	}
	if (!isVtOn()) {
		DbgPrint("VT被锁定（msr 0x3A）\n");
		return STATUS_UNSUCCESSFUL;
	}
	//cr4的14位和cr0的
	_CR0 cr0;
	*((ULONG *)&cr0) = __readcr0();
	_CR4 cr4;
	*((ULONG *)&cr4) = __readcr4();
	if (cr0.PE != 1 || cr0.PG != 1 || cr0.NE != 1) {
		DbgPrint("请将cr0的PE PG NE位 置1\n");
		return STATUS_UNSUCCESSFUL;
	}

	if (cr4.VMXE == 1) {
		DbgPrint("cr4的VMXE位不应该为1\n");
		return STATUS_UNSUCCESSFUL;
	}
	DbgPrint("检查通过\n");
	return STATUS_SUCCESS;
}

NTSTATUS startVt() {
	setCr4Vmxe(1);
	vmx_region = ExAllocatePool(NonPagedPool, 0x1000);
	RtlZeroMemory(vmx_region, 0x1000);
	*(PULONG)vmx_region = 1;

	PHYSICAL_ADDRESS vmx_region_pa = MmGetPhysicalAddress(vmx_region);
	_EFLAGS eflags;
	Vmx_VmxOn(vmx_region_pa.LowPart, vmx_region_pa.HighPart);
	*((PULONG)&eflags) = Asm_GetEflags();
	// __vmx_on(&vmx_region_pa.QuadPart);
	//成功则cf为1
	if (eflags.CF != 0) {
		DbgPrint("开启VT失败\n");
		return STATUS_UNSUCCESSFUL;
	}
	DbgPrint("开启VT成功\n");

	return STATUS_SUCCESS;
}
NTSTATUS setCr4Vmxe(ULONG t) {
	_CR4 cr4;
	*((PULONG)&cr4) = __readcr4();
	if (t == 0) {
		cr4.VMXE = 0;
	}
	else {
		cr4.VMXE = 1;
	}
	__writecr4(*((PULONG)(&cr4)));
	return STATUS_SUCCESS;
}
NTSTATUS stopVt() {
	Vmx_VmxOff();
	ExFreePool(vmx_region);
	setCr4Vmxe(0);
	return STATUS_SUCCESS;
}

NTSTATUS createVmcs() {
	vmcs_region = ExAllocatePool(NonPagedPool, 0x1000);
	memset(vmcs_region, 0, 0x1000);
	PHYSICAL_ADDRESS vmcs_region_pa = MmGetPhysicalAddress(vmcs_region);
	*((PULONG)vmcs_region) = 1;

	Vmx_VmClear(vmcs_region_pa.LowPart, vmcs_region_pa.HighPart);

	Vmx_VmPtrld(vmcs_region_pa.LowPart, vmcs_region_pa.HighPart);

	return STATUS_SUCCESS;
}


NTSTATUS setupVmcs() {
	//1.guest state fields
	//2.host state fields
	//3.vm-control fields
		//vm excution control
	Vmx_VmWrite(PIN_BASED_VM_EXEC_CONTROL, vmxAdjustControls(0, MSR_IA32_VMX_PINBASED_CTLS));
	Vmx_VmWrite(CPU_BASED_VM_EXEC_CONTROL, vmxAdjustControls(0, MSR_IA32_VMX_PROCBASED_CTLS));
		//vm exit control
	Vmx_VmWrite(VM_ENTRY_CONTROLS, vmxAdjustControls(0, MSR_IA32_VMX_ENTRY_CTLS));
		//vm entry control
	Vmx_VmWrite(VM_EXIT_CONTROLS, vmxAdjustControls(0, MSR_IA32_VMX_EXIT_CTLS));

	Vmx_VmLaunch();
	DbgPrint("error: %d\n", Vmx_VmRead(VM_INSTRUCTION_ERROR));

	return STATUS_SUCCESS;
}
//BOOLEAN IsVTEnabled()
//{
//	ULONG       uRet_EAX, uRet_ECX, uRet_EDX, uRet_EBX;
//	_CPUID_ECX  uCPUID;
//	_CR0        uCr0;
//	_CR4    uCr4;
//	IA32_FEATURE_CONTROL_MSR msr;
//	//1. CPUID
//	Asm_CPUID(1, &uRet_EAX, &uRet_EBX, &uRet_ECX, &uRet_EDX);
//	*((PULONG)&uCPUID) = uRet_ECX;
//
//	if (uCPUID.VMX != 1)
//	{
//		Log("ERROR: 这个CPU不支持VT!", 0);
//		return FALSE;
//	}
//
//	// 2. CR0 CR4
//	*((PULONG)&uCr0) = Asm_GetCr0();
//	*((PULONG)&uCr4) = Asm_GetCr4();
//
//	if (uCr0.PE != 1 || uCr0.PG != 1 || uCr0.NE != 1)
//	{
//		Log("ERROR:这个CPU没有开启VT!", 0);
//		return FALSE;
//	}
//
//	if (uCr4.VMXE == 1)
//	{
//		Log("ERROR:这个CPU已经开启了VT!", 0);
//		Log("可能是别的驱动已经占用了VT，你必须关闭它后才能开启。", 0);
//		return FALSE;
//	}
//
//	// 3. MSR
//	*((PULONG)&msr) = (ULONG)Asm_ReadMsr(MSR_IA32_FEATURE_CONTROL);
//	if (msr.Lock != 1)
//	{
//		Log("ERROR:VT指令未被锁定!", 0);
//		return FALSE;
//	}
//	Log("SUCCESS:这个CPU支持VT!", 0);
//	return TRUE;
//}
