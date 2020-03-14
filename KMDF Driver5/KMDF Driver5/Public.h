/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_KMDFDriver5,
    0x240b7c12,0x6048,0x42c3,0xaf,0x86,0x3a,0xbc,0xfb,0xff,0xd3,0xdd);
// {240b7c12-6048-42c3-af86-3abcfbffd3dd}
