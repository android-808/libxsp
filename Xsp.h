/*
 * Copyright © 2006 Nokia Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the authors not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The authors make no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _XSP_H_
#define _XSP_H_

#include <X11/Xfuncproto.h>
#include <X11/Xlib.h>

typedef struct {
    int type;			/* event base */
    unsigned long serial;
    Bool send_event;
    Display *display;
    int x;
    int y;
    int pressure;
} XSPRawTouchscreenEvent;

typedef struct {
    int type;			/* event base */
    unsigned long serial;
    Bool send_event;
    Display *display;
} XSPDSPStoppedEvent;


_XFUNCPROTOBEGIN

Bool XSPQueryExtension(
    Display* dpy,
    int* event_basep,
    int* error_basep,
    int* majorp,
    int* minorp
);


Status XSPSetTSRawMode(Display *dpy, Bool enable);
Status XSPSetTSCalibration(Display *dpy, 
			int calconst1,
			int calconst2,
			int calconst3,
			int calconst4,
			int calconst5,
			int calconst6,
			int calconst7);

Status XSPRegisterDSPArea(Display *dpy, int screen_num, int x, int y, int width, int height);
Status XSPCancelDSPArea(Display *dpy, int screen_num);
Status XSPSetPixelDoubling(Display *dpy, int screen_num, int state);
Status XSPSetThumbPressure(Display *dpy, int pressure);

_XFUNCPROTOEND


#endif
