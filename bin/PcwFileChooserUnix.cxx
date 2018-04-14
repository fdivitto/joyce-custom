/************************************************************************

    JOYCE v2.1.0 - Amstrad PCW emulator

    Copyright (C) 1996, 2001-2  John Elliott <seasip.webmaster@gmail.com>

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
#include <vector>
#include "Pcw.hxx"

#ifdef UNIX

#include "UiContainer.hxx"
#include "UiMenu.hxx"
#include "UiLabel.hxx"
#include "UiSeparator.hxx"
#include "UiCommand.hxx"
#include "UiTextInput.hxx"
#include "UiScrollingMenu.hxx"
#include "PcwFileList.hxx"
#include "PcwFileChooser.hxx"
#include "PcwFileEntry.hxx"
#include "Path.hxx"

PcwFileChooser::PcwFileChooser(const string okCaption, UiDrawer *d) : 
		UiContainer(d), m_menuL(16, 13, d), m_menuR(d)
{
	m_path = new Path;

	m_dir  = m_path->getPathName();	
	m_menuR.add (new UiLabel    ("     JOYCE directories    ", d));
	m_menuR.add (new UiSeparator(d));
	m_menuR.add (new UiCommand  (SDLK_s, "  System boot files  ", d));
	m_menuR.add (new UiCommand  (SDLK_u, "  User   boot files  ", d));
	m_menuR.add (new UiCommand  (SDLK_d, "  System Disc files  ", d));
	m_menuR.add (new UiCommand  (SDLK_f, "  User   disc Files  ", d));
	m_menuR.add (new UiCommand  (SDLK_PERIOD, "  Current directory  ", d));
	m_menuR.add (new UiSeparator(d));
	m_menuR.add (new UiTextInput(SDLK_r, "  Directory: ____________________________  ", d));
	m_menuR.add (new UiTextInput(SDLK_n, "  Filename:  ____________________________  ", d));  
	m_menuR.add (new UiSeparator(d));
	m_menuR.add (new UiCommand  (SDLK_o,   okCaption, d));
	m_menuR.add (new UiCommand  (SDLK_ESCAPE, "  EXIT  ", d));

	SDL_Rect rc, rco;

	d->measureString("1234567890123456789123456789001234567890",&rc.w,&rc.h);
	m_lwi = rc.w;
	d->measureMenuBorder(rc, rco);	
	m_lwo = rco.w;

	((UiTextInput &)m_menuR[8]).setText(m_path->getPathName());
	((UiTextInput &)m_menuR[9]).setText("");
	m_pane = 0;
}


PcwFileChooser::~PcwFileChooser()
{
	delete m_path;
};



void PcwFileChooser::draw(int selected)
{
	m_menuL.setX(getX());
	m_menuL.setY(getY());
	m_menuL.setW(m_lwo, m_lwi);
	m_menuL.draw(selected && m_pane == 0);

	m_menuR.setY(getY());
	m_menuR.setX(getX() + m_lwo);
	SDL_Rect rc;
	
	rc.x = m_menuR.getX();
	rc.y = m_menuR.getY();
	rc.w = m_menuR.getWidth();
	rc.h = m_menuR.getHeight();
	m_menuR.setBounds(rc);

	m_menuR.draw(selected && m_pane == 1);
}


void PcwFileChooser::pack(void)
{
	//SDL_Rect rc;

        m_menuL.setX(getX());
        m_menuL.setY(getY());
	m_menuR.setY(getY());
	
	m_menuL.pack();
	m_menuR.pack();

	m_menuL.setW(m_lwo, m_lwi);
	m_w = m_lwo + m_menuR.getWidth();
	m_h = m_menuR.getHeight();
}


Uint16 PcwFileChooser::getWidth(void)
{
	return m_w;
}

Uint16 PcwFileChooser::getHeight(void)
{
	return m_h;
}


UiEvent PcwFileChooser::onEvent(SDL_Event &ev)
{
	UiEvent e;

        switch(ev.type)
        {
                case SDL_KEYDOWN: 
                        return onKey(ev);

                case SDL_MOUSEBUTTONDOWN:
			if (m_menuL.hitTest(ev.button.x, ev.button.y))
			{
				m_pane = 0;
				draw(1);
				e = m_menuL.onEvent(ev);
				if (e == UIE_OK || e == UIE_SELECT) return selectL();
				return e;
			}
			if (m_menuR.hitTest(ev.button.x, ev.button.y))
			{
				m_pane = 1;
				draw(1);
				e = m_menuR.onEvent(ev);
				if (e == UIE_OK || e == UIE_SELECT) return selectR();
				return e;
			}
                        return onMouse(ev.button.x, ev.button.y);
        }
	return UiContainer::onEvent(ev);	
}



UiEvent PcwFileChooser::onKey(SDL_Event &ev)
{
	SDLKey k = ev.key.keysym.sym;
	UiEvent e;
	SDLMod m;

	m = SDL_GetModState();
	
	switch (k)
	{
                case SDLK_END:                  /* ESCAPE */
                case SDLK_ESCAPE:       return UIE_CANCEL;

                case SDLK_KP_ENTER:
                case SDLK_RETURN:
				if (m_pane == 0) return selectL();
				return			selectR();

		case SDLK_TAB:

			m_pane = 1 - m_pane;
			draw(1);
			return UIE_CONTINUE;
		case SDLK_LEFT:
		case SDLK_RIGHT:
			if (m & (KMOD_SHIFT))
			{
				m_pane = 1 - m_pane;
				draw(1);
				return UIE_CONTINUE;
			}	
			/* FALL THROUGH */	
		default: 
			if (m_pane == 0) 
			{
				e = m_menuL.onEvent(ev);
				return e;
			}
			else
			{
				e = m_menuR.onEvent(ev);
				if (e == UIE_QUIT || e == UIE_CANCEL) return e;

				if (e == UIE_OK || e == UIE_SELECT)
					return selectR();				
			}
	}
	return UIE_CONTINUE;
}

UiEvent PcwFileChooser::selectL(void)
{
	int ns = m_menuL.getSelected();

	string sname    = ((PcwFileEntry &)m_menuL[ns]).getFilename();
	struct stat &st = ((PcwFileEntry &)m_menuL[ns]).stat();
		// If it's a directory, change to it
	if (S_ISDIR(st.st_mode))
	{
		changeDir(sname, true);
		return UIE_CONTINUE;
	} 
	// If it's a file, open it
	m_choice = constructPath(m_dir, sname);
	return UIE_OK;
}

UiEvent PcwFileChooser::selectR(void)
{
	string ndir;
	string sname, lname;
	struct stat st;
	Path *p;

	switch (m_menuR.getSelected())
	{
		case 2:	changeDir(findPcwDir(FT_BOOT, true )); return UIE_CONTINUE;
		case 3: changeDir(findPcwDir(FT_BOOT, false)); return UIE_CONTINUE;
		case 4:	changeDir(findPcwDir(FT_DISC, true )); return UIE_CONTINUE;
		case 5: changeDir(findPcwDir(FT_DISC, false)); return UIE_CONTINUE;
		case 6: changeDir("."); return UIE_CONTINUE;

		case 8:	// Entered dirname
			sname = ((UiTextInput &)m_menuR[8]).getText();
			p = new Path(sname);
			while (stat(sname.c_str(), &st))
			{
				if (!p->up()) return UIE_CONTINUE;
				sname = p->getPathName();	
			}
			if (S_ISDIR(st.st_mode)) changeDir(sname);
			return UIE_CONTINUE;
 
		case 9: // Entered filename
			sname = ((UiTextInput &)m_menuR[9]).getText();
			lname = constructPath(m_dir, sname);
			if (!stat(lname.c_str(), &st))
			{
				// If it's a directory, change to it
				if (S_ISDIR(st.st_mode))
				{
					changeDir(lname);
					return UIE_CONTINUE;
				} 
				// If it's a file, open it
				m_choice = lname;
				return UIE_OK;
			}
			// Can't stat
			return UIE_CONTINUE;

		case 11: sname = ((UiTextInput &)m_menuR[9]).getText();
			 lname = constructPath(m_dir, sname);
			 m_choice = lname;
			 return UIE_OK;
		case 12: return UIE_CANCEL;	
	}
	return UIE_CONTINUE;
}


void PcwFileChooser::initDir(string name)
{
	struct stat st;

        if (!stat(name.c_str(), &st))
        {
		if (!S_ISDIR(st.st_mode))
		{
			int sl = name.rfind('/');
			if (sl >= 0) name = name.substr(0, sl);	
		}
	}
	if (m_path->chdirAbs(name)) m_dir = m_path->getPathName();
}


void PcwFileChooser::changeDir(string s, bool relative)
{
	if (relative  && !m_path->chdir(s)) return;
	if (!relative && !m_path->chdirAbs(s)) return;

        m_dir  = m_path->getPathName();

        ((UiTextInput &)m_menuR[8]).setText(m_path->getPathName());
	m_menuR[8].draw(0);

	vector<string> v;
	m_path->listFiles(v);

	string d = constructPath(m_dir, "");

	m_menuL.fill(v, d);
        m_menuL.setW(m_lwo, m_lwi);
	m_menuL.draw(m_pane == 0);

}



UiEvent PcwFileChooser::eventLoop(void)
{
	SDL_Rect rc; 
        pack();
	m_drawer->centreContainer(*this);	
	rc.x = getX();
	rc.y = getY();
	rc.w = m_w;
	rc.h = m_h;

        SDL_Surface *surface = m_drawer->saveSurface(rc);

        draw(1);
	m_menuL.setSelected(0);
	m_menuR.setSelected(8);
	m_menuR.draw(0);
	
	changeDir(m_path->getPathName());
	draw(1);

        UiEvent e = UiContainer::eventLoop();

	m_drawer->restoreSurface(surface, rc);
	return e;
}


string PcwFileChooser::constructPath(string dir, string file)
{
	if (dir == "") return file;
	char c = dir[dir.size() - 1];

	if (c == '/') return dir + file;
	return dir + "/" + file;
}

#endif // def UNIX
