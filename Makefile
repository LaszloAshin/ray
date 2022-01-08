PROGS = ray
ray_OBJS = BaseObject.o Color.o Ellipsoid.o Plane.o Ray.o Scene.o Targa.o Tracer.o VectorMath.o

CC = g++
CFLAGS = -pedantic -Wall -DHAVE_CONFIG_H
CFLAGS += -O3 -march=native
CFLAGS += -ggdb3
#CFLAGS = -march=nocona -O3 -fomit-frame-pointer -fgcse-after-reload -funswitch-loops -mfpmath=sse -mtune=nocona -DHAVE_CONFIG_H
#CC = icpc
#CFLAGS = -Wall -DHAVE_CONFIG_H -fast
#CFLAGS = -pedantic -Wall -O0 -g -DHAVE_CONFIG_H

CXXFLAGS = $(CFLAGS)
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

