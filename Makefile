#$L$
# Copyright (C) 2013 Ridgerun (http://www.ridgerun.com). 
##$L$

# We don't have to fetch the source code, is local
FETCHER_NO_DOWNLOAD=yes

# Vala code needs to build on his own src dir
AUTOTOOLS_BUILD_DIR=src
# Extra flags for gst apps
AUTOTOOLS_PARAMS = LDFLAGS="-Wl,--rpath-link -Wl,$(FSDEVROOT)/usr/lib:$(FSDEVROOT)/lib" --sysconfdir=$(FSDEVROOT)/etc

# Name of the binary to install on the final fs
BINARIES = /usr/bin/video_capture_application

include ../../bsp/classes/rrsdk.class
include $(CLASSES)/autotools.class

hostclean: clean
	$(V)( rm -f $(FETCHED_FLAG) )
	$(V)( cd src ; rm -f aclocal.m4 AUTHORS config.* ChangeLog configure COPYING depcomp INSTALL install-sh ltmain.sh )
	$(V)( cd src ; rm -f libtool.* Makefile Makefile.in missing NEWS *~ ; rm -rf autom4te.cache m4 )
	$(V)( cd src/src ; rm -f Makefile Makefile.in *.o *.lo *.la ; rm -rf .libs .deps )
