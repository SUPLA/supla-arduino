COMPONENT_ADD_INCLUDEDIRS := ../../../src
#COMPONENT_OBJS := file1.o file2.o thing/filea.o thing/fileb.o anotherthing/main.o

COMPONENT_SRCDIRS := ../../../src/supla-common ../../../src/supla

CFLAGS += -DSUPLA_DEVICE
CPPFLAGS += -DSUPLA_DEVICE
