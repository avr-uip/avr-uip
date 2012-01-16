#Chip choice
MCU	= atmega1284p
CHIP	= m1284p

#Project name
NAME    = halfmoon_tech


HTTPD_FS_DIRNAME = httpd-fs

#Compiled apps
APPS    = webserver.PROGMEM port_app_mapper
APPS += telnetd dhcpc

#Compiled drivers
DEVS	+= enc28j60.tux #The rest are determined by makefiles for apps

#Compiled libs
#LIBS	+= misc 1wire analog eeprom
LIBS += net_conf

#Project specific files
$(NAME)_SOURCES = main.c clock-arch.c a2d.c uart.c

#Include global configurations
-include ../../MakeConf.include

#Include the genetic makefile
-include ../../Makefile.include

