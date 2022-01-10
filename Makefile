PROGS = ray
ray_OBJS += BaseObject.o
ray_OBJS += Color.o
ray_OBJS += Ellipsoid.o
ray_OBJS += Image.o
ray_OBJS += Light.o
ray_OBJS += Plane.o
ray_OBJS += Ray.o
ray_OBJS += Scene.o
ray_OBJS += Targa.o
ray_OBJS += Tracer.o
ray_OBJS += VectorMath.o

CC = g++
CFLAGS := -pedantic -Wall -DHAVE_CONFIG_H
CFLAGS += -O3 -march=native
CFLAGS += -ggdb3
#CFLAGS = -march=nocona -O3 -fomit-frame-pointer -fgcse-after-reload -funswitch-loops -mfpmath=sse -mtune=nocona -DHAVE_CONFIG_H
#CC = icpc
#CFLAGS = -Wall -DHAVE_CONFIG_H -fast
#CFLAGS = -pedantic -Wall -O0 -g -DHAVE_CONFIG_H

CXXFLAGS := $(CFLAGS)
CXXFLAGS += -std=c++17
LDFLAGS = -pthread

DEPDIR = .deps
df = $(DEPDIR)/$(*F)
BUILDDIR = .build
bf = $(BUILDDIR)/$(*F)

.PHONY: all clean

all: $(BUILDDIR) $(DEPDIR) $(PROGS)

$(BUILDDIR):
	@mkdir $(BUILDDIR)

$(DEPDIR):
	@mkdir $(DEPDIR)

$(BUILDDIR)/%.o: %.c
	@$(CC) -MD $(CFLAGS) -c -o $@ $<
	@echo "  CC    $<"
	@mv $(bf).d $(df).d

$(BUILDDIR)/%.o: %.cc
	@$(CC) -MD $(CXXFLAGS) -c -o $@ $<
	@echo "  CC    $<"
	@mv $(bf).d $(df).d

-include $(ray_OBJS:%.o=$(DEPDIR)/%.d)

ray: $(ray_OBJS:%.o=$(BUILDDIR)/%.o)
	@$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(OUTPUT_OPTION)
	@echo "  LN    $@"

clean:
	@rm -rf $(BUILDDIR) $(PROGS) $(DEPDIR)
	@echo "  CLEAN"

