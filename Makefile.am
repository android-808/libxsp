AM_CFLAGS = -I$(top_srcdir)/include $(XSP_CFLAGS)


lib_LTLIBRARIES = libXsp.la

libXsp_la_SOURCES = Xsp.c

libXsp_la_LIBADD = $(XSP_LIBS)

XSP_CURRENT=1
XSP_REVISION=0
XSP_AGE=1
libXsp_la_LDFLAGS = -version-info ${XSP_CURRENT}:${XSP_REVISION}:${XSP_AGE} -no-undefined

libXspincludedir = $(includedir)/X11/extensions
libXspinclude_HEADERS = Xsp.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = xsp.pc


noinst_PROGRAMS = testxsp

testxsp_SOURCES = testxsp.c
testxsp_LDADD = libXsp.la $(X_LIBS)

MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = xsp.pc.in autogen.sh
