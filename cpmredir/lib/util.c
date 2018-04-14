/*

    CPMREDIR: CP/M filesystem redirector
    Copyright (C) 1998, John Elliott <jce@seasip.demon.co.uk>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    This file holds miscellaneous utility functions.
*/

#include "cpmint.h"

/* In debug mode, lseek()s can be traced. */

#ifdef DEBUG

long zxlseek(int fd, long offset, int wh)
{
	long v = lseek(fd, offset, wh);
	if (v >= 0) return v;
	
	redir_Msg("lseek fails with errno = %d\n", errno);
	if (errno == EBADF)  redir_Msg("     (bad file descriptor %d)\n", fd);
	if (errno == ESPIPE) redir_Msg("     (file %d is a pipe)\n",      fd);
	if (errno == EINVAL) redir_Msg("     (bad parameter %d)\n",       wh);

	return -1;
}


void redir_showfcb(cpm_byte *fd)
{
	int n;

	for (n = 0; n < 32; n++)
	{
		if (!n || n>= 12) printf("%02x ", fd[n]);
		else		  printf("%c", fd[n] & 0x7F);
	}
	printf("\r\n");
}

#endif

/* Get the "sequential access" file pointer out of an FCB */

long redir_get_fcb_pos(cpm_byte *fcb)
{
	long npos;

        npos  = 524288L * fcb[0x0E];      /* S2 */
        npos  += 16384L * fcb[0x0C];      /* Extent */
        npos  += 128L   * fcb[0x20];      /* Record */

	return npos;
}

void redir_put_fcb_pos(cpm_byte *fcb, long npos)
{
        fcb[0x20] = (npos / 128) % 128;
        fcb[0x0C] = (npos / 16384) % 32;
        fcb[0x0E] = (npos / 524288L) % 64;
}




/* Passed a CP/M FCB, convert it to a unix filename. Turn its drive back into
 * a path. */

int redir_fcb2unix(cpm_byte *fcb, char *fname)
{
	int n, q, drv, ddrv;
	char s[2];

	s[1] = 0;
	q    = 0;
	drv  = fcb[0] & 0x7F;
	if (drv == '?') drv = 0;

	ddrv = fcb[0] & 0x7F; 
	if (ddrv < 0x1F) ddrv += '@';

	redir_Msg("%c:%-8.8s.%-3.3s\n",
	    ddrv,
            fcb + 1,
            fcb + 9);

	if (!drv) strcpy(fname, redir_drive_prefix[redir_cpmdrive]);
	else	  strcpy(fname, redir_drive_prefix[drv - 1]);

	for (n = 1; n < 12; n++)
	{	
		s[0] = (fcb[n] & 0x7F);
		if (s[0] == '?') q = 1;
		if (isupper(s[0])) s[0] = tolower(s[0]);
		if (s[0] != ' ')
		{
			if (n == 9) strcat(fname, ".");
			strcat(fname, s);
		}
	}
	return q;
}

#ifndef EROFS	/* Open fails because of read-only FS */
#define EROFS EACCES
#endif

int redir_ofile(cpm_byte *fcb, char *s)
{
	int h, rv;

       /* Software write-protection */
#ifdef __MSDOS__
	if (!redir_ro_fcb(fcb)) 
	{
		rv = _dos_open(s, O_RDWR, &h);
		if (!rv) return h;
		redir_Msg("Open of %s fails: error %x\r\n", s, rv);
	}
	rv = _dos_open(s, O_RDONLY, &h);
	if (rv) return -1;
        fcb[9] |= 0x80;
#else
	(void)rv;	/* Stop compiler warning */
	if (!redir_ro_fcb(fcb)) 
	{
		h = open(s, O_RDWR | O_BINARY);
		if (h >= 0 || (errno != EACCES && errno != EROFS)) return h;
	}
	h = open(s, O_RDONLY | O_BINARY);
	if (h < 0) return -1;
        fcb[9] |= 0x80;
#endif
	return h;
}


/* Extract a file handle from where it was stored in an FCB by fcb_open()
  or fcb_creat(). Aborts if the FCB has been tampered with. 

  Note: Some programs (like GENCOM) close FCBs they never opened. This causes
  the Corrupt FCB message, but no harm seems to ensue. */

int redir_verify_fcb(cpm_byte *fcb)
{
        if (fcb[16] != 0xFD || fcb[17] != 0x00)
        {
                fprintf(stderr,"cpmredir: Corrupt FCB\n");
		return -1; 
	}
        return (int)(redir_rd32(fcb + 18));

}

/* Print a trace message */

#ifdef DEBUG

void redir_Msg(char *s, ...)
{
        va_list ap;

        va_start(ap, s);
        printf("cpmredir trace: ");
        vprintf(s, ap);
        va_end(ap);
        fflush(stdout);
}

#endif

#define BCD(x) (((x % 10)+16*(x/10)) & 0xFF)

/* Convert time_t to CP/M day count/hours/minutes */
dword redir_cpmtime(time_t t)
{
        long d  = (t / 86400) - 2921;  /* CP/M day 0 is unix day 2921 */
        long h  = (t % 86400) / 3600;  /* Hour, 0-23 */
        long m  = (t % 3600)  / 60;    /* Minute, 0-59 */

        return (d | (BCD(h) << 16) | (BCD(m) << 24));
}

#undef BCD

#define UNBCD(x) (((x % 16) + 10 * (x / 16)) & 0xFF)

time_t redir_unixtime(cpm_byte *c)
{
	time_t t;
	cpm_word days;

	days = (c[0] + 256 * c[1]) + 2921;

	t =  60L    * UNBCD(c[3]);
	t += 3600L  * UNBCD(c[2]);
	t += 86400L * days;

	return t;
}

#undef UNBCD


/* Functions to access 24-bit & 32-bit words in memory. These are always
  little-endian. */

void redir_wr24(cpm_byte *addr, dword v)
{
	addr[0] =  v        & 0xFF;
	addr[1] = (v >> 8)  & 0xFF;
	addr[2] = (v >> 16) & 0xFF;
}

void redir_wr32(cpm_byte *addr, dword v)
{
        addr[0] =  v        & 0xFF;
        addr[1] = (v >> 8)  & 0xFF;
        addr[2] = (v >> 16) & 0xFF;
	addr[3] = (v >> 24) & 0xFF;
}

dword redir_rd24(cpm_byte *addr)
{
	register dword rv = addr[2];
	
	rv = (rv << 8) | addr[1];
	rv = (rv << 8) | addr[0];
	return rv;
}


dword redir_rd32(cpm_byte *addr)
{
        register dword rv = addr[3];

	rv = (rv << 8) | addr[2];
        rv = (rv << 8) | addr[1];
        rv = (rv << 8) | addr[0];
        return rv;
}


void redir_log_drv(cpm_byte drv)
{
	if (!drv) redir_l_drives |= 1;
        else redir_l_drives |= (1L << drv);
}

void redir_log_fcb(cpm_byte *fcb)
{
	int drv = fcb[0] & 0x7F;
	
	if (drv && drv != '?') redir_log_drv(drv - 1);
	else redir_log_drv(redir_cpmdrive);
}


int redir_ro_drv(cpm_byte drv)
{
	if (!drv) return redir_ro_drives & 1;
	else	  return redir_ro_drives & (1L << drv);
}

int redir_ro_fcb(cpm_byte *fcb)
{
	int drv = fcb[0] & 0x7F;

	if (drv && drv != '?') return redir_ro_drv(drv - 1);
	else                   return redir_ro_drv(redir_cpmdrive);
}



cpm_word redir_xlt_err(void)
{
	if (redir_password_error()) return 0x7FF;	/* DRDOS pwd error */
	switch(errno)
	{	
		case EISDIR:
		case EBADF:  return 9;		/* Bad FCB */
		case EINVAL: return 0x03FF;	/* Readonly file */
		case EPIPE:  return 0x01FF;	/* Broken pipe */
		case ENOSPC: return 1;		/* No space */
		default:     return 0xFF;	/* Software error */
	}
}
