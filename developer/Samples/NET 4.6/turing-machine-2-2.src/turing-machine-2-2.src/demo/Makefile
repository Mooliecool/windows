###############
# GNU Make 3.80
###############

#################
MAKEFILE      = Makefile
MAKE          = make
ECHO          = echo
CLEAN         = $(MAKE) clean
RM            = rm -f
CD            = cd
CAT           = cat

SRC           = ../src
EXE_BASENAME  = turing
EXE_NAME      = $(SRC)/$(EXE_BASENAME)
LOG_SUFFIX    = .log


###############
compile:
	$(CD) $(SRC); $(MAKE)

howto:
	$(CAT) $@.inf

info:	
	$(CAT) $@.inf

clean:
	$(CD) $(SRC); $(CLEAN)

cleanlog:
	$(RM) *$(LOG_SUFFIX)



#### Invocation of Help ###
help:
	$(EXE_NAME) -h > $@$(LOG_SUFFIX)



############### Demo Targets ##############
#### Invocation of Demo Turing Machines ###
all:
	$(EXE_NAME) > $@$(LOG_SUFFIX)

add:
	$(EXE_NAME) meta_$@ > $@$(LOG_SUFFIX)

mult:
	$(EXE_NAME) meta_$@ > $@$(LOG_SUFFIX)

aho:
	$(EXE_NAME) meta_$@ > $@$(LOG_SUFFIX)

euclid:
	$(EXE_NAME) meta_$@ > $@$(LOG_SUFFIX)

fib:
	$(EXE_NAME) meta_$@ > $@$(LOG_SUFFIX)

u2b:
	$(EXE_NAME) meta_$@ > $@$(LOG_SUFFIX)

