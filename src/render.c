/*
 * Heart of The Alien: Renderer
 * Copyright (c) 2004 Gil Megidish
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include <string.h>
#include <SDL.h>
#include "debug.h"
#include "render.h"
#include "game2bin.h"

#include "client.h"
#include "scale2x.h"
#include "scale3x.h"
#include "scale800x480.h"

static int fullscreen = 0;
static int scroll_reg = 0;

extern int fullscreen_flag;
extern SDL_Surface *screen;

static int palette_changed = 0;
static int current_palette = 0;
static SDL_Color palette[256];

/** Returns the current palette used
    @returns palette

    Palettes are stored in game2.bin
*/
int get_current_palette()
{
	return current_palette;
}

/** Sets VSCROLL value
    @param scroll
*/
void set_scroll(int scroll)
{
	scroll_reg = scroll;
}

/** Returns VSCROLL register value
    @returns vscroll
*/
int get_scroll_register()
{
	return scroll_reg;
}

void render1x(char *src)
{
	int y, p;

	if (scroll_reg == 0)
	{
		/* no scroll */
		for (y=0; y<192; y++)
		{
			memcpy((char *)screen->pixels + y*screen->pitch, src + 304*y, 304);
		}
	}
	else if (scroll_reg < 0)
	{
		/* scroll from bottom */
		p = - scroll_reg;
		for (y=p; y<192; y++)
		{
			memcpy((char *)screen->pixels + (y-p)*screen->pitch, src + 304*y, 304);
		}

		for (y=192; y<192+p; y++)
		{
			memcpy((char *)screen->pixels + (y-p)*screen->pitch, src + 304*191, 304);
		}
	}
	else
	{
		/* scroll from top */
		p = scroll_reg;
		for (y=0; y<p; y++)
		{
			memcpy((char *)screen->pixels + y*screen->pitch, src, 304);
		}

		for (y=p; y<192; y++)
		{
			memcpy((char *)screen->pixels + y*screen->pitch, src + 304*(y-p), 304);
		}
	}
}

static void normal2x_line(Uint8 *dstpix, int dstpitch, char *src, int height)
{
	int x, y;
	unsigned char wide[304*2];
	unsigned char *widep;

	widep = wide;
	for (x=0; x<304; x++)
	{
		*widep++ = *src;
		*widep++ = *src++;
	}

	for (y=0; y<height; y++)
	{
		memcpy(dstpix + y*2*dstpitch, wide, 304*2);
		memcpy(dstpix + (y*2+1)*dstpitch, wide, 304*2);
	}
}

static void normal3x_line(Uint8 *dstpix, int dstpitch, char *src, int height)
{
	int x, y;
	unsigned char wide[304*3];
	unsigned char *widep;

	widep = wide;
	for (x=0; x<304; x++)
	{
		*widep++ = *src;
		*widep++ = *src;
		*widep++ = *src++;
	}

	for (y=0; y<height; y++)
	{
		memcpy(dstpix + y*3*dstpitch, wide, 304*3);
		memcpy(dstpix + (y*3+1)*dstpitch, wide, 304*3);
		memcpy(dstpix + (y*3+2)*dstpitch, wide, 304*3);
	}
}

static void normal800x480_line(Uint8 *dstpix, int dstpitch, char *src, int dstheight)
{
	int x, y;
	unsigned char wide[800];
	unsigned char *widep;
	unsigned int delta, num_pixels;

	delta = (12 << 24) / 19; // = (800-2*304) / 304
	widep = wide;

	num_pixels = 1 << 23; // = 0.5
	for (x=0; x<304; x++)
	{
		num_pixels += delta;
		if (num_pixels & (1 << 24))
		{
			num_pixels -= (1 << 24);
			*widep++ = *src;
		}
		*widep++ = *src;
		*widep++ = *src++;
	}

	for (y=0; y<dstheight; y++)
	{
		memcpy(dstpix + y*dstpitch, wide, 800);
	}
}

/* advmame2x scaler */
void render2x_scaled(char *src)
{
	if (scroll_reg >= 0)
	{
		if (scroll_reg != 0)
		{
			normal2x_line((Uint8*)screen->pixels, screen->pitch, src, scroll_reg);
		}
		scale2x(((Uint8*)screen->pixels) + 2*scroll_reg*screen->pitch, screen->pitch, (Uint8 *)src, 304, 304, 192-scroll_reg);
	}
	else
	{
		scale2x((Uint8*)screen->pixels, screen->pitch, ((Uint8 *)src) - 304*scroll_reg, 304, 304, 192+scroll_reg);
		normal2x_line(((Uint8*)screen->pixels) + 2*(192+scroll_reg)*screen->pitch, screen->pitch, src + 304*191, -scroll_reg);
	}
}

/* advmame3x scaler */
void render3x_scaled(char *src)
{
	if (scroll_reg >= 0)
	{
		if (scroll_reg != 0)
		{
			normal3x_line((Uint8*)screen->pixels, screen->pitch, src, scroll_reg);
		}
		scale3x(((Uint8*)screen->pixels) + 3*scroll_reg*screen->pitch, screen->pitch, (Uint8 *)src, 304, 304, 192-scroll_reg);
	}
	else
	{
		scale3x((Uint8*)screen->pixels, screen->pitch, ((Uint8 *)src) - 304*scroll_reg, 304, 304, 192+scroll_reg);
		normal3x_line(((Uint8*)screen->pixels) + 3*(192+scroll_reg)*screen->pitch, screen->pitch, src + 304*191, -scroll_reg);
	}
}

/** Simple X2 scaler
    @param src
*/
void render2x(char *src)
{
	int x, y, srcy;
	unsigned char wide[304*2];

	for (y=0; y<192; y++)
	{
		char *srcp;
		unsigned char *widep;

		srcy = y - scroll_reg;
		if (srcy < 0) srcy = 0;
		if (srcy >= 192) srcy = 191;
		srcp = src + 304*srcy;
		widep = wide;
		for (x=0; x<304; x++)
		{
			*widep++ = *srcp;
			*widep++ = *srcp++;
		}

		memcpy((char *)screen->pixels + y*2*screen->pitch, wide, 304*2);
		memcpy((char *)screen->pixels + (y*2+1)*screen->pitch, wide, 304*2);
	}
}

/** Simple X3 scaler
    @param src
*/
void render3x(char *src)
{
	int x, y, srcy;
	unsigned char wide[304*3];

	for (y=0; y<192; y++)
	{
		char *srcp;
		unsigned char *widep;

		srcy = y - scroll_reg;
		if (srcy < 0) srcy = 0;
		if (srcy >= 192) srcy = 191;
		srcp = src + 304*srcy;
		widep = wide;
		for (x=0; x<304; x++)
		{
			*widep++ = *srcp;
			*widep++ = *srcp;
			*widep++ = *srcp++;
		}

		memcpy((char *)screen->pixels + y*3*screen->pitch, wide, 304*3);
		memcpy((char *)screen->pixels + (y*3+1)*screen->pitch, wide, 304*3);
		memcpy((char *)screen->pixels + (y*3+2)*screen->pitch, wide, 304*3);
	}
}

void render800x480(char *src)
{
	int x, y, srcy;
	unsigned char wide[800], *dst;
	unsigned int delta, num_pixels;

	dst = (unsigned char *)screen->pixels;
	delta = (12 << 24) / 19; // = (800-2*304) / 304
	for (y=0; y<192; y++)
	{
		char *srcp;
		unsigned char *widep;

		srcy = y - scroll_reg;
		if (srcy < 0) srcy = 0;
		if (srcy >= 192) srcy = 191;
		srcp = src + 304*srcy;
		widep = wide;

		num_pixels = 1 << 23; // = 0.5
		for (x=0; x<304; x++)
		{
			num_pixels += delta;
			if (num_pixels & (1 << 24))
			{
				num_pixels -= (1 << 24);
				*widep++ = *srcp;
			}
			*widep++ = *srcp;
			*widep++ = *srcp++;
		}

		memcpy(dst, wide, 800);
		dst += screen->pitch;
		memcpy(dst, wide, 800);
		dst += screen->pitch;
		if (y & 1)
		{
			memcpy(dst, wide, 800);
			dst += screen->pitch;
		}
	}
}

void render800x480_scaled(char *src)
{
	if (scroll_reg >= 0)
	{
		if (scroll_reg != 0)
		{
			normal800x480_line((Uint8*)screen->pixels, screen->pitch, src, (scroll_reg * 5) >> 1);
		}
		scale800x480(((Uint8*)screen->pixels) + ((scroll_reg * 5) >> 1)*screen->pitch, screen->pitch, (Uint8 *)src, 192-scroll_reg);
	}
	else
	{
		scale800x480((Uint8*)screen->pixels, screen->pitch, ((Uint8 *)src) - 304*scroll_reg, 192+scroll_reg);
		normal800x480_line(((Uint8*)screen->pixels) + (480-((-scroll_reg * 5) >> 1))*screen->pitch, screen->pitch, src + 304*191, (-scroll_reg * 5) >> 1);
	}
}

/** Renders a virtual screen
    @param src
*/
void render(char *src)
{
	SDL_LockSurface(screen);

	if (palette_changed)
	{
		palette_changed = 0;
		SDL_SetColors(screen, palette, 0, 256);
	}

	switch(cls.scale)
	{
		case 1:
		/* normal 1x */
		render1x(src);
		break;

		case 2:
		if (cls.filtered == 0)
		{
			render2x(src);
		}
		else
		{
			render2x_scaled(src);
		}
		break;

		case 3:
		if (cls.filtered == 0)
		{
			if (cls.pandora)
			{
				render800x480(src);
			}
			else
			{
				render3x(src);
			}
		}
		else
		{
			if (cls.pandora)
			{
				render800x480_scaled(src);
			}
			else
			{
				render3x_scaled(src);
			}
		}
		break;
	}

	scroll_reg = 0;
	SDL_UnlockSurface(screen);
	SDL_Flip(screen);
}

/** Module initializer
    @returns zero on success
*/
int render_init()
{
	return 0;
}

/** Converts a Sega CD RGB444 to RGB888
    @param rgb12   pointer to 16x2 of palette data
*/
void set_palette_rgb12(unsigned char *rgb12)
{
	int i;

	for (i=0; i<16; i++)
	{
		int c, r, g, b;

		c = (rgb12[i*2] << 8) | rgb12[i*2+1];
		r = (c & 0xf) << 4;
		g = ((c >> 4) & 0xf) << 4;
		b = ((c >> 8) & 0xf) << 4;

		palette[i].r = r | (r >> 4);
		palette[i].g = g | (g >> 4);
		palette[i].b = b | (b >> 4);
	}

	palette_changed = 1;
}

void set_palette(int which)
{
	unsigned char rgb12[16*2];

	copy_from_game2bin(rgb12, 0x5cb8 + (which * 16 * 2), sizeof(rgb12));

	current_palette = which;
	set_palette_rgb12(rgb12);

	palette[255].r = 255;
	palette[255].g = 0;
	palette[255].b = 255;
}

void toggle_fullscreen()
{
	/* hack, fullscreen not supported at scale==3 */
	if ((cls.scale == 3) && !cls.pandora)
	{
		return;
	}

	fullscreen = 1 ^ fullscreen;
	screen = 0;

	if (fullscreen == 0)
	{
		LOG(("create SDL surface of 304x192x8\n"));

		if (cls.pandora && (cls.scale == 3))
		{
			screen = SDL_SetVideoMode(800, 480, 8, SDL_SWSURFACE);
		}
		else
		{
			screen = SDL_SetVideoMode(304*cls.scale, 192*cls.scale, 8, SDL_SWSURFACE);
		}
		SDL_SetColors(screen, palette, 0, 256);
		SDL_ShowCursor(1);
	}
	else
	{
		int w, h;

#if defined(PANDORA)
		w = 304*cls.scale;
		h = 192*cls.scale;
#else
		w = 320*cls.scale;
		h = 200*cls.scale;
#endif

		LOG(("setting fullscreen mode %dx%dx8\n", w, h));

		if (cls.pandora && (cls.scale == 3))
		{
			screen = SDL_SetVideoMode(800, 480, 8, SDL_DOUBLEBUF|SDL_HWSURFACE|SDL_FULLSCREEN);
		}
		else
		{
			screen = SDL_SetVideoMode(w, h, 8, SDL_DOUBLEBUF|SDL_HWSURFACE|SDL_FULLSCREEN);
		}

		SDL_SetColors(screen, palette, 0, 256);
		SDL_ShowCursor(0);
	}
}

int render_create_surface()
{
	if (cls.pandora && (cls.scale == 3))
	{
		screen = SDL_SetVideoMode(800, 480, 8, SDL_SWSURFACE);
	}
	else
	{
		screen = SDL_SetVideoMode(304*cls.scale, 192*cls.scale, 8, SDL_SWSURFACE);
	}
	if (screen == NULL)
	{
		return -1;
	}

	SDL_WM_SetCaption("Heart of The Alien Redux", 0);

	if (fullscreen_flag)
	{
		toggle_fullscreen();
	}

	return 0;
}
