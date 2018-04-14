/*************************************************************************
**	 Copyright 2005, John Elliott                                   **
**       Copyright 1999, Caldera Thin Clients, Inc.                     **
**       This software is licenced under the GNU Public License.        **
**       Please see LICENSE.TXT for further information.                **
**                                                                      **
**                  Historical Copyright                                **
**                                                                      **
**                                                                      **
**                                                                      **
**  Copyright (c) 1987, Digital Research, Inc. All Rights Reserved.     **
**  The Software Code contained in this listing is proprietary to       **
**  Digital Research, Inc., Monterey, California and is covered by U.S. **
**  and other copyright protection.  Unauthorized copying, adaptation,  **
**  distribution, use or display is prohibited and may be subject to    **
**  civil and criminal penalties.  Disclosure to others is prohibited.  **
**  For the terms and conditions of software code use refer to the      **
**  appropriate Digital Research License Agreement.                     **
**                                                                      **
*************************************************************************/

#include "sdsdl.hxx"
#include <math.h>

Sint32 SMUL_DIV (Sint32 m1, Sint32 m2, Sint32 d1)
{
	long long mr = (m1 * m2);
	long dq = (mr / d1);
	long dr = (mr % d1);
	int tick = (mr < 0) ? -1 : 1;

	if (d1 < 0) { tick = (-tick); d1 = (-d1); }
	if (dr < 0) dr = (-dr);
	if ((2 * dr) > d1) dq += tick;

	return dq;
}


Sint32 vec_len  (Sint32 dx, Sint32 dy)
{
// XXX copout
	double dxd = dx, dyd = dy;
	double l = (dxd * dxd) + (dyd * dyd);

	return (long)sqrt(l);
}
