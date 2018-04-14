/************************************************************************

    JOYCE v1.90 - Amstrad PCW emulator

    Copyright (C) 1996, 2001  John Elliott <seasip.webmaster@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*************************************************************************/

#include <SDL.h>
#include <string>
#include "UiTypes.hxx"
#include "UiControl.hxx"
#include "UiPicture.hxx"

UiPicture::UiPicture(UiDrawer *d, SDL_Surface *s) : UiControl(d)
{
	m_surface = s;
}

UiPicture::~UiPicture()
{	
//?	SDL_FreeSurface(s);
}


Uint16 UiPicture::getHeight()
{
	return m_surface->h;
}


Uint16 UiPicture::getWidth()
{
	return m_surface->w;
}


void UiPicture::draw(int selected)
{
	m_drawer->drawPicture(selected, getX(), getY(), m_surface);
}


