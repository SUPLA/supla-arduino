COMPONENT_ADD_INCLUDEDIRS := ../../../src \
                             ../../porting/esp-idf
#COMPONENT_OBJS := file1.o file2.o thing/filea.o thing/fileb.o anotherthing/main.o

COMPONENT_SRCDIRS := ../../../src/supla-common \
										 ../../../src/supla \
										 ../../../src/supla/conditions \
										 ../../../src/supla/clock \
										 ../../../src/supla/control \
										 ../../../src/supla/sensor \
										 ../../../src/supla/storage \
										 ../../../src/ \
										 ../../../src/supla/network \
										 ../../porting/esp-idf

CFLAGS += -DSUPLA_DEVICE
CPPFLAGS += -DSUPLA_DEVICE

COMPONENT_EMBED_TXTFILES := supla_org_cert.pem
