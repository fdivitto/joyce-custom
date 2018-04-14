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

#include "Joyce.hxx"
#include "PcwMenuFont.hxx"
#include "inline.hxx"
#include "UiContainer.hxx"
#include "UiMenuBar.hxx"
#include "UiLabel.hxx"
#include "UiCommand.hxx"
#include "UiSetting.hxx"
#include "UiMenu.hxx"
#include "UiSeparator.hxx"
#include "UiMenuBarCommand.hxx"

JoyceMenuTerm::JoyceMenuTerm(JoyceSystem *s) : PcwMenuTerm(s)
{
	XLOG("JoyceMenuTerm::JoyceMenuTerm()");
	m_menuMutex = false;

        m_clrBG.r   = 0x00; m_clrBG.g   = 0x00; m_clrBG.b   = 0xAA;
        m_clrFG.r   = 0x55; m_clrFG.g   = 0xFF; m_clrFG.b   = 0xFF;

	m_popupBacking = NULL;

	m_menuBar = new UiMenuBar(this);
	m_menuBar->add(new UiMenuBarCommand(SDLK_ESCAPE, "ESC=Back to JOYCE", this));
	m_menuBar->add(new UiMenuBarCommand(SDLK_F1,     "f1=Help", this));
	m_menuBar->add(new UiMenuBarCommand(SDLK_F2,     "f2=Disc", this));
	m_menuBar->add(new UiMenuBarCommand(SDLK_F4,     "f4=Printer", this));
	m_menuBar->add(new UiMenuBarCommand(SDLK_F5,     "f5=Reboot", this));
	m_menuBar->add(new UiMenuBarCommand(SDLK_F6,     "f6=Settings", this));
	m_menuBar->add(new UiMenuBarCommand(SDLK_F7,     "f7=Dbg", this));
	m_menuBar->add(new UiMenuBarCommand(SDLK_F8,     "f8=Action", this));
	m_menuBar->add(new UiMenuBarCommand(SDLK_F10,    "f10=Quit", this));
}


JoyceMenuTerm::~JoyceMenuTerm()
{
	if (m_menuBar) delete m_menuBar;
	m_menuBar = NULL;
}

UiEvent JoyceMenuTerm::menuF1(int x, int y)
{
	UiMenu uim(this);
	UiEvent uie;

        x = alignX(x);
        y = alignY(y);

	uim.add(new UiCommand (SDLK_a, "  About JOYCE...   ", this, UIG_SUBMENU));
	uim.add(new UiCommand (SDLK_k, "  Keyboard layout  ", this));
	uim.setX(x);
	uim.setY(y);

	uie = uim.eventLoop();

	if (uie < 0) return uie;
	switch(uim.getKey(uim.getSelected()))
	{
		case SDLK_a: return aboutBox();
		case SDLK_k: 
			drawKeyboard();
		default:
			break;
	}
	return UIE_CONTINUE;
}


UiEvent JoyceMenuTerm::debugMenu(Z80 *R, int x, int y)
{
	UiMenu uim(this);
	UiEvent uie;

	x = alignX(x);
	y = alignY(y);

	uim.add(new UiSetting(SDLK_d, false, "  Debugger  ",  this))
	   .add(new UiCommand(SDLK_c, "  CPU state  ", this, UIG_SUBMENU))
	   .add(new UiCommand(SDLK_r, "  RAM dump   ", this))
	   .add(new UiSeparator(this))
	   .add(new UiCommand(SDLK_ESCAPE, "  EXIT  ", this));
	uim.setX(x);
	uim.setY(y);

	while (1)
	{
		((UiSetting &)uim[0]).setChecked(m_sys->getDebug());
		uie = uim.eventLoop();

		if (uie < 0) return uie;
		switch(uim.getKey(uim.getSelected()))
		{
			case SDLK_d:	m_sys->setDebug(!m_sys->getDebug());
					return UIE_OK;
			
			case SDLK_c:	uie= cpuState(R); 
					if (uie== UIE_QUIT) return uie;
					break;	

			case SDLK_r:
					{
						FILE *fp = fopen("joyce.ram", "wb");
						fwrite(PCWRAM, 16384, 16, fp);
						fclose(fp);
					}	
					uie = report("Bottom 256k dumped as joyce.ram", "  OK");
					if (uie == UIE_QUIT) return uie;
					break;
			default:	return UIE_CONTINUE;
		}
	}
	return UIE_CONTINUE;
}



UiEvent JoyceMenuTerm::actionMenu(Z80 *R, int x, int y)
{
	UiMenu uim(this);
	UiEvent uie;

	x = alignX(x);
	y = alignY(y);

	uim.add(new UiCommand(SDLK_p,        "  Select PCW screen  ", this))
	   .add(new UiSeparator(this))
	   .add(new UiCommand(SDLK_ESCAPE, "  EXIT  ", this));
	uim.setX(x);
	uim.setY(y);

	while (1)
	{
		uie = uim.eventLoop();

		if (uie < 0) return uie;
		switch(uim.getKey(uim.getSelected()))
		{
			case SDLK_p:	m_oldTerm = m_sys->m_defaultTerminal;
					return UIE_OK;
			default:	return UIE_CONTINUE;
		}
	}
	return UIE_CONTINUE;
}

#define MENU_X(ww)  (*m_menuBar)[m_menuBar->getSelected()].getX() \
		 + ((*m_menuBar)[m_menuBar->getSelected()].getWidth() / 2) \
                            - (ww * charW / 2)



void JoyceMenuTerm::mainMenu(Z80 *R)	/* Handle the menu */
{
	UiEvent uie;
	int y, x;
	int inMenu;
	Uint16 charW, charH;

	if (m_menuMutex) return;	/* Prevent recursion */

	m_menuMutex=1;
        m_oldTerm = m_sys->m_activeTerminal;
        SDL_BlitSurface(m_sysVideo->getSurface(), NULL,
                        m_framebuf->getSurface(), NULL);
        m_sys->selectTerminal(this);
	menuDraw();
        SDL_ShowCursor(1);
	SDL_Rect &rc = getSysVideo()->getBounds();

	measureString("A", &charW, &charH);
	
	do
	{
		inMenu = 1;
		uie = m_menuBar->eventLoop();

		if (uie == UIE_QUIT) goodbye(99); /* SDL_QUIT */
		
		y = m_menuBar->getY() + m_menuBar->getHeight();

		switch(m_menuBar->getKey(m_menuBar->getSelected()))
		{
			case SDLK_F8:	/* Actions menu */
			uie = actionMenu(R, MENU_X(21), y);
			if (uie == UIE_QUIT) goodbye(99);
			if (uie == UIE_OK) 
			{
				inMenu = 0; 
			}
			break;	

			case SDLK_F7:	/* D = Debug mode */
			uie = debugMenu(R, MENU_X(15), y);
			if (uie == UIE_OK) inMenu = 0; 
			else if (uie == UIE_QUIT) goodbye(99);
			else menuDraw();
			break;
			
			case SDLK_ESCAPE:	/* ESC */
       			inMenu = 0;
			break;

			case SDLK_F5:	/* Reboot */
			uie = confirm(MENU_X(17), y, "");

			if (uie == UIE_QUIT) goodbye(99);
			if (uie == UIE_OK) 
			{
				m_sys->setResetFlag(); 
				inMenu = 0; 
			}
			menuDraw();
			break;

			case SDLK_F4:	/* Printer */
			if (m_sys->getModel() == PCW9000 || 
                            m_sys->getModel() == PCW9000P)
			     uie = ((JoyceSystem *)m_sys)->m_daisy.control(this);	
			else uie = ((JoyceSystem *)m_sys)->m_matrix.control(this);
			if (uie == UIE_QUIT) goodbye(99);
			menuDraw();
			break;

			case SDLK_F2:	/* Disc */
			x = alignX(MENU_X(20));
			uie = m_sys->m_fdc->discsMenu(x, alignY(y), this);
			if (uie == UIE_QUIT) goodbye(99);
			menuDraw();
			break;

			case SDLK_F1:	/* Help */
			uie = menuF1(MENU_X(19), y);
			if (uie == UIE_QUIT) goodbye(99);
			break;

			case SDLK_F6:	/* Settings */
			uie = m_sys->settings(this, MENU_X(19), y);
			if (uie == UIE_QUIT) goodbye(99);
			menuDraw();
			break;		

			case SDLK_F10:	/* f10 = Exit */
			x = rc.w - 17 * charW;
			uie = confirm(x, y, "");
			if (uie == UIE_OK || uie == UIE_QUIT) goodbye(99);
			menuDraw();
			break;

			default: uie = UIE_CONTINUE; break;
		}	/* switch */
		if (uie == UIE_QUIT) goodbye(99);	// Quit event 
	} while(inMenu);

	// Cleanup
	m_sys->selectTerminal(m_oldTerm);
        m_menuMutex=0;

}


