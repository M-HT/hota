/*
 * Heart of The Alien: client static
 * Copyright (c) 2005 Gil Megidish
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
#ifndef __CLIENT_INCLUDED__
#define __CLIENT_INCLUDED__

typedef struct
{
	int paused;
	int quit;

	int scale;
	int use_iso;
	int nosound;
	int fullscreen;
	int speed_throttle;
	int filtered;
	char *iso_prefix;
} client_static_t;

extern client_static_t cls;

#endif
