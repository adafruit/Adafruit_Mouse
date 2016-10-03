/*
  Mouse.cpp

  Copyright (c) 2015, Arduino LLC
  Original code (pre-library): Copyright (c) 2011, Peter Barrett

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Adafruit_Mouse.h"

#define HID_REPORTID_MOUSE     (1)
#define HID_REPORTID_MOUSE_ABS (5)

#if defined(_USING_HID)

static const uint8_t _hidReportDescriptor[] PROGMEM = {

    //  Mouse relative
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)	// 54
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x85, HID_REPORTID_MOUSE,      //     REPORT_ID (1)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x09, 0x38,                    //     USAGE (Wheel)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
    0xc0,                          //   END_COLLECTION
    0xc0,                          // END_COLLECTION

    //  Mouse absolute
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x85, HID_REPORTID_MOUSE_ABS,  //     REPORT_ID (5)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x09, 0x38,                    //     USAGE (Wheel)
    0x16, 0x00, 0x80,              //     LOGICAL_MINIMUM (-32768)
    0x26, 0xff, 0x7f,              //     LOGICAL_MAXIMUM (32767)
    0x75, 0x10,                    //     REPORT_SIZE (16)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xc0,                          //   END_COLLECTION
    0xc0,                          // END_COLLECTION
};


//================================================================================
//================================================================================
//	Mouse

Adafruit_Mouse::Adafruit_Mouse(void) : _buttons(0)
{
   static HIDSubDescriptor node(_hidReportDescriptor, sizeof(_hidReportDescriptor));
   HID().AppendDescriptor(&node);
}

void Adafruit_Mouse::begin(void) 
{
}

void Adafruit_Mouse::end(void) 
{
}

void Adafruit_Mouse::click(uint8_t b)
{
	_buttons = b;
	move(0,0,0);
	_buttons = 0;
	move(0,0,0);
}

void Adafruit_Mouse::move(signed char x, signed char y, signed char wheel)
{
	uint8_t m[4];
	m[0] = _buttons;
	m[1] = x;
	m[2] = y;
	m[3] = wheel;
	HID().SendReport(1,m,4);
}

// all parameters have the range of -32768 to 32767 and must be scaled to screen pixels
// some examples:
//   x=0,y=0 is the middle of the screen
//   x=-32768,y=-32768 is the top left corner
//   x=32767,y=-32768 is the top right corner
//   x=32767,y=32767 is the bottom right corner
//   x=-32768,y=32767 is the bottom left corner
void Adafruit_Mouse::moveAbs(int16_t x, int16_t y, int16_t wheel)
{
	u8 m[7];
	m[0] = _buttons; // TODO: is it a good idea to take over the _buttons from relative report here or should it be left out?
	m[1] = x & 0xFF;
	m[2] = x >> 8;
	m[3] = y & 0xFF;
	m[4] = y >> 8;
	m[5] = wheel & 0xFF;
	m[6] = wheel >> 8;
	HID().SendReport(HID_REPORTID_MOUSE_ABS,m,sizeof(m));
}

void Adafruit_Mouse::buttons(uint8_t b)
{
	if (b != _buttons)
	{
		_buttons = b;
		move(0,0,0);
	}
}

void Adafruit_Mouse::press(uint8_t b) 
{
	buttons(_buttons | b);
}

void Adafruit_Mouse::release(uint8_t b)
{
	buttons(_buttons & ~b);
}

bool Adafruit_Mouse::isPressed(uint8_t b)
{
	if ((b & _buttons) > 0) 
		return true;
	return false;
}

#endif
