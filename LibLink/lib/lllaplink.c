/* liblink: Simulate parallel / serial hardware for emulators

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

#include "ll.h"

/* Set LapLink mode. This emulates the effect of a LapLink cable 
 *  * between two devices. */
ll_error_t ll_set_cable(LL_PDEV self, LL_CABLE cable) 
{
	if (!self || !self->lld_clazz) return LL_E_BADPTR;
	
	if (self->lld_clazz->llv_set_cable)
		return (*self->lld_clazz->llv_set_cable)(self, cable);

	self->lld_cable = cable;
	return LL_E_OK;
}

/* Get LapLink setting */
ll_error_t ll_get_cable(LL_PDEV self, LL_CABLE *cable)
{
	if (!self || !self->lld_clazz || !cable) return LL_E_BADPTR;
	
	if (self->lld_clazz->llv_get_cable)
		return (*self->lld_clazz->llv_get_cable)(self, cable);

	*cable = self->lld_cable;
	return LL_E_OK;
}



