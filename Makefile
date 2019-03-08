CXX=g++ -std=c++11
CFLAGS=-Wall -Wno-unused-parameter -Wunused-function
OBJ=Speaker.o Section.o Util.o Turn.o MDTM.o OptAssign.o UEM.o SdResultat.o CrossFile.o Logging.o Error.o RTTM.o DiaFormat.o


all: sd-eval

sd-eval: sd-eval.cc $(OBJ)
	${CXX} -o sd-eval sd-eval.cc $(OBJ)

.cc.o:
	${CXX} -c ${CFLAGS} $< 

SdResultat.o: SdResultat.h UEM.h CrossFile.h DiaFormat.h

Speaker.o: Speaker.h Turn.h Util.h

Section.o: Section.h Turn.h

Util.o: Util.h

Turn.o: Turn.h

MDTM.o: MDTM.h Speaker.h Turn.h Section.h Util.h DiaFormat.h

OptAssign.o: OptAssign.h

UEM.o: UEM.h Util.h

CrossFile.o: CrossFile.h DiaFormat.h

Logging.o: Logging.h

Error.o: Error.h Logging.h

RTTM.o: RTTM.h Speaker.h Turn.h Section.h Util.h DiaFormat.h

clean:
	rm *.o
	rm sd-eval
