#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <nsyshid/hid.h>
#include <coreinit/internal.h>
#include <coreinit/screen.h>
#include <coreinit/memory.h>
#include <coreinit/debug.h>
#include <coreinit/thread.h>
#include <sndcore2/core.h>
#include <whb/proc.h>
#include <whb/log.h>
#include <vpad/input.h>
#include "common/retain_vars.h"
#include "utils/hid.h"

#define PRINT_TEXT1(x, y, str) { OSScreenPutFontEx(SCREEN_DRC, x, y, str); OSScreenPutFontEx(SCREEN_TV, x, y, str); }
#define PRINT_TEXT2(x, y, _fmt, ...) { __os_snprintf(msg, 80, _fmt, __VA_ARGS__); OSScreenPutFontEx(SCREEN_TV, x, y, msg); OSScreenPutFontEx(SCREEN_DRC, x, y, msg); }
#define SWAP16(x) ((x>>8) | ((x&0xFF)<<8))
#define SWAP8(x) ((x>>4) | ((x&0xF)<<4))

int main(int argc, char **argv)
{
    WHBProcInit();
    AXInit();

    WHBLogPrintf("Starting HID-TEST by VannyBuns. Building time: %s %s\n\n", __DATE__, __TIME__);

    hid_init();
    WHBLogPrintf("HID Initialized.\n");
	
		// Init screen and screen buffers
		OSScreenInit();
		int screen_buf0_size = OSScreenGetBufferSizeEx(SCREEN_TV);
		void* screen_buffer_tv = memalign(0x100, screen_buf0_size);
		void* screen_buffer_drc = memalign(0x100, screen_buf0_size);

		OSScreenSetBufferEx(SCREEN_TV, screen_buffer_tv);
		OSScreenSetBufferEx(SCREEN_DRC, screen_buffer_drc);
		
		OSScreenEnableEx(SCREEN_TV, 1);
		OSScreenEnableEx(SCREEN_DRC, 1);
	
		char* msg = (char*) malloc(80);
		
		WHBLogPrintf("Entering main loop...\n");

    while (WHBProcIsRunning())
    {
		
		if (!screen_buffer_tv || !screen_buffer_drc) 
		{
			WHBLogPrintf("Failed to allocate screen buffers.\n");
			return -1;
		}

		OSScreenClearBufferEx(SCREEN_TV, 0);
		OSScreenClearBufferEx(SCREEN_DRC, 0);
		
		// Read vpad
        VPADStatus vpad_status;
        int vpad_read_result = VPADRead(VPAD_CHAN_0, &vpad_status, 1, NULL);
        if (vpad_read_result != 0) 
		{
            WHBLogPrintf("Error reading VPAD: %d\n", vpad_read_result);
        }

        int i = 0;
        PRINT_TEXT2(0, i, "HID-TEST - by VannyBuns - %s %s", __DATE__, __TIME__); i++; i++;
        if (hid_callback_data != NULL) 
		{
            unsigned char *buffer = hid_callback_data->buffer;
            if (buffer != NULL) 
			{
                HIDDevice *device = hid_callback_data->device;

                PRINT_TEXT2(0, i, "vid              %04x\n", SWAP16(device->vid)); i++;
                PRINT_TEXT2(0, i, "pid              %04x\n", SWAP16(device->pid)); i++;
                PRINT_TEXT2(0, i, "interface index  %02x\n", device->interfaceIndex); i++;
                PRINT_TEXT2(0, i, "sub class        %02x\n", device->subClass); i++;
                PRINT_TEXT2(0, i, "protocol         %02x\n", device->protocol); i++;
                PRINT_TEXT2(0, i, "max packet in    %02x\n", device->maxPacketSizeRx); i++;
                PRINT_TEXT2(0, i, "max packet out   %02x\n", device->maxPacketSizeTx); i++;

                PRINT_TEXT2(0, i, "bytes transferred: %d", hid_callback_data->transfersize); i++;
                i++;
                PRINT_TEXT1(0, i, "Pos: 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15"); i++;
                PRINT_TEXT1(0, i, "----------------------------------------------------"); i++;
                PRINT_TEXT2(0, i, "     %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14], buffer[15]); i++;

            }
        }
        PRINT_TEXT1(0, 17, "Press HOME to return to the Wii U Menu");
			OSScreenFlipBuffersEx(SCREEN_TV);
			OSScreenFlipBuffersEx(SCREEN_DRC);
    }
		WHBLogPrintf("Exiting main loop...\n");

		free(msg);
		free(screen_buffer_tv);
		free(screen_buffer_drc);
							
		HIDTeardown();
		hid_deinit();
		WHBLogPrintf("HID Deinitialized.\n");
									
		AXQuit();
		WHBProcShutdown();
		return 0;
}
