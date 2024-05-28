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

#ifndef HID_H
#define HID_H

#ifdef __cplusplus
extern "C" {
#endif

#include <nsyshid/hid.h>

typedef struct _my_cb_user {
    unsigned char *buffer;
    unsigned int transfersize;
    unsigned int handle;
    HIDDevice *device;
} my_cb_user;

void hid_init();
void hid_deinit();
void my_read_cb(uint32_t handle, int32_t error, uint8_t *buffer, uint32_t bytesTransferred, void *userContext);
int32_t my_attach_cb(HIDClient *client, HIDDevice *device, uint32_t attach);

#ifdef __cplusplus
}
#endif

#endif /* HID_H */
