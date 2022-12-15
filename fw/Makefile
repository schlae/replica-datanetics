

GCC := avr-gcc
RM := rm
OBJCOPY := avr-objcopy
MINIPRO := minipro

PROJECT_NAME = apple1keyb

GCCFLAGS = -Os -DF_CPU=1000000UL -mmcu=atmega644pa -c
LDFLAGS = -mmcu=atmega644pa

OBJS := $(patsubst %.c,%.o,$(wildcard *.c))
OBJS := $(addprefix build/,$(sort $(OBJS)))

.PHONY: all clean target prog

all: target

clean:
	$(RM) -r build

target: $(OBJS) build/${PROJECT_NAME} build/${PROJECT_NAME}.hex

build/%.o: %.c
	@echo Compiling $<
	@mkdir -p $(@D)
	@$(GCC) $(GCCFLAGS) -o $@ $<

build/${PROJECT_NAME}: $(OBJS)
	$(GCC) $(LDFLAGS) $(OBJS) -o $@
#    $(GCC) $(LDFLAGS) $@

build/${PROJECT_NAME}.hex: build/${PROJECT_NAME}
	$(OBJCOPY) -O ihex -R .eeprom $< $@

prog:
	$(MINIPRO) -p "ATMEGA644PA" -w build/${PROJECT_NAME}.hex

fuses:
	$(MINIPRO) -p "ATMEGA644PA" --fuses -w fuses.conf
