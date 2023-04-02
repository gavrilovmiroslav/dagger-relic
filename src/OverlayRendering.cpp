#include "OverlayRendering.h"
#include "Prelude.h"
extern uint8_t r_image_font_6_11[2464];

/*
 * Context for the software renderer.
 * TODO: Rename this.
 */
struct bitmap
{
    uint32_t  w, h;
    uint32_t *pixel;
};

static void r_puts(struct bitmap *r, uint32_t sx, uint32_t sy, const char *text);
static void r_line(struct bitmap *r, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t colour);

/*
 * Draw text to bitmap.
 * The order of drawing a character is right to left.
 * NOTE: Cannot start drawing with negative x and y values.
 */
void
r_puts(struct bitmap *r, uint32_t sx, uint32_t sy, const char *text)
{
	uint32_t       j      = 0;
	const uint32_t colour = 0xffffffff;

	if (sy >= r->h || text == NULL)
	{
		return;
	}

	while (text[j])
	{
		uint32_t   i;
		const char ch = text[j];

		if (ch >= 32 && ch <= 126)
		{
			for (i = 0; i < 11 && sy + i < r->h; i++)
			{
				unsigned char test = '?';
				unsigned char mask = 1;
				uint32_t      x    = sx + 7;

				test = r_image_font_6_11[(uint32_t)(ch - 32) * 11 + i];

				while (mask != 0)
				{
					const uint32_t map = (sy + i) * r->w + (x);

					/* Must check for each iteration as x is unsigned and may underflow. */
					if (x < r->w)
					{
						r->pixel[map] = (test & mask ? colour : r->pixel[map]);
					}

					x--;
					mask <<= 1;
				}
			}
		}
		sx += 6;
		j++;
	}
}

/*
 * Bresenham's line algorithm.
 * Assuming positions are within the extent of the bitmap (screen).
 */
void
r_line(struct bitmap *r, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t colour)
{
	int32_t dx,  dy;
	int32_t sx,  sy;
	int32_t err, e2;

    /*
     * Clamp values to the extent of the bitmap.
     * Probably extraneous as positions should be valid in the first place,
     * else something is horribly wrong (with the caller).
     * Clamping should be done using linear interpolation (lerp)... but this should be removed anyway...
     * a*(1.0f-x)+(b*x), a, b : range, x : value.
     */
	if (x0 > r->w) { x0 = r->w; }
	if (x1 > r->w) { x1 = r->w; }
	if (y0 > r->h) { y0 = r->h; }
	if (y1 > r->h) { y1 = r->h; }

	dx  = abs((int32_t) x1 - (int32_t) x0);
	sx  = (x0 > x1 ? 1 : -1);
	dy  = abs((int32_t) y1 - (int32_t) y0);
	sy  = (y0 > y1 ? 1 : -1);
	err = (dx > dy ? dx : -dy) / 2;

	for (;;)
	{
		if (x0 < r->w && y0 < r->h)
		{
			r->pixel[x0 + y0 * r->w] = colour;
		}
		if (x0 == x1 && y0 == y1)
		{
			break;
		}
		
		e2 = err;
		if (e2 > -dx)
		{
			err -= dy;
			x0 = (uint32_t) ((int32_t) x0 - sx);
		}
		if (e2 < dy)
		{
			err += dx;
			y0 = (uint32_t) ((int32_t) y0 - sy);
		}
	}
}

void
OverlayLineRenderingModule::process_signal(core::Render2Signal& signal)
{
	for (const auto&& [ entity, line ] : AccessStorage<OverlayLine>::access_storage().each())
	{
        struct bitmap r;

        r.pixel = signal.pixels;
        r.w     = (uint32_t) signal.w;
        r.h     = (uint32_t) signal.h;
        r_line(&r, (uint32_t) line.start.x, (uint32_t) line.start.y, (uint32_t) line.end.x, (uint32_t) line.end.y, line.colour);
	}
}

void
OverlayTextRenderingModule::process_signal(core::Render2Signal& signal)
{
	for (const auto&& [ entity, textdata ] : AccessStorage<OverlayText>::access_storage().each())
	{
        struct bitmap r;

        r.pixel = signal.pixels;
        r.w     = (uint32_t) signal.w;
        r.h     = (uint32_t) signal.h;
        r_puts(&r, (uint32_t) textdata.position.x, (uint32_t) textdata.position.y, textdata.text.c_str());
	}
}

void
OverlayTestRenderingModule::process_signal(core::Render2Signal& signal)
{
	for (const auto&& [ entity, test ] : AccessStorage<OverlayTest>::access_storage().each())
	{
        switch (test.magic)
        {
        case 0:
        for (int y = 0; y < signal.h; y++)
        {
            for (int x = 0; x < signal.w; x++)
            {
                uint32_t c = x^y;

                signal.pixels[y * signal.w + x] = (c << 16) | (c << 8) | (x);
            }
        }
        break;
        case 1:
        for (int y = 0; y < signal.h; y++)
        {
            for (int x = 0; x < signal.w; x++)
            {
                signal.pixels[y * signal.w + x] = (x << 16) | (x << 8) | (x);
            }
        }
        default:
        break;
        }
	}
}

/*
 * Character width: 6, height: 11, total characters: 224, first character: ascii 32.
 * Each line defines 2 characters, every character is made from 11 'unsigned char' bitmasks.
 * Converted from BDF with bdf2c.
 */
uint8_t r_image_font_6_11[2464] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x20, 0x00, 0x00, 0x00, 0x50, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x50, 0xF8, 0x50, 0xF8, 0x50, 0x00, 0x00, 0x00, 0x00, 0x20, 0x70, 0xA8, 0xA0, 0x70, 0x28, 0xA8, 0x70, 0x20, 0x00, 0x00, 0x00, 0x40, 0xA8, 0x50, 0x20, 0x50, 0xA8, 0x10,
	0x00, 0x00, 0x00, 0x60, 0x90, 0x90, 0x60, 0xA8, 0x90, 0x90, 0x68, 0x00, 0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x10, 0x00, 0x00, 0x00, 0x20, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00, 0x50, 0x20, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0xF8, 0x20,
	0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x08, 0x08, 0x10, 0x10, 0x20, 0x20, 0x40, 0x40, 0x00, 0x00, 0x00, 0x70, 0x88, 0x88, 0x98, 0xA8, 0xC8, 0x88, 0x70, 0x00, 0x00, 0x00, 0x20, 0x60, 0xA0, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x70, 0x88, 0x08, 0x70, 0x80, 0x80, 0x80, 0xF8, 0x00, 0x00, 0x00, 0x70, 0x88, 0x08, 0x30, 0x08, 0x08, 0x88, 0x70, 0x00, 0x00, 0x00, 0x10, 0x30, 0x50,
	0x90, 0x90, 0xF8, 0x10, 0x10, 0x00, 0x00, 0x00, 0xF8, 0x80, 0x80, 0xF0, 0x08, 0x08, 0x88, 0x70, 0x00, 0x00, 0x00, 0x70, 0x88, 0x80, 0xF0, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x00, 0xF8, 0x08,
	0x08, 0x10, 0x20, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x70, 0x88, 0x88, 0x70, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x00, 0x70, 0x88, 0x88, 0x88, 0x78, 0x08, 0x88, 0x70, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x40, 0x00, 0x00, 0x00, 0x10, 0x20, 0x40, 0x80, 0x40, 0x20, 0x10, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x20, 0x10, 0x08, 0x10, 0x20, 0x40, 0x00, 0x00, 0x00, 0x70, 0x88, 0x08, 0x08, 0x10, 0x20, 0x00, 0x20, 0x00, 0x00,
	0x00, 0x70, 0x88, 0x88, 0x98, 0xA8, 0xB8, 0x80, 0x78, 0x00, 0x00, 0x00, 0x70, 0x88, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0xF0, 0x88, 0x88, 0xF0, 0x88, 0x88, 0x88, 0xF0, 0x00,
	0x00, 0x00, 0x70, 0x88, 0x80, 0x80, 0x80, 0x80, 0x88, 0x70, 0x00, 0x00, 0x00, 0xF0, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0xF0, 0x00, 0x00, 0x00, 0xF8, 0x80, 0x80, 0xF0, 0x80, 0x80, 0x80, 0xF8,
	0x00, 0x00, 0x00, 0xF8, 0x80, 0x80, 0xF0, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x70, 0x88, 0x80, 0x80, 0x98, 0x88, 0x88, 0x78, 0x00, 0x00, 0x00, 0x88, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x88,
	0x88, 0x00, 0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x88, 0x70, 0x00, 0x00, 0x00, 0x88, 0x90, 0xA0, 0xC0, 0xA0, 0x90,
	0x88, 0x88, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xF8, 0x00, 0x00, 0x00, 0x88, 0xD8, 0xA8, 0x88, 0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x88, 0xC8, 0xA8, 0x98, 0x88,
	0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x00, 0xF0, 0x88, 0x88, 0x88, 0xF0, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x70, 0x88, 0x88, 0x88,
	0x88, 0xA8, 0x90, 0x68, 0x00, 0x00, 0x00, 0xF0, 0x88, 0x88, 0x88, 0xF0, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x78, 0x80, 0x80, 0x70, 0x08, 0x08, 0x08, 0xF0, 0x00, 0x00, 0x00, 0xF8, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x00, 0x88, 0x88, 0x88, 0x88, 0x50, 0x50, 0x20, 0x20, 0x00, 0x00, 0x00, 0x88, 0x88,
	0x88, 0x88, 0xA8, 0xA8, 0xA8, 0x50, 0x00, 0x00, 0x00, 0x88, 0x88, 0x50, 0x20, 0x50, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x88, 0x88, 0x88, 0x50, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xF8,
	0x08, 0x10, 0x20, 0x40, 0x80, 0x80, 0xF8, 0x00, 0x00, 0x00, 0x30, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x30, 0x00, 0x00, 0x00, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x00, 0x00, 0x00,
	0x30, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x30, 0x00, 0x00, 0x00, 0x20, 0x50, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00,
	0x00, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x08, 0x78, 0x88, 0x78, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0xF0, 0x88, 0x88, 0x88, 0xF0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x80, 0x80, 0x80, 0x78, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x78, 0x88, 0x88, 0x88, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x88, 0xF8, 0x80, 0x78,
	0x00, 0x00, 0x00, 0x30, 0x40, 0x40, 0x70, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x88, 0x88, 0x88, 0x78, 0x08, 0xF0, 0x00, 0x80, 0x80, 0x80, 0xF0, 0x88, 0x88, 0x88,
	0x88, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x60, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x30, 0x10, 0x10, 0x10, 0x10, 0x10, 0x60, 0x00, 0x40, 0x40, 0x40, 0x48, 0x50, 0x60,
	0x50, 0x48, 0x00, 0x00, 0x00, 0x60, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xA8, 0xA8, 0xA8, 0xA8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x88,
	0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x88, 0x88, 0x88, 0xF0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x78,
	0x88, 0x88, 0x88, 0x78, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0xB8, 0xC0, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x80, 0x70, 0x08, 0xF0, 0x00, 0x00, 0x00, 0x40, 0x40, 0x40,
	0x70, 0x40, 0x40, 0x40, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x88, 0x88, 0x88, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x88, 0x88, 0x50, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x88, 0x88, 0xA8, 0xA8, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x50, 0x20, 0x50, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x88, 0x88, 0x88, 0x78, 0x08, 0xF0, 0x00, 0x00,
	0x00, 0x00, 0xF8, 0x10, 0x20, 0x40, 0xF8, 0x00, 0x00, 0x00, 0x10, 0x20, 0x20, 0x20, 0x40, 0x20, 0x20, 0x20, 0x10, 0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x00, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00,
	0x40, 0x20, 0x20, 0x20, 0x10, 0x20, 0x20, 0x20, 0x40, 0x00, 0x00, 0x28, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x30, 0x48, 0x40, 0xF0, 0x40, 0xF0, 0x40, 0x48, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x08, 0x78, 0x98, 0xB0, 0xA0, 0xA8, 0x70, 0x40,
	0x00, 0x00, 0x38, 0x44, 0x40, 0x20, 0x78, 0x20, 0x40, 0x40, 0x7C, 0x00, 0x00, 0x00, 0x44, 0x38, 0x44, 0x44, 0x44, 0x38, 0x44, 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x28, 0x7C, 0x10, 0x7C, 0x10,
	0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x00, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x38, 0x44, 0x20, 0x50, 0x48, 0x24, 0x14, 0x08, 0x44, 0x38, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x84, 0xB4, 0xA4, 0xB4, 0x84, 0x78, 0x00, 0x00, 0x00, 0x20, 0x10, 0x28, 0x38, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x28, 0x50,
	0x28, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x84, 0xB4, 0xA4,
	0x94, 0x84, 0x78, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x28, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
	0x10, 0x7C, 0x10, 0x10, 0x7C, 0x00, 0x00, 0x18, 0x04, 0x08, 0x10, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x04, 0x08, 0x04, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x10, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x48, 0x48, 0x48, 0x48, 0x74, 0x40, 0x00, 0x1C, 0x34, 0x74, 0x74, 0x34, 0x14, 0x14, 0x14, 0x14, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x18, 0x70, 0x00, 0x00, 0x08, 0x18, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20,
	0x50, 0x20, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x28, 0x14, 0x28, 0x50, 0x00, 0x00, 0x00, 0x40, 0xC4, 0x48, 0x50, 0x20, 0x54, 0x94, 0x1C, 0x04, 0x00, 0x00,
	0x40, 0xC4, 0x48, 0x50, 0x28, 0x54, 0x88, 0x10, 0x1C, 0x00, 0xC0, 0x20, 0xC4, 0x28, 0xD0, 0x20, 0x54, 0x94, 0x1C, 0x04, 0x00, 0x00, 0x20, 0x00, 0x20, 0x20, 0x40, 0x80, 0x88, 0x88, 0x70, 0x00,
	0x40, 0x20, 0x70, 0x88, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x00, 0x00, 0x10, 0x20, 0x70, 0x88, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x00, 0x00, 0x20, 0x50, 0x70, 0x88, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x00,
	0x00, 0x68, 0x90, 0x70, 0x88, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x00, 0x00, 0x50, 0x00, 0x70, 0x88, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x00, 0x00, 0x70, 0x50, 0x70, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x88,
	0x00, 0x00, 0x00, 0x00, 0x7C, 0x90, 0x90, 0x90, 0xFC, 0x90, 0x9C, 0x00, 0x00, 0x00, 0x70, 0x88, 0x80, 0x80, 0x80, 0x80, 0x88, 0xF0, 0x20, 0x40, 0x40, 0x20, 0xF8, 0x80, 0x80, 0xF0, 0x80, 0x80,
	0xF8, 0x00, 0x00, 0x10, 0x20, 0xF8, 0x80, 0x80, 0xF0, 0x80, 0x80, 0xF8, 0x00, 0x00, 0x20, 0x50, 0xF8, 0x80, 0x80, 0xF0, 0x80, 0x80, 0xF8, 0x00, 0x00, 0x50, 0x00, 0xF8, 0x80, 0x80, 0xF0, 0x80,
	0x80, 0xF8, 0x00, 0x00, 0x40, 0x20, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x10, 0x20, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x20, 0x50, 0x00, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x00, 0x00, 0x50, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x78, 0x44, 0x44, 0xE4, 0x44, 0x44, 0x44, 0x78, 0x00, 0x00, 0x68, 0x90, 0x88, 0xC8, 0xA8,
	0x98, 0x88, 0x88, 0x88, 0x00, 0x00, 0x40, 0x20, 0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x10, 0x20, 0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x20, 0x50, 0x70, 0x88,
	0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x68, 0x90, 0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x50, 0x00, 0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x50, 0x20, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0x88, 0x98, 0x98, 0xA8, 0xC8, 0xC8, 0xF0, 0x00, 0x00, 0x40, 0x20, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x10, 0x20,
	0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x20, 0x50, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x50, 0x00, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x10,
	0x20, 0x88, 0x88, 0x88, 0x88, 0x70, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x80, 0xE0, 0x90, 0x88, 0x90, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x70, 0x88, 0x88, 0xB0, 0x88, 0x88, 0x88, 0xF0, 0x80, 0x80,
	0x00, 0x40, 0x20, 0x00, 0x70, 0x08, 0x78, 0x88, 0x78, 0x00, 0x00, 0x00, 0x10, 0x20, 0x00, 0x70, 0x08, 0x78, 0x88, 0x78, 0x00, 0x00, 0x00, 0x20, 0x50, 0x00, 0x70, 0x08, 0x78, 0x88, 0x78, 0x00,
	0x00, 0x00, 0x68, 0x90, 0x00, 0x70, 0x08, 0x78, 0x88, 0x78, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x70, 0x08, 0x78, 0x88, 0x78, 0x00, 0x00, 0x20, 0x50, 0x20, 0x00, 0x70, 0x08, 0x78, 0x88, 0x78,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x28, 0x78, 0xA0, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x80, 0x80, 0x80, 0x78, 0x10, 0x20, 0x00, 0x40, 0x20, 0x00, 0x70, 0x88, 0xF8, 0x80,
	0x78, 0x00, 0x00, 0x00, 0x10, 0x20, 0x00, 0x70, 0x88, 0xF8, 0x80, 0x78, 0x00, 0x00, 0x00, 0x20, 0x50, 0x00, 0x70, 0x88, 0xF8, 0x80, 0x78, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x70, 0x88, 0xF8,
	0x80, 0x78, 0x00, 0x00, 0x00, 0x40, 0x20, 0x00, 0x60, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x10, 0x20, 0x00, 0x60, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x20, 0x50, 0x00, 0x60, 0x20,
	0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x60, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x50, 0x20, 0x50, 0x08, 0x78, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x00, 0x68, 0x90, 0x00, 0xF0,
	0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x40, 0x20, 0x00, 0x70, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x00, 0x10, 0x20, 0x00, 0x70, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x00, 0x20, 0x50, 0x00,
	0x70, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x00, 0x68, 0x90, 0x00, 0x70, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x70, 0x88, 0x88, 0x88, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x20, 0x00, 0xF8, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x98, 0xA8, 0xC8, 0xF0, 0x00, 0x00, 0x00, 0x40, 0x20, 0x00, 0x88, 0x88, 0x88, 0x88, 0x78, 0x00, 0x00, 0x00, 0x10,
	0x20, 0x00, 0x88, 0x88, 0x88, 0x88, 0x78, 0x00, 0x00, 0x00, 0x20, 0x50, 0x00, 0x88, 0x88, 0x88, 0x88, 0x78, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x88, 0x88, 0x88, 0x88, 0x78, 0x00, 0x00, 0x00,
	0x10, 0x20, 0x00, 0x88, 0x88, 0x88, 0x88, 0x78, 0x08, 0xF0, 0x00, 0x00, 0x80, 0x80, 0xF0, 0x88, 0x88, 0x88, 0xF0, 0x80, 0x80, 0x00, 0x00, 0x50, 0x00, 0x88, 0x88, 0x88, 0x88, 0x78, 0x08, 0xF0,
};
