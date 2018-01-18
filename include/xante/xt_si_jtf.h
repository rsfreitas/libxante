
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Nov 21 19:26:16 2017
 * Project: libxante
 *
 * Copyright (C) 2017 Rodrigo Freitas
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#ifndef _LIBXANTE_XT_SI_JTF_H
#define _LIBXANTE_XT_SI_JTF_H

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <xt_si_jtf.h> directly; include <libxante.h> instead."
# endif
#else

/* Internal library declarations */
struct xante_single_instance_jtf *si_jtf_load(const char *si);
void si_jtf_unload(struct xante_single_instance_jtf *si);

#endif

#endif

