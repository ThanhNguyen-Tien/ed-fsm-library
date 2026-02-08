#include "driver.h"
#include "controller.h"

void HydraDrv_ReceiveHeader(uint8_t data)
{
	if (data == HEADER_INDICATOR)
		HydraDrv.hydraRx = &HydraDrv_ReceiveLength;
}

void HydraDrv_ReceiveLength(uint8_t data)
{
	HydraDrv.rxLength = data;
	if (HydraDrv.rxLength < MAX_PACKET_LENGTH)
	{
		HydraDrv.rxIndex = 2;
		HydraDrv.rxType = 0;
		HydraDrv.rxChecksum = HydraDrv.rxLength;
		HydraDrv.hydraRx = &HydraDrv_ReceiveType;
	} else
		HydraDrv.hydraRx = &HydraDrv_ReceiveHeader;
}

void HydraDrv_ReceiveType(uint8_t data)
{
	HydraDrv.rxChecksum += data;
	HydraDrv.rxType <<= 8;
	HydraDrv.rxType += data;
	if (--HydraDrv.rxIndex == 0)
	{
		if (HydraDrv.rxLength > 0)
			HydraDrv.hydraRx = &HydraDrv_ReceiveData;
		else
			HydraDrv.hydraRx = &HydraDrv_ReceiveChecksum;
	}
}

void HydraDrv_ReceiveData(uint8_t data)
{
	HydraDrv.rxChecksum += data;
	HydraDrv.rxBuffer[HydraDrv.rxIndex++] = data;
	if (HydraDrv.rxIndex == HydraDrv.rxLength)
		HydraDrv.hydraRx = &HydraDrv_ReceiveChecksum;
}

void HydraDrv_ReceiveChecksum(uint8_t data)
{
	if (data == HydraDrv.rxChecksum)
		HydraDrv.hydraRx = &HydraDrv_ReceiveFooter;
	else
		HydraDrv.hydraRx = &HydraDrv_ReceiveHeader;
}

void HydraDrv_ReceiveFooter(uint8_t data)
{
	if (data == FOOTER_INDICATOR)
	{
		HydraCtrl_ProcessCommand(HydraDrv.rxType, HydraDrv.rxLength,
				HydraDrv.rxBuffer);
	}
	HydraDrv.hydraRx = &HydraDrv_ReceiveHeader;
}
