#LIB_DIR = /media/daniela/Shared/Devel/lib/

LIBZIP_DIR = $(LIB_DIR)libzip-1.1.3
TETGEN_DIR = $(LIB_DIR)tetgen1.5.0
TINYXML2_DIR = $(LIB_DIR)tinyxml2
#ZLIB_DIR = $(LIB_DIR)


CC= g++
RM= rm
TAR= tar

FLAGS = -std=c++11 -DIS64BITPLATFORM -DTETLIBRARY
CFLAGS = -Wall -fpermissive -I./ -I$(LIB_DIR) -I$(CAXLIB_DIR) -I$(TETGEN_DIR) -I$(TINYXML2_DIR) -I$(LIBZIP_DIR)/build/include -I$(LIBZIP_DIR)/lib# -I$(ZLIB_DIR)

LIBS += -L$(TETGEN_DIR)/build -ltet
LIBS += -ltinyxml2 -lz -L$(LIBZIP_DIR)/build/lib -lzip

SOURCES.C    = main.cpp

INCLUDES =

OBJECTS  =  $(SOURCES.C:.C=.o)

EXECUTABLES  =  ./build/support_structures

#----------


%.o:	%.C

	$(CC) $(FLAGS) $(CFLAGS) -c -o $@ $<



$(EXECUTABLES): $(OBJECTS)

	mkdir -p ./build
	$(CC) $(FLAGS) $(CFLAGS) $(OBJECTS) -o $(EXECUTABLES) $(LIBS)



clean :
	$(RM) -f *.o


backup :
	$(RM) -f backup.tgz
	$(TAR) zcfv backup.tgz $(SOURCES.C) $(INCLUDES) Makefile
