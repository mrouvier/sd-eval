CXX=g++ -std=c++11
CFLAGS=-Wall -Wno-unused-parameter
OBJ=Speaker.o Section.o Util.o Turn.o MDTM.o OptAssign.o UEM.o SdResultat.o CrossFile.o Logging.o Error.o


all: sd-eval

sd-eval: sd-eval.cc $(OBJ)
	${CXX} -o sd-eval sd-eval.cc $(OBJ)

.cc.o:
	${CXX} -c ${CFLAGS} $< 

SdResultat.o: SdResultat.h MDTM.h UEM.h CrossFile.h

Speaker.o: Speaker.h Turn.h Util.h

Section.o: Section.h Turn.h

Util.o: Util.h

Turn.o: Turn.h

MDTM.o: MDTM.h Speaker.h Turn.h Section.h Util.h

OptAssign.o: OptAssign.h

UEM.o: UEM.h Util.h

CrossFile.o: CrossFile.h MDTM.h

Logging.o: Logging.h

Error.o: Error.h Logging.h

clean:
	rm *.o
	rm sd-eval
