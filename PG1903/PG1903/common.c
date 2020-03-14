#include "common.h"
void WriteProtectDisable() {
	_disable();
	DWORD64 cr0 = __readcr0();
	cr0 &= (0xfffffffffffeffff);
	__writecr0(cr0);
	return;
}

void WriteProtectEnable() {
	DWORD64 cr0 = __readcr0();
	cr0 |= 0x10000;
	__writecr0(cr0);
	_enable();
}
