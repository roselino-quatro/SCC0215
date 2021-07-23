#Name of end binary
PROGNAME = arquivost3

#Compiler and Linker flags and options
CC := gcc
CFLAGS := -Wall -Werror -g

#Folder with all necessary .c files
SRC_F = ./source
C_SRCS := $(wildcard ${SRC_F}/*.c)

# Folder with all necessary .h files
INCLUDE_F = ./includes

#Folder to store the matching .o files
OBJ_F = ./objs
C_OBJS := $(C_SRCS:%.c=%.o)
C_OBJS := $(subst ${SRC_F},${OBJ_F},${C_OBJS})

#Configs for test cases
TEST_F := ./casos
TEST_CASES := $(addsuffix .test, $(basename $(wildcard ${TEST_F}/*.in)))
test_fun = ./${PROGNAME} < $< | diff -qy - $(patsubst %.in,%.out,$<)

#Debugging configs
VAL_FLAGS := --leak-check=full --show-leak-kinds=all --track-origins=yes
VAL_CASE := all

all: ${OBJ_F} ${C_OBJS}
	@echo "Creating ${PROGNAME} binary"
	@${CC} ${C_OBJS} -o ${PROGNAME} ${CFLAGS}

${OBJ_F}:
	@echo "Creating objs folder..."
	@mkdir ${OBJ_F}

${OBJ_F}/%.o: ${SRC_F}/%.c
	@echo $@ $<
	@echo "Creating $@ object"
	@${CC} -I ${INCLUDE_F} ${CFLAGS} -c $< -o $@

run:
	@./$(PROGNAME)

clean:
	@echo "Deleting objs and ${PROGNAME}..."
	@rm -rf ${OBJ_F}/*.o
	@rm -f ${PROGNAME}
	@echo "Done!"

zip:
	@echo "Zipping source and make with optimal compression..."
	@zip -9 -rq ${PROGNAME}.zip ${SRC_F} ${INCLUDE_F} makefile
	@echo "Done!"

test: ${TEST_F} ${TEST_CASES}

${TEST_F}:
	@echo "Creating your testing folder..."
	@mkdir ${OBJ_F}
	@echo "But you're still missing tests"

%.test: %.in %.out
	@echo "Testing $(basename $@)"
	@./${PROGNAME} < $< | diff -sqy - $(patsubst %.in,%.out,$<) || true

valgrind:
	@valgrind ${VAL_FLAGS} ./${PROGNAME}
