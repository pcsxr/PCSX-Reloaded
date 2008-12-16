CFG = cfgDFSound

CFG_OBJECTS = main.o 
#LIBS += `pkg-config libglade-2.0 gtk+-2.0 --libs`
#CFLAGS += `pkg-config libglade-2.0 gtk+-2.0 --cflags` -I..
LIBS += $(shell pkg-config libglade-2.0 gtk+-2.0 --libs)
CFLAGS += $(shell pkg-config libglade-2.0 gtk+-2.0 --cflags) -I.. -DDATADIR=\"/usr/share/psemu/\"

CC = gcc
STRIP = strip
RM = rm -f

all: cfg

cfg:	${CFG_OBJECTS}
	${RM} ${CFG}
	${CC} $(shell pkg-config gtk+-2.0 libglade-2.0 --cflags --libs) ${CFLAGS} ${CFG_OBJECTS} -o ${CFG} ${LIBS}
	${STRIP} ${CFG}

clean:
	${RM} *.o ${CFG}

