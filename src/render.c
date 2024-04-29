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

static int fullscreen = 0;
static int scroll_reg = 0;

extern int fullscreen_flag;
static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static SDL_Surface *screen;
static SDL_Palette *screen_palette;
static Uint32 texture_palette[16];
static int texture_red_low;
#if SDL_VERSION_ATLEAST(2,0,18)
static int renderer_change_sync;
#endif

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

/** Renders a virtual screen
    @param src
*/
void render(char *src)
{
	Uint32 *texture_pixels;
	Uint8 *screen_pixels;
	int texture_pitch, h, x;

	SDL_LockSurface(screen);

	if (palette_changed)
	{
		palette_changed = 0;
		SDL_SetPaletteColors(screen_palette, palette, 0, 256);
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
			render3x(src);
		}
		else
		{
			render3x_scaled(src);
		}
		break;
	}

	scroll_reg = 0;

	SDL_LockTexture(texture, NULL, (void **) &texture_pixels, &texture_pitch);

	screen_pixels = (Uint8 *)screen->pixels;
	for (h = screen->h; h != 0; h--)
	{
		for (x = 0; x < screen->w; x++)
		{
			texture_pixels[x] = texture_palette[screen_pixels[x]];
		}
		screen_pixels += screen->pitch;
		texture_pixels = (Uint32 *) (texture_pitch + (Uint8 *)texture_pixels);
	}

	SDL_UnlockTexture(texture);
	SDL_UnlockSurface(screen);

	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
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

		texture_palette[i] = (texture_red_low) ? (palette[i].r | (palette[i].g << 8) | (palette[i].b << 16)) : (palette[i].b | (palette[i].g << 8) | (palette[i].r << 16));
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
	fullscreen = 1 ^ fullscreen;

	if (fullscreen == 0)
	{
		LOG(("create SDL surface of 304x192\n"));

		SDL_SetWindowFullscreen(window, 0);
#if SDL_VERSION_ATLEAST(2,0,18)
		if (renderer_change_sync) SDL_RenderSetVSync(renderer, 0);
#endif

		SDL_ShowCursor(1);
	}
	else
	{
		LOG(("setting fullscreen mode\n"));

		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
#if SDL_VERSION_ATLEAST(2,0,18)
		if (renderer_change_sync) SDL_RenderSetVSync(renderer, 1);
#endif

		SDL_ShowCursor(0);
	}
}

static void atexit_callback(void)
{
	SDL_FreeSurface(screen);
	SDL_FreePalette(screen_palette);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

int render_create_surface()
{
	Uint32 flags, format;
	int index;
	SDL_RendererInfo info;

#if SDL_VERSION_ATLEAST(2,0,18)
	SDL_version linked;
	SDL_GetVersion(&linked);
	renderer_change_sync = 0;
	if (SDL_VERSIONNUM(linked.major, linked.minor, linked.patch) >= SDL_VERSIONNUM(2, 0, 18))
	{
		renderer_change_sync = 1;
	}
#endif

	window = SDL_CreateWindow("Heart of The Alien Redux", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 304*cls.scale, 192*cls.scale, SDL_WINDOW_HIDDEN);
	if (window == NULL)
	{
		return -1;
	}

	flags = SDL_RENDERER_PRESENTVSYNC;
#if SDL_VERSION_ATLEAST(2,0,18)
	if (renderer_change_sync)
	{
		flags = 0;
	}
#endif

	renderer = SDL_CreateRenderer(window, -1, flags);
	if (renderer == NULL)
	{
		return -2;
	}

#ifndef PYRA
	SDL_RenderSetLogicalSize(renderer, 304*cls.scale, 192*cls.scale);
#endif

	format = 0;

	if (SDL_GetRendererInfo(renderer, &info) <= 0)
	{
		for (index = 0; index < (int)info.num_texture_formats; index++)
		{
			if (info.texture_formats[index] == SDL_PIXELFORMAT_ARGB8888 || info.texture_formats[index] == SDL_PIXELFORMAT_ABGR8888 || info.texture_formats[index] == SDL_PIXELFORMAT_RGB888 || info.texture_formats[index] == SDL_PIXELFORMAT_BGR888)
			{
				format = info.texture_formats[index];
				break;
			}
		}
	}

	if (format == 0) format = SDL_PIXELFORMAT_ABGR8888;

	texture_red_low = (format == SDL_PIXELFORMAT_ABGR8888 || format == SDL_PIXELFORMAT_BGR888) ? 1 : 0;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	texture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_STREAMING, 304*cls.scale, 192*cls.scale);
	if (texture == NULL)
	{
		return -3;
	}

	screen = SDL_CreateRGBSurface(0, 304*cls.scale, 192*cls.scale, 8, 0, 0, 0, 0);
	if (screen == NULL)
	{
		return -4;
	}

	screen_palette = SDL_AllocPalette(256);
	if (screen_palette == NULL)
	{
		return -5;
	}

	SDL_SetSurfacePalette(screen, screen_palette);

	SDL_ShowWindow(window);

	if (fullscreen_flag)
	{
		toggle_fullscreen();
	}

	atexit(atexit_callback);

	return 0;
}

