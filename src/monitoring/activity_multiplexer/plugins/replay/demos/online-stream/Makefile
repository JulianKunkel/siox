CC=gcc

SOURCE=program.c
TARGET=program

all:
	${CC} -o ${TARGET} ${SOURCE}
	${CC} -o startup_dummy startup_dummy.c


clean:
	rm -f test.tmp
	rm -f ${TARGET}
	rm -f startup_dummy
	rm -rf mytrace
