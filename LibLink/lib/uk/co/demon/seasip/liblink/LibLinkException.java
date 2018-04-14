/* liblocolink: Simulate a LibLink interface using shared memory

    Copyright (C) 2002  John Elliott <seasip.webmaster@gmail.com>

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
*/

package uk.co.demon.seasip.liblink;

/** A LibLinkException is thrown if any LibLink operation fails.
 * @author John Elliott
 * @version 1.0 
 */
public class LibLinkException extends Exception
{
	int error;

	LibLinkException(String message)
	{
		super(message);
		this.error = UNKNOWN;
	}

	LibLinkException(String message, int err)
	{
		super(message);
		this.error = err;
	}

	/** Get the numerical error code returned by the library.
	 * @see #OK
	 */
	public int getError()
	{
		return error;
	}

/* LibLink errors */
	/** No error. */
	public static final int OK      = 0;
	/** Unknown error. */
	public static final int UNKNOWN = -1;
	/** Out of memory. */
	public static final int NOMEM   = -2;
	/** Invalid pointer passed to LibLink library. */
	public static final int BADPTR  = -3;
	/** System call failed. */
	public static final int SYSERR  = -4;
	/** Slave could not create its connector file - it exists. */
	public static final int EXIST   = -5;
	/** The specified file is not a LibLink connector file. */
	public static final int NOMAGIC = -6;
	/** The link is not open. */
	public static final int CLOSED  = -7;
	/** The specified link is already in use. */
	public static final int INUSE   = -8;
	/** Operation timed out. */
	public static final int TIMEOUT = -9;
	/** The packet length is not valid. */
	public static final int BADPKT  = -10;
	/** CRC error in received packet. */
	public static final int CRC     = -11;
	/** Unexpected value received. */
	public static final int UNEXPECT= -12;
}



