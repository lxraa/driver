// R3ReadDevice.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <string>
#include<Windows.h>
#include <winioctl.h>
using namespace std;
#define IOCTL_TEST1 CTL_CODE(FILE_DEVICE_UNKNOWN,0x801,METHOD_BUFFERED,FILE_ANY_ACCESS)

int main()
{

	INT32 pid;
	cout << "input pid \n" << endl;
	cin >> pid;
	wstring ws_device_name(TEXT("\\\\.\\MyRead"));
	HANDLE h_device = CreateFile(ws_device_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h_device == INVALID_HANDLE_VALUE) {
		wcout << TEXT("open device failed") << endl;
		return 1;
	}
	DWORD dw_output;
	UCHAR uc_outbuffer[10];
	UCHAR uc_inbuffer[10];
	memset(uc_outbuffer, 0x00, 10);
	memset(uc_inbuffer, 0xbb, 10);
	

	/*UCHAR buffer[10];
	ULONG ul_read_len;*/

	////BOOL ret = ReadFile(h_device, buffer, 10, &ul_read_len, NULL);
	
	//此处的inbuffer和outbuffer是站在内核设备角度的in和out，而不是相对于程序的in和out
	//程序输出到设备的信息写在in buffer中，输入程序的信息写在out buffer中
	BOOL ret = DeviceIoControl(h_device, IOCTL_TEST1, &pid, sizeof(int), uc_outbuffer, 10, &dw_output, NULL);
	if (ret) {
		wcout << TEXT("read byte ") << dw_output << endl;


		for (int i = 0; i < int(dw_output); i++) {
			printf("%02x",uc_outbuffer[i]);
		}
	}
	CloseHandle(h_device);
	return 0;
    //std::cout << "Hello World!\n"; 
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
