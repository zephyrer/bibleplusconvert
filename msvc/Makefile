CFLAGS = /nologo /I . /I .. /I ../utils 
LFLAGS = /nologo

all:byteTest.exe dictTest.exe \
	pdbdecoder.exe pdbdictTest.exe pdbinfoTest.exe \
	cacheTest.exe \
	bible.lib

#--executive-----------------------------------------------
BYTE_SRCS = byteTest.obj byteUtils.obj
byteTest.exe:$(BYTE_SRCS)
	link $(BYTE_SRCS) \
		$(LFLAGS) \
		/OUT:byteTest.exe 

DICT_SRCS = dictTest.obj dictUtils.obj
dictTest.exe:$(DICT_SRCS)
	link $(DICT_SRCS) \
		$(LFLAGS) \
		/OUT:dictTest.exe

PDBDEC_SRCS= decodeTest.obj \
			 pdbdecoder.obj memcat.obj byteUtils.obj \
			 pdbhelper.obj pdbcommon.obj dictUtils.obj
pdbdecoder.exe:$(PDBDEC_SRCS)
	link $(PDBDEC_SRCS) \
		$(LFLAGS) \
		/OUT:pdbdecoder.exe

PDBDICT_SRCS= pdbdictTest.obj \
			 pdbdecoder.obj memcat.obj byteUtils.obj \
			 pdbhelper.obj pdbcommon.obj dictUtils.obj
pdbdictTest.exe:$(PDBDICT_SRCS)
	link $(PDBDICT_SRCS) \
		$(LFLAGS) \
		/OUT:pdbdictTest.exe

PDBINFO_SRCS= pdbinfoTest.obj \
			 pdbdecoder.obj memcat.obj byteUtils.obj \
			 pdbhelper.obj pdbcommon.obj dictUtils.obj
pdbinfoTest.exe:$(PDBINFO_SRCS)
	link $(PDBINFO_SRCS) \
		$(LFLAGS) \
		/OUT:pdbinfoTest.exe

CACHE_SRCS= cacheTest.obj \
			 memcacher.obj
cacheTest.exe:$(CACHE_SRCS)
	link $(CACHE_SRCS) \
		$(LFLAGS) \
		/OUT:cacheTest.exe

bible.lib:pdbcommon.obj pdbdecoder.obj pdbhelper.obj \
	dictUtils.obj byteUtils.obj memcacher.obj
	lib /nologo pdbcommon.obj pdbdecoder.obj pdbhelper.obj \
		dictUtils.obj byteUtils.obj memcacher.obj \
		/OUT:bible.lib

#--object--------------------------------------------------
byteTest.obj:../unitTest/byteTest.c
	cl ../unitTest/byteTest.c \
		$(CFLAGS) /c

dictTest.obj:../unitTest/dictTest.c
	cl ../unitTest/dictTest.c \
		$(CFLAGS) /c

decodeTest.obj:../unitTest/decodeTest.c
	cl ../unitTest/decodeTest.c \
		$(CFLAGS) /c

pdbdictTest.obj:../unitTest/pdbdictTest.c 
	cl ../unitTest/pdbdictTest.c \
		$(CFLAGS) /c

pdbinfoTest.obj:../unitTest/pdbinfoTest.c
	cl ../unitTest/pdbinfoTest.c \
		$(CFLAGS) /c

cacheTest.obj:../unitTest/cacheTest.c
	cl ../unitTest/cacheTest.c \
		$(CFLAGS) /c

memcat.obj:../utils/memcat.c
	cl ../utils/memcat.c \
		$(CFLAGS) /c

byteUtils.obj:../utils/byteUtils.c
	cl ../utils/byteUtils.c \
		$(CFLAGS) /c

dictUtils.obj:../utils/dictUtils.c
	cl ../utils/dictUtils.c \
		$(CFLAGS) /c

pdbdecoder.obj:../utils/pdbdecoder.c
	cl ../utils/pdbdecoder.c \
		$(CFLAGS) /c

pdbhelper.obj:../utils/pdbhelper.c
	cl ../utils/pdbhelper.c \
		$(CFLAGS) /c

pdbcommon.obj:../utils/pdbcommon.c
	cl ../utils/pdbcommon.c \
		$(CFLAGS) /c

memcacher.obj:../utils/memcacher.c
	cl ../utils/memcacher.c \
		$(CFLAGS) /c

#-/clean---------------------------------------------------
clean:
	rm -rf *.obj

cleanall:clean
	rm -rf *.exe

