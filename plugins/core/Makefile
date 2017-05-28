#
# Compiler: g++
#

PROGRAM := ../libcore.so
VERSION := 0.1

DIRS := . 
CC := g++
LD := g++

CFLAGS := -c
LDFLAGS := -shared
LIBS :=
LIBDIRS :=
DEFINES := -DPROGRAM=\"$(PROGRAM)\" -DVERSION=\"$(VERSION)\" -DBMOTION_PLUGIN

CFLAGS := $(if $(DEBUG)==1, $(CFLAGS) -ggdb, $(CFLAGS)) -Wall -W
LDFLAGS := $(if $(DEBUG)==1, $(LDFLAGS) -ggdb, $(LDFLAGS))

CPP_SOURCE_FILES := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp))
CPP_OBJECT_FILES := $(CPP_SOURCE_FILES:.cpp=.o)
INCLUDE_DIRS := $(foreach dir,$(DIRS),-I$(dir)) -I../../

default: $(CPP_OBJECT_FILES) 
	@echo linking  $(PROGRAM)...; \
	$(LD) $(LDFLAGS) $(LIBDIRS) $(LIBS) -o $(PROGRAM) $(CPP_OBJECT_FILES)

-include depend

$(CPP_OBJECT_FILES): %.o: %.cpp
	@echo building $<...; \
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDE_DIRS) -o $@ $<	

clean:
	@echo cleaning $(PROGRAM)...; \
	rm -f $(CPP_OBJECT_FILES) $(PROGRAM) depend

depend: $(CPP_SOURCE_FILES) $(CPP_HEADER_FILES)
	@echo generating dependencies...; \
	$(CC) -MM $(CPP_SOURCE_FILES) > $@
