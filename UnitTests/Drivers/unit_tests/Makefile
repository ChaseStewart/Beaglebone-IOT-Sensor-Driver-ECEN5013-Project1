CC = cc
CFLAGS = -std=c99 -g -O0 -pthread -lrt
SELECT = CB
#
# build third-party libraries
#
SUBDIRS = 3rd-party

ifdef SUBDIRS
.PHONY : $(SUBDIRS)
$(SUBDIRS) : 
	@if [ -d $@ ]; then \
		$(MAKE) --no-print-directory --directory=$@ \
			CC=$(CC) CFLAGS="$(CFLAGS)" $(MAKECMDGOALS); \
	fi	
endif


THIRD_PARTY_DIR = 3rd-party
CMOCKA_INCLUDE_DIR = $(THIRD_PARTY_DIR)/build-Debug/include
CMOCKA_LIBRARY = $(THIRD_PARTY_DIR)/build-Debug/lib/libcmocka.a

cmocka : $(SUBDIRS)

CFLAGS += -I $(CMOCKA_INCLUDE_DIR)

#
# quick and dirty build rules to demonstrate testing, not good build
# system practices.
#

#
# library
#

ifeq ($(SELECT), TEMP)
LIB_SRCS = \
	temp_driver.c \
	light_driver.c
else ifeq ($(SELECT), LIGHT)
LIB_SRCS = \
	 light_driver.c
else
LIB_SRCS = \
	
endif
LIB_OBJS = $(LIB_SRCS:%.c=%.o)

LIB_HEADERS = $(LIB_SRCS:%.c=%.h)

LIB = libutils.a

#
# main executable
#
EXE_SRCS = main.c
EXE_OBJS = $(EXE_SRCS:%.c=%.o)

EXE = main.out

#
# tests
#
ifeq ($(SELECT), TEMP)
TEST_QUADRATIC_SRCS = \
	test_temp.c
else ifeq ($(SELECT), LIGHT)
TEST_QUADRATIC_SRCS = \
	test_light.c
else
TEST_QUADRATIC_SRCS = \
	
endif

TEST_QUADRATIC_OBJS = $(TEST_QUADRATIC_SRCS:%.c=%.o)

TEST_QUADRATIC_EXE = test.out


#
# Generic rule to generate objects
#
%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(LIB) : $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $^

all : $(LIB) $(EXE)

$(EXE) : $(EXE_OBJS) $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lm

test : $(TEST_QUADRATIC_EXE)
	./$(TEST_QUADRATIC_EXE)

$(TEST_QUADRATIC_EXE) : $(TEST_QUADRATIC_SRCS) $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ $(CMOCKA_LIBRARY) -lm


.PHONY : clean
clean : FORCE
	-rm -rf *~ *.o $(LIB) $(EXE) $(TEST_QUADRATIC_EXE) *.dSYM/

.PHONY : clobber
clobber : $(SUBDIRS) clean


FORCE :
