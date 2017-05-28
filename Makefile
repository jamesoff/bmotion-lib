#
# Compiler: g++
#

PROGRAM := libbmotion.so
VERSION := 0.1

DIRS := . utils system plugin
OTHERS := test plugins
CC := g++
LD := g++

CFLAGS := -c
LDFLAGS := -Wl,-export-dynamic -shared
LIBS := -ldl -lgthread-2.0
LIBDIRS :=
INCLUDE_DIRS := -I/usr/include/glib-2.0/ -I/usr/lib/glib-2.0/include
DEFINES := -DPROGRAM=\"$(PROGRAM)\" -DVERSION=\"$(VERSION)\"

CFLAGS := $(if $(DEBUG)==1, $(CFLAGS) -ggdb, $(CFLAGS)) -Wall -W
LDFLAGS := $(if $(DEBUG)==1, $(LDFLAGS) -ggdb, $(LDFLAGS))

CPP_SOURCE_FILES := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp))
CPP_OBJECT_FILES := $(CPP_SOURCE_FILES:.cpp=.o)
INCLUDE_DIRS += $(foreach dir,$(DIRS),-I$(dir))

default: $(CPP_OBJECT_FILES)
	@echo linking  $(PROGRAM)...; \
	$(LD) $(LDFLAGS) $(LIBDIRS) $(LIBS) -o $(PROGRAM) $(CPP_OBJECT_FILES); \
	$(foreach dir,$(OTHERS),cd $(dir); make; cd ..;)

-include depend

$(CPP_OBJECT_FILES): %.o: %.cpp
	@echo building $<...; \
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDE_DIRS) -o $@ $<	

clean:
	@echo cleaning $(PROGRAM)...; \
	rm -f $(CPP_OBJECT_FILES) $(PROGRAM) depend; \
	$(foreach dir,$(OTHERS),cd $(dir); make clean; cd ..;)

depend: $(CPP_SOURCE_FILES) $(CPP_HEADER_FILES)
	@echo generating dependencies...; \
	$(CC) -MM $(INCLUDE_DIRS) $(CPP_SOURCE_FILES) > $@
