/* liblink: Simulate a LibLink interface using shared memory

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

import java.io.File;

/** The LibLink object encapsulates all link types.
 * @author John Elliott
 * @version 1.0 
 */
public class LibLink
{
	private int id;

/* Control lines */
/* Parallel port outgoing :*/
	public static final int LL_CTL_STROBE     = 1;     /* STROBE */
	public static final int LL_CTL_AUFEED     = 2;     /* AUTO FEED XT */
	public static final int LL_CTL_OSELECT    = 4;     /* SELECT */
	public static final int LL_CTL_INIT       = 8;     /* INIT */
/* Parallel port incoming: */
	public static final int LL_CTL_BUSY     = 256;     /* BUSY */
	public static final int LL_CTL_ACK      = 512;     /* ACK */
	public static final int LL_CTL_NOPAPER = 1024;     /* NO PAPER */
	public static final int LL_CTL_ISELECT = 2048;     /* SELECT */
	public static final int LL_CTL_ERROR   = 4096;     /* ERROR */
/* Serial port outgoing */
	public static final int LL_CTL_DTR    = 65536;  
	public static final int LL_CTL_RTS   = 131072; 
/* Serial port incoming */
	public static final int LL_CTL_CTS = 16777216;
	public static final int LL_CTL_DSR = 33554432;


        static
        {
                try
                {
                        System.loadLibrary("link");
                }
                catch (UnsatisfiedLinkError e)
                {
                        System.err.println("Could not load LibLink dynamic library");
                        System.exit(1);
                }
        }


	private LibLink(int i)
	{
		this.id = i;
	}

	protected void finalize()
	{
		try
		{
			if (id != 0) close();
		}
		catch (LibLinkException e)
		{

		}
	}


	/** Open a LibLink connection as the master.
	 * @param filename The name of the connector file to use.
	 * @return a new LibLink connection object.
	 * @exception LibLinkException if the connection could not be opened.
	 */
	public static native LibLink open(String filename, String devType) throws LibLinkException;

	/** Read the last byte that the other end wrote. 
	 * @return The relevant byte.
	 * @exception LibLinkException if the byte could not be read. */
	public native byte recvPoll() throws LibLinkException;

	/** Wait till the value read changes, and return that.
	 * @return The new value.
	 * @exception LibLinkException if the byte could not be read. */
	public native byte recvWait() throws LibLinkException;

	/** Send a byte to the other end. This will replace any 
	 * existing byte; LibLink does not use byte streams.
	 * @param b The byte to write.
	 * @exception LibLinkException if the byte could not be written.
         */
	public native void send(byte b) throws LibLinkException;

	/** Read the last byte that the other end wrote. 
	 * @return The relevant byte.
	 * @exception LibLinkException if the byte could not be read. */
	public native int readControlPoll() throws LibLinkException;

	/** Wait till the value read changes, and return that.
	 * @return The new value.
	 * @exception LibLinkException if the byte could not be read. */
	public native int readControlWait() throws LibLinkException;

	/** Send a byte to the other end. This will replace any 
	 * existing byte; LibLink does not use byte streams.
	 * @param b The byte to write.
	 * @exception LibLinkException if the byte could not be written.
         */
	public native void setControl(int b) throws LibLinkException;

	/** Close the connection.
	 * If this is a slave, the connector file will be deleted.
	/** Close the connection.
	 * If this is a slave, the connector file will be deleted.
	 * @exception LibLinkException if the connection could not be closed.
	 */
	public native void close() throws LibLinkException;			
	
	/** Open a LibLink connection as the master.
	 * @param filename A reference to the connector file to use.
	 * @return a new LibLink connection object.
	 * @exception LibLinkException if the connection could not be opened.
	 */
	public static LibLink open(File filename, String devType) throws LibLinkException
	{
		return open(filename.getAbsolutePath(), devType);
	}


	/** Return the version string from the underlying LibLink library. */
	public static native String getVersion();

	/** Send a packet down the link.
	 * @param bytes The packet to send. 
	 *  If the other end fails, this can hang. You need to implement 
	 *  a timeout system.
	 * @exception LibLinkException if the packet was not sent. */
	//public native void sendPacket(byte[] bytes) throws LibLinkException;

	/** Receive a packet from the link.
	 *  If the other end fails, this can hang. You need to implement 
	 *  a timeout system.
	 *  @return The packet that was read. 
	 *  @exception LibLinkException if the read failed. */
	//public native byte[] receivePacket() throws LibLinkException;
}



