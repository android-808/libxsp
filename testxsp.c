#include <X11/X.h>
#include <X11/Xlib.h>

#include "Xsp.h"

#include <stdio.h>

int main (int argc, char **argv)
{
	Display *dpy;
	int event_base, error_base;
	int major, minor;

	dpy = XOpenDisplay(NULL);
	if (!XSPQueryExtension(dpy, &event_base, &error_base, &major, &minor))
	{
		fprintf(stderr, "XSP extension not available\n");
		XCloseDisplay(dpy);
		return 1;
	}

	printf("XSP extension found:\n");
	printf("  event_base: %d\n", event_base);
	printf("  error_base: %d\n", error_base);
	printf("       major: %d\n", major);
	printf("       minor: %d\n", minor);
	
	if (!XSPRegisterDSPArea(dpy, DefaultScreen(dpy), 0, 0, 800, 480)) {
		fprintf(stderr, "XSPRegisterDSPArea failed\n");
	}

	if (!XSPCancelDSPArea(dpy, DefaultScreen(dpy))) {
		fprintf(stderr, "XSPCancelDSPArea failed\n");
	}
	XCloseDisplay(dpy);
	return 0;
}
