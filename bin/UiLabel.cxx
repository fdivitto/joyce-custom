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
#include "UiLabel.hxx"

UiLabel::UiLabel(string caption, UiDrawer *d) : UiControl(d)
{
	m_caption = caption;
}

UiLabel::~UiLabel() 
{

}


void UiLabel::draw(int selected)
{
	SDL_Rect rcb = getBounds();
	int w        = getWidth();

	m_drawer->drawString(selected, getX(), getY(), m_caption);
	rcb.x += w;
	rcb.w -= w;
	if (rcb.w > 0) m_drawer->drawRect  (selected, rcb);
}

Uint16 UiLabel::getHeight()
{
	Uint16 x, y;

	m_drawer->measureString(m_caption, &x, &y);
	return y;
}


Uint16 UiLabel::getWidth()
{
	Uint16 x, y;

	m_drawer->measureString(m_caption, &x, &y);
	return x;	
}





