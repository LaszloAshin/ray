PROGS = ray
ray_OBJS += BaseObject.o
ray_OBJS += Color.o
ray_OBJS += Image.o
ray_OBJS += Light.o
ray_OBJS += MultithreadedTracer.o
ray_OBJS += Plane.o
ray_OBJS += Ray.o
ray_OBJS += Scene.o
ray_OBJS += Spheroid.o
ray_OBJS += Targa.o
ray_OBJS += Tracer.o
ray_OBJS += Vector.o
ray_OBJS += VectorMath.o

CPPFLAGS := -DHAVE_CONFIG_H
CPPFLAGS += -MD
CPPFLAGS += -ffunction-sections -fdata-sections

CFLAGS := -pedantic -Wall -Wextra
CFLAGS += -O3 -march=native
CFLAGS += -ggdb3

CXXFLAGS := $(CFLAGS)
CXXFLAGS += -std=c++17
CXXFLAGS += -fno-exceptions -fno-rtti

LDFLAGS := -pthread
LDFLAGS += -Wl,--gc-sections

DEPDIR = .deps
df = $(DEPDIR)/$(*F)
BUILDDIR = .build
bf = $(BUILDDIR)/$(*F)

Q := $(if $V,,@)

.PHONY: all clean

all: $(BUILDDIR) $(DEPDIR) $(PROGS)

$(BUILDDIR):
	@mkdir $(BUILDDIR)

$(DEPDIR):
	@mkdir $(DEPDIR)

$(BUILDDIR)/%.o: %.c
	$Q$(CC) -c $(CPPFLAGS) $(CFLAGS) $(OUTPUT_OPTION) $<
	$(if $Q,@echo "  CC    $@")
	@mv $(bf).d $(df).d

$(BUILDDIR)/%.o: %.cc
	$Q$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(OUTPUT_OPTION) $<
	$(if $Q,@echo "  CXX   $@")
	@mv $(bf).d $(df).d

-include $(ray_OBJS:%.o=$(DEPDIR)/%.d)

ray: $(ray_OBJS:%.o=$(BUILDDIR)/%.o)
	$Q$(CXX) $(LDFLAGS) $^ $(LOADLIBES) $(OUTPUT_OPTION)
	$(if $Q,@echo "  LINK  $@")

clean:
	@rm -rf $(BUILDDIR) $(PROGS) $(DEPDIR)
	@echo "  CLEAN"

