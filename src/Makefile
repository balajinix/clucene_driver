PROJECT = clucenedriver
CXX = g++
CXXFLAGS = -Wall -g -Wno-deprecated
ARCHIVE = ar -r

INCLUDE_DIR = -I. -I/usr/include/c++/4.4/ -I/usr/local/include/CLucene/
LIBS = -l${PROJECT}
LIB_DIR = ../lib
BIN_DIR = ../bin
LINK_DIR = -L../lib

CLUCENE_LIB_DIR = -L/usr/local/lib/
CLUCENE_LIBS = -lclucene-core -lclucene-shared

LIB = ${LIB_DIR}/lib${PROJECT}.so

BINS = ${BIN_DIR}/t_clucene_driver

#OBJS = IndexFiles.o SearchFiles.o Statistics.o
OBJS = clucene_driver.o

all: ${LIB} ${BINS}
	@echo "build done!"

lib: ${LIB}
	@echo "lib done!"

clean:
	@echo "cleaning libraries ..."
	@rm *.o ${BINS} ${LIB}
	@echo "done!"

${LIB}: ${OBJS}
	@echo "creating library file ..."
	@${ARCHIVE} ${LIB} ${OBJS}
	@ranlib ${LIB} # this creates an index table
	@echo "done!"

.cc.o:
	@echo "compiling ..." $< "..."
	@${CXX} ${CXXFLAGS} -shared -fPIC -c ${INCLUDE_DIR} $<

${BIN_DIR}/t_clucene_driver: t_clucene_driver.o ${LIB}
	@echo "creating binary:" $@
	@${CXX} ${CXXFLAGS} $< ${LINK_DIR} ${CLUCENE_LIB_DIR} ${LIBS} ${CLUCENE_LIBS} -o ${BIN_DIR}/t_clucene_driver
	@echo "done!"

# DO NOT DELETE THIS LINE
