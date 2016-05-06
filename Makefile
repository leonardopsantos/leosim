
SOURCEDIR = src
BUILDDIR = build

CPP_FILES := $(wildcard $(SOURCEDIR)/*.cc)
OBJ_FILES := $(addprefix $(BUILDDIR)/,$(notdir $(CPP_FILES:.cc=.o)))
LD_FLAGS :=
CC_FLAGS := -std=c++11 -Wall -O0 -g -I$(SOURCEDIR)

leosim: $(OBJ_FILES)
	g++ $(LD_FLAGS) -o $(BUILDDIR)/$@ $^

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.cc
	g++ $(CC_FLAGS) -c -o $@ $<

.PHONY: createdirs
createdirs:
	mkdir -p $(@D)/$(BUILDDIR)

.PHONY: clean
clean:
	rm -f $(BUILDDIR)/*

all: createdirs leosim
