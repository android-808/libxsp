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

#include <stdio.h>

#define NEED_REPLIES
#include <X11/Xlibint.h>
#include <X11/extensions/Xext.h>
#include <X11/extensions/extutil.h>

#include <X11/extensions/xspproto.h>
#include <X11/extensions/xspwire.h>
#include "Xsp.h"



#define XSPCheckExtension(dpy,i,val) \
  XextCheckExtension (dpy, i, xsp_extension_name, val)
#define XSPSimpleCheckExtension(dpy,i) \
    XextSimpleCheckExtension (dpy, i, xsp_extension_name)

static XExtensionInfo _xsp_info_data;
static XExtensionInfo *xsp_info = &_xsp_info_data;
static /* const */ char *xsp_extension_name = XSP_NAME;

static int XSPCloseDisplay();
static Bool XSPWireToEvent(Display *dpy, XEvent *event, xEvent *wire);

static Status XSPEventToWire(Display *dpy, XEvent *event, xEvent *wire);


static /* const */ XExtensionHooks xsp_extension_hooks =
{
    NULL,                               /* create_gc */
    NULL,                               /* copy_gc */
    NULL,                               /* flush_gc */
    NULL,                               /* free_gc */
    NULL,                               /* create_font */
    NULL,                               /* free_font */
    XSPCloseDisplay,                    /* close_display */
    XSPWireToEvent,                     /* wire_to_event */
    XSPEventToWire,                     /* event_to_wire */
    NULL,                               /* error */
    NULL                                /* error_string */
};

static XEXT_GENERATE_FIND_DISPLAY (XSPFindDisplay, xsp_info,
        xsp_extension_name,
        &xsp_extension_hooks, XSPNumberEvents,
        NULL)

static XEXT_GENERATE_CLOSE_DISPLAY (XSPCloseDisplay, xsp_info)


#define SPHasExtension(i) ((i) && ((i)->codes))

    Bool
XSPQueryExtension (dpy, event_basep, error_basep, majorp, minorp)
    Display *dpy;
    int *event_basep, *error_basep;
    int *majorp, *minorp;
{
    XExtDisplayInfo *info = XSPFindDisplay (dpy);
    register xXSPQueryVersionReq *req;
    xXSPQueryVersionReply rep;

    if (!SPHasExtension(info))
        return False;

    LockDisplay(dpy);
    GetReq(XSPQueryVersion, req);
    req->reqType = info->codes->major_opcode;
    req->xspReqType = X_XSPQueryVersion;
    req->majorVersion = XSP_VERSION_MAJOR;
    req->minorVersion = XSP_VERSION_MINOR;
    if (!_XReply(dpy, (xReply *)&rep, 0, xFalse)) {
        UnlockDisplay(dpy);
        SyncHandle();
        return False;
    }
    UnlockDisplay(dpy);
    SyncHandle();
    *event_basep = info->codes->first_event;
    *error_basep = info->codes->first_error;
    *majorp = rep.majorVersion;
    *minorp = rep.minorVersion;
    return True;
}

static Bool XSPWireToEvent(Display *dpy, XEvent *event, xEvent *wire)
{
    XExtDisplayInfo *info = XSPFindDisplay(dpy);

    if (!SPHasExtension(info))
        return False;

    XSPCheckExtension(dpy, info, False);

    switch ((wire->u.u.type & 0x7F) - info->codes->first_event)
    {
        case X_XSPTSRaw:
            {
                XSPRawTouchscreenEvent *aevent;
                xXSPRawTouchscreenEvent *awire;
                awire = (xXSPRawTouchscreenEvent *) wire;
                aevent = (XSPRawTouchscreenEvent *) event;
                aevent->type = awire->type & 0x7F;
                aevent->serial = _XSetLastRequestRead(dpy,
                        (xGenericReply *) wire);
                aevent->send_event = (awire->type & 0x80) != 0;
                aevent->display = dpy;
                aevent->x = awire->x;
                aevent->y = awire->y;
                aevent->pressure = awire->pressure;
                return True;
            }
        case X_XSPDSPStopped:
            {
                XSPDSPStoppedEvent *aevent;
                xXSPDSPStoppedEvent *awire;
                awire = (xXSPDSPStoppedEvent *)wire;
                aevent = (XSPDSPStoppedEvent *)event;
                aevent->type = awire->type & 0x7F;
                aevent->serial = _XSetLastRequestRead(dpy, (xGenericReply *)wire);
                aevent->send_event = (awire->type & 0x80) != 0;
                aevent->display = dpy;
                return True;
            }
    }
    return False;
}

static Status XSPEventToWire(Display *dpy, XEvent *event, xEvent *wire)
{
    XExtDisplayInfo *info = XSPFindDisplay(dpy);

    if (!SPHasExtension(info))
        return False;

    XSPCheckExtension(dpy, info, False);

    switch ((event->type & 0x7F) - info->codes->first_event)
    {
        case X_XSPTSRaw:
            {
                XSPRawTouchscreenEvent *aevent;
                xXSPRawTouchscreenEvent *awire;
                awire = (xXSPRawTouchscreenEvent *) wire;
                aevent = (XSPRawTouchscreenEvent *) event;
                awire->type = aevent->type | (aevent->send_event ? 0x80 : 0);
                awire->x = aevent->x;
                awire->y = aevent->y;
                awire->pressure = aevent->pressure;
                return True;
            }
        case X_XSPDSPStopped:
            {
                XSPDSPStoppedEvent *aevent;
                xXSPDSPStoppedEvent *awire;
                awire = (xXSPDSPStoppedEvent *)wire;
                aevent = (XSPDSPStoppedEvent *)event;
                awire->type = aevent->type | (aevent->send_event ? 0x80 : 0);
                return True;
            }

    }
    return False;
}


Status XSPSetTSRawMode (Display *dpy, Bool enable)
{
    XExtDisplayInfo		*info = XSPFindDisplay (dpy);
    xXSPSetTSRawModeReq	*req;
    xXSPSetTSRawModeReply	rep;

    if (!SPHasExtension(info))
        return False;
    
    LockDisplay (dpy);
    GetReq (XSPSetTSRawMode, req);
    req->reqType = info->codes->major_opcode;
    req->xspReqType = X_XSPSetTSRawMode;
    req->on = enable;
    if (!_XReply (dpy, (xReply *) &rep, 0, xFalse)) 
    {
        UnlockDisplay (dpy);
        SyncHandle ();
        return False;
    }

    UnlockDisplay (dpy);
    SyncHandle ();
    return rep.status;
}


Status XSPSetTSCalibration(Display *dpy,
                int calconst1,
                int calconst2,
                int calconst3,
                int calconst4,
                int calconst5,
                int calconst6,
                int calconst7)
{
    XExtDisplayInfo *info = XSPFindDisplay(dpy);
    xXSPSetTSCalibrationReq *req;
    xXSPSetTSRawModeReply rep;

    if (!SPHasExtension(info))
        return False;
    
    LockDisplay(dpy);
    GetReq(XSPSetTSCalibration, req);
    req->reqType = info->codes->major_opcode;
    req->xspReqType = X_XSPSetTSCalibration;
    req->calconst1 = calconst1;
    req->calconst2 = calconst2;
    req->calconst3 = calconst3;
    req->calconst4 = calconst4;
    req->calconst5 = calconst5;
    req->calconst6 = calconst6;
    req->calconst7 = calconst7;
    if (!_XReply(dpy, (xReply *)&rep, 0, xFalse))
    {
        UnlockDisplay(dpy);
        SyncHandle();
        return False;
    }
    UnlockDisplay(dpy);
    SyncHandle();
    return rep.status;
}

Status XSPRegisterDSPArea(Display *dpy, int screen_num, int x, int y, int width, int height)
{
    XExtDisplayInfo *info = XSPFindDisplay(dpy);
    xXSPRegisterDSPAreaReq *req;
    xXSPRegisterDSPAreaReply rep;

    if (!SPHasExtension(info))
        return False;
    
    LockDisplay(dpy);
    GetReq(XSPRegisterDSPArea, req);
    req->reqType = info->codes->major_opcode;
    req->xspReqType = X_XSPRegisterDSPArea;
    req->screen_num = screen_num;
    req->x = x;
    req->y = y;
    req->width = width;
    req->height = height;

    if (!_XReply(dpy, (xReply *)&rep, 0, xFalse))
    {
        UnlockDisplay(dpy);
        SyncHandle();
        return False;
    }
    UnlockDisplay(dpy);
    SyncHandle();
    return rep.status;

}

Status XSPCancelDSPArea(Display *dpy, int screen_num)
{
    XExtDisplayInfo *info = XSPFindDisplay(dpy);
    xXSPCancelDSPAreaReq *req;
    xXSPCancelDSPAreaReply rep;

    if (!SPHasExtension(info))
        return False;
    
    GetReq(XSPCancelDSPArea, req);
    req->reqType = info->codes->major_opcode;
    req->xspReqType = X_XSPCancelDSPArea;

    req->screen_num = screen_num;

    if (!_XReply(dpy, (xReply *)&rep, 0, xFalse))
    {
        SyncHandle();
        return False;
    }
    SyncHandle();
    return rep.status;

}


Status XSPSetPixelDoubling(Display *dpy, int screen_num, int state)
{
    XExtDisplayInfo *info = XSPFindDisplay(dpy);
    xXSPSetPixelDoublingReq *req;
    xXSPSetPixelDoublingReply rep;

    if (!SPHasExtension(info))
        return False;
    
    GetReq(XSPSetPixelDoubling, req);
    req->reqType = info->codes->major_opcode;
    req->xspReqType = X_XSPSetPixelDoubling;

    req->screen_num = screen_num;
    req->state = state;

    if (!_XReply(dpy, (xReply *)&rep, 0, xFalse))
    {
        SyncHandle();
        return False;
    }
    SyncHandle();
    return rep.status;
}


Status XSPSetThumbPressure(Display *dpy, int pressure)
{
    XExtDisplayInfo *info = XSPFindDisplay(dpy);
    xXSPSetThumbPressureReq *req;

    if (!SPHasExtension(info))
        return False;
    
    GetReq(XSPSetThumbPressure, req);
    req->reqType = info->codes->major_opcode;
    req->xspReqType = X_XSPSetThumbPressure;

    req->pressure = pressure;

    SyncHandle();
    return True;
}


