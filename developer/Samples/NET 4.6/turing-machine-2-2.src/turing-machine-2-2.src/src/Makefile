###############
# GNU Make 3.80
###############

#################
MAKEFILE      = Makefile
RM            = rm -f
CP            = cp
CXX           = g++
LD            = $(CXX)
COMPFLAGS     = -I.
LDFLAGS       = 
DEPENDFLAGS   = -MM
LIBS          =
#OTHER         = -Wno-deprecated
#OTHER         =
OTHER         =	-W -Wall


#################
TARGET    = turing.exe

OBJS = \
	common.o \
	main.o \
	rules.o \
	run.o \
	service.o \
	tape.o \
	turing-m.o \
	version.o


#################
# Target Rules
#################
.SUFFIXES :
.SUFFIXES : .h .cpp .o
.cpp.o:
	@echo "------------------"
	$(RM) $*.o
#	$(CXX) $(DEPENDFLAGS) $<
	$(CXX) $(COMPFLAGS) $(OPT_LEVEL_FLAG) $(OTHER) -c $<

#################
all:	perfo

help:
	@echo "USAGE : "
	@echo "  TBD"

perfo: 		$(TARGET)

#################
$(TARGET):	$(OBJS) $(LIBS) $(MAKEFILE)
		@echo "=================="
		$(RM) $(TARGET)
		$(LD) -o $(TARGET) $(LDFLAGS) $(OBJS) $(LIBS)
		@echo " "


#################
cleano:	        
	$(RM) $(OBJS)
	$(RM) *.ii
	$(RM) *.s
	$(RM) *.o
	$(RM) core

cleane:
	$(RM) $(TARGET)
	$(RM) *.exe
	$(RM) *.exe*

clean:	cleano cleane

#################

common.o: common.cpp common.h macro.h version.h
main.o: main.cpp run.h turing-m.h rules.h defs.h common.h macro.h \
  version.h tape.h
rules.o: rules.cpp rules.h defs.h common.h macro.h version.h
run.o: run.cpp run.h turing-m.h rules.h defs.h common.h macro.h version.h \
  tape.h
service.o: service.cpp service.h defs.h common.h macro.h version.h
tape.o: tape.cpp tape.h defs.h common.h macro.h version.h
turing-m.o: turing-m.cpp turing-m.h rules.h defs.h common.h macro.h \
  version.h tape.h
version.o: version.cpp version.h

