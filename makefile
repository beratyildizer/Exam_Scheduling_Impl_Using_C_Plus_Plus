##############
# Makefile to build OGLX library
##############

CC = gcc
OBJECTS = termProject.o examScheduling.o
APP_NAME = app

${APP_NAME}: ${OBJECTS}
        ${CC} -o app termProject.o examScheduling.o

termProject.o: termProject.c
        ${CC} -c termProject.c
    
examScheduling.o: examScheduling.c examScheduling.h
        ${CC} -c examScheduling.o

clean:
        rm -f ${OBJECTS}		