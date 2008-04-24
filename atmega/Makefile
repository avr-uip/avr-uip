MCU		= atmega644
CHIP	= m644		#Chip parameter given to avrdude
#TODO: Add programmer specific configuration parameters for avrdude, maybe place these in a toplevel .include makefile
#TODO: Make DRIVERS variable that works as APPS does
#TODO: Make LIBS variables that works as APPS does

CC		= avr-gcc
AR		= ar
OBJCOPY	= avr-objcopy
APPS	= simple-httpd
NAME    = avr-uip

#I'm no compiler expert but these seams to work
CFLAGS	= -I../uip -I. -Wall -mmcu=$(MCU) -g -fpack-struct -Os -funsigned-char -funsigned-bitfields -mcall-prologues -Wstrict-prototypes -std=gnu99 

#Sources for this architecture port
ATMEGA_SOURCES = main.c clock-arch.c network.c enc28j60.c

all: $(NAME).hex

-include ../uip/Makefile.include

$(NAME)_SOURCES = $(APP_SOURCES) $(UIP_SOURCES) $(ATMEGA_SOURCES)

uip_OBJECTS: ${addprefix $(OBJECTDIR)/, $(UIP_SOURCES:.c=.o)}
apps_OBJECTS: ${addprefix $(OBJECTDIR)/, $(APP_SOURCES:.c=.o)}
atmega_OBJECTS: $(addprefix $(OBJECTDIR)/, $(ATMEGA_SOURCES:.c=.o))

$(NAME).out: atmega_OBJECTS apps_OBJECTS uip_OBJECTS
	$(CC) $(CFLAGS) -o $(OBJECTDIR)/$(NAME).out -Wl ${addprefix $(OBJECTDIR)/, $($(NAME)_SOURCES:.c=.o)}
$(NAME).hex: $(NAME).out
	$(OBJCOPY) -R .eeprom -O ihex $(OBJECTDIR)/$(NAME).out $(OBJECTDIR)/$(NAME).hex
	avr-size $(OBJECTDIR)/$(NAME).out
	@echo "Expl.: data=initialized data, bss=uninitialized data, text=code"

#Loads without verifying
load: $(NAME).hex
	set -x
	avrdude -V -p $(CHIP) -c avrusb500 -e -B 2 -U flash:w:$(OBJECTDIR)/$(NAME).hex
	set +x
#Loads with verification
loadcheck: $(NAME).hex
	set -x
	avrdude -p $(CHIP) -c avrusb500 -e -B 2 -U flash:w:$(OBJECTDIR)/$(NAME).hex
	set +x
clean:
	rm -fr *.o *~ *core uip $(OBJECTDIR) *.a
help:
	echo "Somebody please write a help page for this makefile";
