# Process this file with automake to produce Makefile.in

NULL =

# The following warning options are useful for debugging: -Wpadded -Wcast-align
#AM_CXXFLAGS =

lib_LTLIBRARIES = libharfbuzz.la

HBCFLAGS =
HBLIBS =
HBSOURCES =  \
	hb-blob.c \
	hb-buffer.c \
	hb-buffer-private.h \
	hb-font.cc \
	hb-font-private.h \
	hb-object-private.h \
	hb-open-file-private.hh \
	hb-open-type-private.hh \
	hb-language.c \
	hb-private.h \
	hb-shape.c \
	hb-unicode.c \
	hb-unicode-private.h \
	$(NULL)
HBHEADERS = \
	hb.h \
	hb-blob.h \
	hb-buffer.h \
	hb-common.h \
	hb-font.h \
	hb-language.h \
	hb-shape.h \
	hb-unicode.h \
	$(NULL)

HBSOURCES += \
	hb-ot-layout.cc \
	hb-ot-layout-common-private.hh \
	hb-ot-layout-gdef-private.hh \
	hb-ot-layout-gpos-private.hh \
	hb-ot-layout-gsubgpos-private.hh \
	hb-ot-layout-gsub-private.hh \
	hb-ot-layout-private.h \
	hb-ot-shape.c \
	hb-ot-shape-private.h \
	hb-ot-tag.c \
	$(NULL)
HBHEADERS += \
	hb-ot.h \
	hb-ot-layout.h \
	hb-ot-tag.h \
	$(NULL)

if HAVE_GLIB
HBCFLAGS += $(GLIB_CFLAGS)
HBLIBS   += $(GLIB_LIBS)
HBSOURCES += \
	hb-glib.c \
	$(NULL)
HBHEADERS += \
	hb-glib.h \
	$(NULL)
endif

if HAVE_ICU
HBCFLAGS += $(ICU_CFLAGS)
HBLIBS   += $(ICU_LIBS)
HBSOURCES += \
	hb-icu.c \
	$(NULL)
HBHEADERS += \
	hb-icu.h \
	$(NULL)
endif

if HAVE_FREETYPE
HBCFLAGS += $(FREETYPE_CFLAGS)
HBLIBS   += $(FREETYPE_LIBS)
HBSOURCES += \
	hb-ft.c \
	$(NULL)
HBHEADERS += \
	hb-ft.h \
	$(NULL)
endif

CXXLINK = $(LINK)
libharfbuzz_la_SOURCES = $(HBSOURCES) $(HBHEADERS)
libharfbuzz_la_CPPFLAGS = $(HBCFLAGS)
libharfbuzz_la_LIBADD = $(HBLIBS)
pkginclude_HEADERS = $(HBHEADERS)

noinst_PROGRAMS = main shape

main_SOURCES = main.cc
main_CPPFLAGS = $(HBCFLAGS)
main_LDADD = libharfbuzz.la $(HBLIBS)

shape_SOURCES = shape.c
shape_CPPFLAGS = $(HBCFLAGS)
shape_LDADD = libharfbuzz.la $(HBLIBS)

TESTS =

if HAVE_ICU
else
TESTS += check-libstdc++.sh
endif

-include $(top_srcdir)/git.mk
