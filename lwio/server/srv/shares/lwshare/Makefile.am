noinst_LTLIBRARIES = liblwshare.la

liblwshare_la_SOURCES = \
    dbcontext.c \
    globals.c   \
    libmain.c   \
    sharedb.c

liblwshare_la_CPPFLAGS = \
    -DLW_ENABLE_THREADS=1  \
    -I@top_srcdir@         \
    -I@top_srcdir@/include \
    -I@top_srcdir@/server/include \
    -I@top_srcdir@/server/srv/include \
    -I@top_srcdir@/server/srv/shares/include

liblwshare_la_LIBADD = \
    @SQLITE_LIBS@

liblwshare_la_LDFLAGS = -static

