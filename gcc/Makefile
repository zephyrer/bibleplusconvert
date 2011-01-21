CFLAGS = -I .. -I ../utils -Wall

all:byteTest.exe dictTest.exe \
	pdbdecoder.exe pdbdictTest.exe pdbinfoTest.exe \
	cacheTest.exe

#--executive-----------------------------------------------
BYTE_SRCS = ../unitTest/byteTest.c byteUtils.o
byteTest.exe:$(BYTE_SRCS)
	gcc $(BYTE_SRCS) \
		$(CFLAGS) \
		-o byteTest.exe 

DICT_SRCS = ../unitTest/dictTest.c dictUtils.o
dictTest.exe:$(DICT_SRCS)
	gcc $(DICT_SRCS) \
		$(CFLAGS) \
		-o dictTest.exe

PDBDEC_SRCS= ../unitTest/decodeTest.c \
			 pdbdecoder.o memcat.o byteUtils.o \
			 pdbhelper.o pdbcommon.o dictUtils.o
pdbdecoder.exe:$(PDBDEC_SRCS)
	gcc $(PDBDEC_SRCS) \
		$(CFLAGS) \
		-o pdbdecoder.exe

PDBDICT_SRCS= ../unitTest/pdbdictTest.c \
			 pdbdecoder.o memcat.o byteUtils.o \
			 pdbhelper.o pdbcommon.o dictUtils.o
pdbdictTest.exe:$(PDBDICT_SRCS)
	gcc $(PDBDICT_SRCS) \
		$(CFLAGS) \
		-o pdbdictTest.exe

PDBINFO_SRCS= ../unitTest/pdbinfoTest.c \
			 pdbdecoder.o memcat.o byteUtils.o \
			 pdbhelper.o pdbcommon.o dictUtils.o
pdbinfoTest.exe:$(PDBINFO_SRCS)
	gcc $(PDBINFO_SRCS) \
		$(CFLAGS) \
		-o pdbinfoTest.exe

CACHE_SRC = ../unitTest/cacheTest.c \
			memcacher.o
cacheTest.exe:$(CACHE_SRC)
	gcc $(CACHE_SRC) \
		$(CFLAGS) \
		-o cacheTest.exe

#--object--------------------------------------------------
memcat.o:../utils/memcat.c
	gcc ../utils/memcat.c \
		$(CFLAGS) -c

byteUtils.o:../utils/byteUtils.c
	gcc ../utils/byteUtils.c \
		$(CFLAGS) -c

dictUtils.o:../utils/dictUtils.c
	gcc ../utils/dictUtils.c \
		$(CFLAGS) -c

pdbdecoder.o:../utils/pdbdecoder.c
	gcc ../utils/pdbdecoder.c \
		$(CFLAGS) -c

pdbhelper.o:../utils/pdbhelper.c
	gcc ../utils/pdbhelper.c \
		$(CFLAGS) -c

pdbcommon.o:../utils/pdbcommon.c
	gcc ../utils/pdbcommon.c \
		$(CFLAGS) -c

memcacher.o:../utils/memcacher.c
	gcc ../utils/memcacher.c \
		$(CFLAGS) -c

#--clean---------------------------------------------------
clean:
	rm -rf *.o

cleanall:clean
	rm -rf *.exe

