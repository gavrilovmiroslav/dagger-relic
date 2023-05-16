#include "PostProcessRendering.h"
#include "Prelude.h"
#include "Scene.h"

extern U8 r_image_font_6_11[2464];

/*
 * Context for the software renderer.
 */
struct Bitmap
{
    U32  width, height;
    U32 *pixel;
};

static void render_puts(struct Bitmap *bm, U32 sx, U32 sy, const char *text);
static void render_line(struct Bitmap *bm, U32 x0, U32 y0, U32 x1, U32 y1, U32 colour);

/*
 * Draw text to bitmap.
 * The order of drawing a character is right to left.
 * NOTE: Cannot start drawing with negative x and y values.
 */
void render_puts(struct Bitmap *bm, U32 sx, U32 sy, const char *text)
{
	U32       j      = 0;
	const U32 colour = 0xffffffff;

	if (sy >= bm->height || text == NULL)
	{
		return;
	}

	while (text[j])
	{
		U32   i;
		const char ch = text[j];

		if (ch >= 32 && ch <= 126)
		{
			for (i = 0; i < 11 && sy + i < bm->height; i++)
			{
				U8  test = '?';
				U8  mask = 1;
				U32 x    = sx + 7;

				test = r_image_font_6_11[(U32)(ch - 32) * 11 + i];

				while (mask != 0)
				{
					const U32 map = (sy + i) * bm->width + (x);

					/* Must check for each iteration as x is unsigned and may underflow. */
					if (x < bm->width)
					{
						bm->pixel[map] = (test & mask ? colour : bm->pixel[map]);
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
 * NOTE: Offscreen positions are handled by clamping to the extent of the bitmap,
 * this is only meant for drawing lines within the screen.
 */
void render_line(struct Bitmap *bm, U32 x0, U32 y0, U32 x1, U32 y1, U32 colour)
{
	I32 dx,  dy;
	I32 sx,  sy;
	I32 err, e2;

    /*
     * Clamp values to the extent of the bitmap.
     * Probably extraneous as positions should be valid in the first place,
     * else something is horribly wrong (with the caller).
     * Clamping should be done using linear interpolation (lerp)... but this should be removed anyway...
     * a*(1.0f-x)+(b*x), a, b : range, x : value.
     */
	if (x0 > bm->width)  { x0 = bm->width;  }
	if (x1 > bm->width)  { x1 = bm->width;  }
	if (y0 > bm->height) { y0 = bm->height; }
	if (y1 > bm->height) { y1 = bm->height; }

	dx  = abs((I32) x1 - (I32) x0);
	sx  = (x0 > x1 ? 1 : -1);
	dy  = abs((I32) y1 - (I32) y0);
	sy  = (y0 > y1 ? 1 : -1);
	err = (dx > dy ? dx : -dy) / 2;

	for (;;)
	{
		if (x0 < bm->width && y0 < bm->height)
		{
			bm->pixel[x0 + y0 * bm->width] = colour;
		}
		if (x0 == x1 && y0 == y1)
		{
			break;
		}

		e2 = err;
		if (e2 > -dx)
		{
			err -= dy;
			x0 = (U32) ((I32) x0 - sx);
		}
		if (e2 < dy)
		{
			err += dx;
			y0 = (U32) ((I32) y0 - sy);
		}
	}
}

void PostProcessLineRenderingModule::process_signal(core::PostProcessRenderSignal& signal)
{
	for (const auto&& [ entity, line ] : AccessStorage<PostProcessLine>::access_storage().each())
	{
        struct Bitmap bm;

        bm.pixel  = signal.pixels;
        bm.width  = (U32) signal.w;
        bm.height = (U32) signal.h;
        render_line(&bm, (U32) line.start.x, (U32) line.start.y, (U32) line.end.x, (U32) line.end.y, line.colour);
	}
}

void PostProcessTextRenderingModule::process_signal(core::PostProcessRenderSignal& signal)
{
	for (const auto&& [ entity, textdata ] : AccessStorage<PostProcessText>::access_storage().each())
	{
        struct Bitmap bm;

        bm.pixel  = signal.pixels;
        bm.width  = (U32) signal.w;
        bm.height = (U32) signal.h;
        render_puts(&bm, (U32) textdata.position.x, (U32) textdata.position.y, textdata.text.c_str());
	}
}

F32        postprocess_fade_timer = 0.0f;
Bool       postprocess_fade_force = false;
U32        dynamiclight_x         = 0;
U32        dynamiclight_y         = 0;
static U32 buffer_vignette[960*960];
static U8  buffer_lightmap[960*960];
static U8  buffer_decal[960*960];

static const U8 lightmap_shadow = 0x10u;
static const U8 lightmap_light  = 0x5fu;

void Lightmap_Calculate(std::vector<Lightmap_Block> &block)
{
    U32 i, j;
    const U32 bluramount = 10;

    for (i = 0; i < 960*960; i++)
    {
        buffer_lightmap[i] = lightmap_light;
    }

    for (i = 0; i < block.size(); i++)
    {
        U32 x, y;

        for (x = block[i].x; x < block[i].x + block[i].w; x++)
        {
            for (y = block[i].y; y < block[i].y + block[i].h; y++)
            {
                buffer_lightmap[x + y * 960] = lightmap_shadow;
            }
        }
    }

    /* Blur lightmap buffer... */
    for (j = 0; j < 960; j++)
    {
        for (i = 0; i < 960; i++)
        {
            U32 x, y;
            U32 c = 0;
            U32 k = 0;

            for (x = i; x <= i+bluramount*2; x++)
            {
                for (y = j; y <= j+bluramount*2; y++)
                {
                    const U32 x2 = x-bluramount;
                    const U32 y2 = y-bluramount;

                    if (x2 < 960 && y2 < 960)
                    {
                        c += buffer_lightmap[x2 + y2 * 960];
                    }
                    else
                    {
                        c += lightmap_shadow;
                    }
                    k++;
                }
            }

            if (k > 0)
            {
                buffer_lightmap[i + j * 960] = (U8) (c / k);
            }
        }
    }
}

void Decal_Stamp(U32 x, U32 y, U32 what)
{
	U32 i, j;

	switch (what)
	{
	case DECAL_FOOTPRINT_BIPED1:
	case DECAL_TEST:
		for (j = y; j < y+4; j++)
		{
			for (i = x; i < x+8; i++)
			{
				if (x >= 0 && x < 960 && y >= 0 && y < 960)
				{
					buffer_decal[j*960+i] = 0x80;
				}
			}
		}
	break;
	case DECAL_FOOTPRINT_BIPED2:
		for (j = y+6; j < y+6+4; j++)
		{
			for (i = x+1; i < x+1+8; i++)
			{
				if (x >= 0 && x < 960 && y >= 0 && y < 960)
				{
					buffer_decal[j*960+i] = 0x80;
				}
			}
		}
	break;
	case DECAL_BOXGRIND:
		for (j = y; j < y+32; j++)
		{
			for (i = x; i < x+32; i++)
			{
				if (x >= 0 && x < 960 && y >= 0 && y < 960)
				{
					buffer_decal[j*960+i] = 0x9f;
				}
			}
		}
	break;
	default:
	break;
	}
}

void Decal_Clear(void)
{
	memset(buffer_decal, 0, 960*960);
}

/*
 * TODO: This is slow because the engine is not designed for this... Maybe use SIMD?
 */
void PostProcessTestRenderingModule::process_signal(core::PostProcessRenderSignal& signal)
{
	if(!scene.in_game)
	{
		memset(signal.pixels, 0xff, signal.h * signal.w * sizeof(U32));
		return;
	}

	static Bool buffercreate      = true;
	const  U32  dynamiclight_size = 144;
	static U32  switchcolour      = 0;

	if (buffercreate)
	{
		U32 x, y;

		buffercreate = false;

		for (y = 0; y < signal.h; y++)
		{
			for (x = 0; x < signal.w; x++)
			{
				F32 dx = x-(signal.w/2.0f);
				F32 dy = y-(signal.h/2.0f);
				F32 d  = sqrtf(dx*dx+dy*dy);
				F32 v  = d/signal.w;
				U32 a  = (U32) (v*255);
				U32 c  = 0;

				buffer_vignette[y * signal.w + x] = (a << 0) | (c << 16) | (c << 8) | (c << 24);
			}
		}
	}

	for (const auto&& [ entity, test ] : AccessStorage<PostProcessTest>::access_storage().each())
	{
		U32 x, y;

        switch (test.magic)
        {
        case 0:
        for (y = 0; y < signal.h; y++)
        {
            for (x = 0; x < signal.w; x++)
            {
                U32 c = x^y;

                signal.pixels[y * signal.w + x] = (c << 16) | (c << 8) | (x);
            }
        }
        break;
        case 999:
        for (y = 0; y < signal.h; y++)
        {
            for (x = 0; x < signal.w; x++)
            {
                signal.pixels[y * signal.w + x] = (x << 16) | (x << 8) | (x);
            }
        }
		case POSTPROCESS_TEST_VIGNETTE:
		for (y = 0; y < signal.h; y++)
		{
			for (x = 0; x < signal.w; x++)
			{
				signal.pixels[y * signal.w + x] = buffer_vignette[y * signal.w + x];
			}
		}
		break;
		case POSTPROCESS_TEST_FADE:
		if (postprocess_fade_timer > 0.0f || postprocess_fade_force)
		{
			for (y = 0; y < signal.h; y++)
			{
				for (x = 0; x < signal.w; x++)
				{
					F32 d = postprocess_fade_timer;
					U32 c = signal.pixels[y * signal.w + x];
					U32 a = (c >> 0)  & 0xff;

					U32 r = ((U32) (((c>>24)&0xff) * d))&0xff;
					U32 g = ((U32) (((c>>16)&0xff) * d))&0xff;
					U32 b = ((U32) (((c>> 8)&0xff) * d))&0xff;

					signal.pixels[y * signal.w + x] = (r << 24) | (g << 16) | (b << 8) | (a << 0);
				}
			}
		}
		else if (postprocess_fade_timer < 0.0f)
		{
			for (y = 0; y < signal.h; y++)
			{
				for (x = 0; x < signal.w; x++)
				{
					F32 d = 1.0f+postprocess_fade_timer;
					U32 c = signal.pixels[y * signal.w + x];
					U32 a = (c >> 0)  & 0xff;

					U32 r = ((U32) (((c>>24)&0xff) * d))&0xff;
					U32 g = ((U32) (((c>>16)&0xff) * d))&0xff;
					U32 b = ((U32) (((c>> 8)&0xff) * d))&0xff;

					signal.pixels[y * signal.w + x] = (r << 24) | (g << 16) | (b << 8) | (a << 0);
				}
			}
		}
		break;
		case POSTPROCESS_TEST_LIGHTMAP:
		for (y = 0; y < signal.h; y++)
		{
			for (x = 0; x < signal.w; x++)
			{
				U32 c = signal.pixels[y * signal.w + x];
				U32 r = (c >> 24) & 0xff;
				U32 g = (c >> 16) & 0xff;
				U32 b = (c >> 8)  & 0xff;
				U32 a = (c >> 0)  & 0xff;
				U32 c2 = (buffer_vignette[y * signal.w + x]) & 0xff;

				r  = (buffer_lightmap[y * signal.w + x]) & 0xff;
				g  = (buffer_lightmap[y * signal.w + x]) & 0xff;
				b  = (buffer_lightmap[y * signal.w + x]) & 0xff;
				a  = 0x9f;

				if (r > 255) r = 255;
				if (g > 255) g = 255;
				if (b > 255) b = 255;
				if (a > 255) a = 255;

				signal.pixels[y * signal.w + x] = (a << 0) | (r << 16) | (g << 8) | (b << 24);
			}
		}
		break;
		case POSTPROCESS_TEST_DYNAMICLIGHT:
		{
			for (y = 0; y < signal.h; y++)
			{
				for (x = 0; x < signal.w; x++)
				{
					F32 dx = (F32)x-dynamiclight_x;
					F32 dy = (F32)y-dynamiclight_y;
					F32 d  = sqrtf(dx*dx+dy*dy);

					if (d < dynamiclight_size)
					{
						U32 i;

						for (i = 0; i < 3; i++)
						{
							F32 c = ((U8*) (&signal.pixels[y * signal.w + x]))[i+1];
							F32 dl = d/dynamiclight_size;
							F32 v = (2.0f-dl)*c;
							((U8*) (&signal.pixels[y * signal.w + x]))[i+1] = (U8) v;
						}
					}
				}
			}
		}
		break;
		case POSTPROCESS_TEST_UIFRAME:
#define UIFRAME_HEIGHT 64
			// Crappy algorithm.
			for (x = 0; x < signal.w; x++)
			{
				signal.pixels[(signal.h-UIFRAME_HEIGHT-2) * signal.w + x] = 0x101010ff;
			}
			for (x = 0; x < signal.w; x++)
			{
				signal.pixels[(signal.h-UIFRAME_HEIGHT-1) * signal.w + x] = 0x070707ff;
			}
			switchcolour = (switchcolour+1)%2;
			for (y = signal.h-UIFRAME_HEIGHT; y < signal.h; y++)
			{
				for (x = 0; x < signal.w; x++)
				{
					U32 c = signal.pixels[y * signal.w + x];
					U32 r = (c >> 24) & 0xff;
					U32 g = (c >> 16) & 0xff;
					U32 b = (c >> 8)  & 0xff;
					U32 a = (c >> 0)  & 0xff;

					if (y % 2 == switchcolour)
					{
						if (x % 2 == 0)
						{
							r = 0;
							g = 0;
							b = 0;
						}
						else
						{
							r = 0x10;
							g = 0x10;
							b = 0x10;
						}
					}
					else
					{
						if (x % 2 == switchcolour)
						{
							r = 0x10;
							g = 0x10;
							b = 0x10;
						}
						else
						{
							r = 0;
							g = 0;
							b = 0;
						}
					}

					signal.pixels[y * signal.w + x] = (a << 0) | (r << 24) | (g << 16) | (b << 8);
				}
			}
		break;
		case POSTPROCESS_TEST_TRACK:
			for (y = 0; y < signal.h; y++)
			{
				for (x = 0; x < signal.w; x++)
				{
					U32 i;

					if (buffer_decal[y*signal.w+x] != 0)
					{
						for (i = 0; i < 3; i++)
						{
							((U8*) (&signal.pixels[y * signal.w + x]))[i+1] -= buffer_decal[y * signal.w + x]/32;
						}
					}
				}
			}
		break;
        default:
        break;
        }
	}
}

void PostProcessRectangleRenderingModule::process_signal(core::PostProcessRenderSignal& signal)
{
	for (const auto&& [ entity, rectangle ] : AccessStorage<PostProcessRectangle>::access_storage().each())
	{
		for (I32 y = rectangle.y; y < rectangle.y+rectangle.h; y++)
		{
			for (I32 x = rectangle.x; x < rectangle.x+rectangle.w; x++)
			{
				/* Terrible thing to do here. */
				if (x < signal.w && y < signal.h)
				{
					signal.pixels[y * signal.w + x] = 0xffffffff;
				}
			}
		}
	}
}

/*
 * Character width: 6, height: 11, total characters: 224, first character: ascii 32.
 * Each line defines 2 characters, every character is made from 11 'U8' bitmasks.
 * Converted from BDF with bdf2c.
 * NOTE: This is only for debugging purposes, this is not a font to be used in the final product.
 */
U8 r_image_font_6_11[2464] =
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
