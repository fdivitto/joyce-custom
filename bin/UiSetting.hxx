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

class UiSetting : public UiLabel
{
protected:
	bool m_checked;
public:
	UiSetting(SDLKey k, bool checked, std::string caption, UiDrawer *d);
	virtual ~UiSetting();

	virtual void draw(int selected);
	virtual UiEvent onKey(SDLKey k);
	virtual bool canFocus(void);
	
	inline bool isChecked(void)    { return m_checked; }
	inline void setChecked(bool b) { m_checked = b;    }
};




