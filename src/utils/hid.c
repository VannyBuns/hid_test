/****************************************************************************
 * Copyright (C) 2016 Maschell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <whb/log.h>
#include "hid.h"
#include "common/retain_vars.h"

void hid_init() 
{
    HIDSetup();
    HIDAddClient(&gHIDClient, my_attach_cb);
}

void hid_deinit() 
{
    HIDDelClient(&gHIDClient);
}

#define SWAP16(x) ((x >> 8) | ((x & 0xFF) << 8))
#define SWAP8(x) ((x >> 4) | ((x & 0xF) << 4))

int32_t my_attach_cb(HIDClient *client, HIDDevice *device, uint32_t attach) 
{
    if (attach) 
	{
        WHBLogPrintf("vid %04x pid %04x connected\n", SWAP16(device->vid), SWAP16(device->pid));
        WHBLogPrintf("interface index  %02x\n", device->interfaceIndex);
        WHBLogPrintf("sub class        %02x\n", device->subClass);
        WHBLogPrintf("protocol         %02x\n", device->protocol);
        WHBLogPrintf("max packet in    %02x\n", device->maxPacketSizeRx);
        WHBLogPrintf("max packet out   %02x\n", device->maxPacketSizeTx);

        int bufferSize = 64;
        unsigned char *buffer = memalign(64, bufferSize);
        memset(buffer, 0, bufferSize);
        my_cb_user *usr = memalign(64, sizeof(my_cb_user));

        usr->buffer = buffer;
        usr->device = device;
        usr->transfersize = device->maxPacketSizeRx;
        usr->handle = device->handle;

        if (SWAP16(device->vid) == 0x057e && SWAP16(device->pid) == 0x0337) 
		{
            buffer[0] = 0x13;
            HIDWrite(device->handle, usr->buffer, 1, NULL, NULL);
        }

        HIDRead(device->handle, usr->buffer, device->maxPacketSizeRx, my_read_cb, usr);
        return HID_DEVICE_ATTACH;
    } 
	else 
	{
        WHBLogPrintf("vid %04x pid %04x disconnected\n", SWAP16(device->vid), SWAP16(device->pid));

        if (hid_callback_data) 
		{
            my_cb_user *user_data = hid_callback_data;
            if (user_data->buffer) 
			{
                free(user_data->buffer);
                user_data->buffer = NULL;
            }
            free(user_data);
            hid_callback_data = NULL;
        }
    }
    return HID_DEVICE_DETACH;
}

void my_read_cb(uint32_t handle, int32_t error, uint8_t *buffer, uint32_t bytesTransferred, void *userContext) 
{
    if (error == 0 && userContext != NULL) 
	{
        my_cb_user *usr = (my_cb_user*)userContext;
        hid_callback_data = usr;
        unsigned char*  buffer = usr->buffer;
        if (buffer != NULL) 
		{
            WHBLogPrintf("data: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n", 
                          buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], 
                          buffer[8], buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14], buffer[15]);
        }
        // Reinitialize the read operation
        HIDRead(handle, usr->buffer, usr->transfersize, my_read_cb, usr);
    } 
	else 
	{
        WHBLogPrintf("HID read error: %d\n", error);
    }
}
