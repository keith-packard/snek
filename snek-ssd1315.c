/*
 * Copyright © 2021 Keith Packard <keithp@keithp.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "snek.h"
#include "snek-i2c.h"

/* SSD1315 128x64 OLED display */

#define SSD_ADDR	0x78

#define SSD_COLUMN_LO(x)	(0x00 | (x))
#define SSD_COLUMN_HI(x)	(0x10 | (x))
#define SSD_SET_ADDR_MODE	0x20		/* + 1 byte */
#define SSD_SET_COLUMN_ADDR	0x21		/* + 2 bytes */
#define SSD_SET_PAGE_ADDR	0x22		/* + 2 bytes */
#define SSD_SET_START_LINE(x)	(0x40 | (x))
#define SSD_SET_CONTRAST	0x81		/* + 1 byte */
#define SSD_SET_SEG_REMAP(x)	(0xa0 | (x))
#define SSD_ENTIRE_ON(x)	(0xa4 | (x))
#define SSD_SET_INVERSE(x)	(0xa6 | (x))
#define SSD_SET_MUX		0xa8		/* + 1 byte */
#define SSD_DISPLAY_ON(x)	(0xae | (x))
#define SSD_SET_PAGE_START(x)	(0xb0 | (x))
#define SSD_COM_OUTPUT_SCAN(x)	(0xc0 | ((x) << 3))
#define SSD_SET_VERT_SHIFT	0xd3		/* + 1 byte */
#define SSD_SET_CLOCK_DIV	0xd5		/* + 1 byte */
#define SSD_SET_PRECHARGE	0xd9		/* + 1 byte */
#define SSD_SET_PIN_CONFIG	0xda		/* + 1 byte */
#define SSD_SET_COM_VOLTAGE	0xdb		/* + 1 byte */
#define SSD_NOP			0xe3
#define SSD_CHARGE_PUMP		0x8d		/* + 1 byte */
#define SSD_SCROLL_H(x)		(0x26 | (x))	/* + 6 bytes */
#define SSD_SCROLL_V(x)		(0x29 | (x))	/* + 7 bytes */
#define SSD_SCROLL_STOP		0x2e
#define SSD_SCROLL_START	0x2f
#define SSD_SCROLL_AREA_V	0xa3		/* + 2 bytes */
#define SSD_SCROLL_SETUP(x)	(0x2c | (x))	/* + 6 bytes */
#define SSD_SET_FADE		0x23		/* + 1 byte */
#define SSD_SET_ZOOM		0xd6		/* + 1 byte */

static CONST uint8_t setup[] = {
	0x00,
	SSD_DISPLAY_ON(0),	/* display off */
	SSD_SET_START_LINE(0),
	SSD_SET_CONTRAST, 0x7f,
	SSD_SET_INVERSE(0),
	SSD_SET_MUX, 0x3f,
	SSD_SET_SEG_REMAP(0),
	SSD_COM_OUTPUT_SCAN(0),
	SSD_SET_VERT_SHIFT, 0x00,
	SSD_SET_CLOCK_DIV, 0x80,
	SSD_SET_PRECHARGE, 0x22,
	SSD_SET_PIN_CONFIG, 0x12,
	SSD_SET_COM_VOLTAGE, 0x20,
	SSD_CHARGE_PUMP, 0x15,
	SSD_SCROLL_STOP,
	SSD_DISPLAY_ON(1),	/* display on */
};

#define SETUP_LEN	(sizeof setup)

#define SCALE	2

#define SCREEN_WIDTH	128
#define SCREEN_HEIGHT	64
#define PAGE_HEIGHT	8
#define FB_WIDTH	(SCREEN_WIDTH / SCALE)
#define FB_HEIGHT	(SCREEN_HEIGHT / SCALE)
#define FB_STRIDE	((FB_WIDTH + 7) / 8)
#define BITS		((1 << SCALE) - 1)

uint8_t	fb[FB_STRIDE * FB_HEIGHT];

static uint8_t *
bit_addr(uint8_t x, uint8_t y)
{
	return fb + y * FB_STRIDE + (x >> 3);
}

static void
ssd_start(uint8_t mode)
{
	snek_i2c_start(SSD_ADDR | SNEK_I2C_WRITE);
	snek_i2c_write(mode);
}

static void
ssd_start_write(uint8_t col, uint8_t page)
{
	ssd_start(0x00);
	snek_i2c_write(SSD_COLUMN_HI(col >> 4));
	snek_i2c_write(SSD_COLUMN_LO(col & 0xf));
	snek_i2c_write(SSD_SET_PAGE_START(page));
	snek_i2c_stop();

	ssd_start(0x40);

}

static void
paint_block(uint8_t x, uint8_t y)
{
	uint8_t page = y / (PAGE_HEIGHT / SCALE);
	uint8_t col = x * SCALE;
	uint8_t c;

	ssd_start_write(col, page);
	y &= ~((PAGE_HEIGHT / SCALE) - 1);
	uint8_t *p = bit_addr(x, y);
	uint8_t bit = 1 << (x & 7);
	uint8_t bits = BITS;
	uint8_t b = 0;
	for (c = 0; c < PAGE_HEIGHT / SCALE; c++) {
		if (*p & bit)
			b |= bits;
		p += FB_STRIDE;
		bits <<= SCALE;
	}
	for (c = 0; c < SCALE; c++)
		snek_i2c_write(b);
	snek_i2c_stop();
}

static uint8_t color = 1;

static void
set_bit(uint8_t x, uint8_t y)
{
	uint8_t *b = bit_addr(x, y);
	uint8_t ix = x & 7;
	uint8_t mask = (1 << ix);
	*b = (*b & ~mask) | color << ix;
	paint_block(x, y);
}

static bool been_here;

snek_poly_t
snek_builtin_draw_erase(void)
{
	uint8_t page;
	uint16_t i;

	memset(fb, '\0', sizeof(fb));
	for (page = 0; page < 8; page++) {
		ssd_start_write(0, page);
		for (i = 0; i < (SCREEN_WIDTH * SCREEN_HEIGHT)>>3; i++)
			snek_i2c_write(0);
		snek_i2c_stop();
	}
	return SNEK_NULL;
}

static void
snek_oled_init(void)
{
	if (!been_here) {
		been_here = true;
		uint8_t i;
		ssd_start(0x00);
		for (i = 0; i < SETUP_LEN; i++)
			snek_i2c_write(setup[i]);
		snek_i2c_stop();
		snek_builtin_draw_erase();
	}
}

static int8_t
snek_get_coord(snek_poly_t c, uint8_t max)
{
	snek_soffset_t v = snek_poly_get_soffset(c);
	if (v < 0)
		v = 0;
	if (v > max)
		v = max;
	return v;
}

static int8_t
snek_get_x(snek_poly_t c)
{
	return snek_get_coord(c, FB_WIDTH - 1);
}

static int8_t
snek_get_y(snek_poly_t c)
{
	return snek_get_coord(c, FB_HEIGHT - 1);
}

static int8_t last_x, last_y;

snek_poly_t
snek_builtin_draw_color(snek_poly_t v)
{
	color = !!snek_poly_get_soffset(v);
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_draw_line(snek_poly_t x, snek_poly_t y)
{
	int8_t ix = snek_get_x(x);
	int8_t iy = snek_get_y(y);

	snek_oled_init();

	int8_t dx = ix - last_x;
	int8_t step_x = 1;
	if (dx < 0) {
		dx = -dx;
		step_x = -1;
	}
	int8_t dy = iy - last_y;
	int8_t step_y = -1;
	if (dy > 0) {
		dy = -dy;
		step_y = 1;
	}
	int8_t len = dx;
	if (-dy > dx)
		len = -dy;

	int8_t err = dx + dy;

	for (;;) {
		set_bit(last_x, last_y);
		int8_t e2 = err * 2;
		if (len-- == 0)
			break;
		if (e2 >= dy) {
			err += dy;
			last_x += step_x;
		}
		if (e2 <= dx) {
			err += dx;
			last_y += step_y;
		}
	}
	return SNEK_NULL;
}

snek_poly_t
snek_builtin_draw_move(snek_poly_t x, snek_poly_t y)
{
	last_x = snek_get_x(x);
	last_y = snek_get_y(y);
	return SNEK_NULL;
}
