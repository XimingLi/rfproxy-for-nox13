include ../../../Make.vars 

CONFIGURE_DEPENCIES = $(srcdir)/Makefile.am

EXTRA_DIST = \
	meta.json

if PY_ENABLED
AM_CPPFLAGS += $(PYTHON_CPPFLAGS)
endif # PY_ENABLED 

pkglib_LTLIBRARIES =\
	rfproxy.la

rfproxy_la_CPPFLAGS = $(AM_CPPFLAGS) -I $(top_srcdir)/src/nox -I/usr/include/mongo \
                        -I$(top_srcdir)/src/nox/coreapps/

rfproxy_la_SOURCES = convert.h defs.h IPAddress.h IPAddress.cc \
		      MACAddress.h MACAddress.cc \
		      TLV.hh TLV.cc Action.hh Action.cc \
		      Match.hh Match.cc Option.hh \
 		      Option.cc IPC.h IPC.cc MongoIPC.h \
		      MongoIPC.cc RFProtocol.h RFProtocol.cc \
		      RFProtocolFactory.h RFProtocolFactory.cc \
		      rfproxy.hh rfproxy.cc
rfproxy_la_LIBADD = -lmongoclient -lboost_thread -lboost_filesystem \
		-lboost_program_options -lboost_system
rfproxy_la_LDFLAGS = -module -export-dynamic

#-lrt -lnetlink -lutil \
#-lpthread -lboost_thread -lboost_thread-mt -lboost_system \
#-lboost_filesystem -lboost_program_options -lmongoclient 



NOX_RUNTIMEFILES = meta.json	

all-local: nox-all-local
clean-local: nox-clean-local 
install-exec-hook: nox-install-local
