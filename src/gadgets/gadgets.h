
/*
 * Description: Internal gadgets declarations.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Dec 23 14:56:58 2017
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

#ifndef _LIBXANTE_GADGETS_H
#define _LIBXANTE_GADGETS_H          1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <gadgets.h> directly; include <libxante.h> instead."
# endif
#endif

/*
 * Important: Internal library gadgets only must have the 'gadget_' prefix in
 *            the function name.
 *
 *            Exported gadgets, the ones accessible inside modules, must have
 *            the 'xante_dlg_' prefix.
 */

/* dtable */
void gadget_dispatch_init(void);
void gadget_dispatch_uninit(void);
int gadget_dispatch_call(const char *function, struct xante_app *xpp,
                         struct xante_item *item, void *data);

void gadget_dispatch_add(const char *function,
                         int (*symbol)(struct xante_app *, struct xante_item *,
                                       void *));

/* question */
bool gadget_question(struct xante_app *xpp, const char *title, const char *msg,
                     const char *button1_label, const char *button2_label,
                     const char *statusbar_text);

#endif

