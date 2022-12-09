#====================================================================
# File automatically generated by ES3  
# This file regenerated each time you run EISC Studio, so save under a 
# new name if you hand edit, or it will be overwritten.
#				www.adc.co.kr
#====================================================================



ECHO	=	echo.exe
CC	=	ae32000-elf-gcc
AS	=	ae32000-elf-as
LD	=	ae32000-elf-ld
AR	=	ae32000-elf-ar
OBJCOPY	=	ae32000-elf-objcopy
OBJDUMP	=	ae32000-elf-objdump
CFLAGS	=	 -O2  -g -fno-strength-reduce -Wall 
ASFLAGS	=	 -gstabs 
LDFLAGS	=	-Wall -nostartfiles -lgcc -lc -lm
DISFLAGS	=	-dzShxl
ARFLAGS	=	rcusv
#====== Directory =====
OUTPUTDIR	=	output/
OBJDIR	=	obj/
INCDIR	=	
LIBDIR	=	
LIBS	=	

SOURCES	 += jaricom.c
SOURCES	 += jcapimin.c
SOURCES	 += jcapistd.c
SOURCES	 += jcarith.c
SOURCES	 += jccoefct.c
SOURCES	 += jccolor.c
SOURCES	 += jcdctmgr.c
SOURCES	 += jchuff.c
SOURCES	 += jcinit.c
SOURCES	 += jcmainct.c
SOURCES	 += jcmarker.c
SOURCES	 += jcmaster.c
SOURCES	 += jcomapi.c
SOURCES	 += jcparam.c
SOURCES	 += jcprepct.c
SOURCES	 += jcsample.c
SOURCES	 += jctrans.c
SOURCES	 += jdapimin.c
SOURCES	 += jdapistd.c
SOURCES	 += jdarith.c
SOURCES	 += jdatadst.c
SOURCES	 += jdatasrc.c
SOURCES	 += jdcoefct.c
SOURCES	 += jdcolor.c
SOURCES	 += jddctmgr.c
SOURCES	 += jdhuff.c
SOURCES	 += jdinput.c
SOURCES	 += jdmainct.c
SOURCES	 += jdmarker.c
SOURCES	 += jdmaster.c
SOURCES	 += jdmerge.c
SOURCES	 += jdpostct.c
SOURCES	 += jdsample.c
SOURCES	 += jdtrans.c
SOURCES	 += jerror.c
SOURCES	 += jfdctflt.c
SOURCES	 += jfdctfst.c
SOURCES	 += jfdctint.c
SOURCES	 += jidctflt.c
SOURCES	 += jidctfst.c
SOURCES	 += jidctint.c
SOURCES	 += jmemmgr.c
SOURCES	 += jmemnobs.c
SOURCES	 += jpegtran.c
SOURCES	 += jquant1.c
SOURCES	 += jquant2.c
SOURCES	 += jutils.c

OBJECTS	 += ${OBJDIR}jaricom.o
OBJECTS	 += ${OBJDIR}jcapimin.o
OBJECTS	 += ${OBJDIR}jcapistd.o
OBJECTS	 += ${OBJDIR}jcarith.o
OBJECTS	 += ${OBJDIR}jccoefct.o
OBJECTS	 += ${OBJDIR}jccolor.o
OBJECTS	 += ${OBJDIR}jcdctmgr.o
OBJECTS	 += ${OBJDIR}jchuff.o
OBJECTS	 += ${OBJDIR}jcinit.o
OBJECTS	 += ${OBJDIR}jcmainct.o
OBJECTS	 += ${OBJDIR}jcmarker.o
OBJECTS	 += ${OBJDIR}jcmaster.o
OBJECTS	 += ${OBJDIR}jcomapi.o
OBJECTS	 += ${OBJDIR}jcparam.o
OBJECTS	 += ${OBJDIR}jcprepct.o
OBJECTS	 += ${OBJDIR}jcsample.o
OBJECTS	 += ${OBJDIR}jctrans.o
OBJECTS	 += ${OBJDIR}jdapimin.o
OBJECTS	 += ${OBJDIR}jdapistd.o
OBJECTS	 += ${OBJDIR}jdarith.o
OBJECTS	 += ${OBJDIR}jdatadst.o
OBJECTS	 += ${OBJDIR}jdatasrc.o
OBJECTS	 += ${OBJDIR}jdcoefct.o
OBJECTS	 += ${OBJDIR}jdcolor.o
OBJECTS	 += ${OBJDIR}jddctmgr.o
OBJECTS	 += ${OBJDIR}jdhuff.o
OBJECTS	 += ${OBJDIR}jdinput.o
OBJECTS	 += ${OBJDIR}jdmainct.o
OBJECTS	 += ${OBJDIR}jdmarker.o
OBJECTS	 += ${OBJDIR}jdmaster.o
OBJECTS	 += ${OBJDIR}jdmerge.o
OBJECTS	 += ${OBJDIR}jdpostct.o
OBJECTS	 += ${OBJDIR}jdsample.o
OBJECTS	 += ${OBJDIR}jdtrans.o
OBJECTS	 += ${OBJDIR}jerror.o
OBJECTS	 += ${OBJDIR}jfdctflt.o
OBJECTS	 += ${OBJDIR}jfdctfst.o
OBJECTS	 += ${OBJDIR}jfdctint.o
OBJECTS	 += ${OBJDIR}jidctflt.o
OBJECTS	 += ${OBJDIR}jidctfst.o
OBJECTS	 += ${OBJDIR}jidctint.o
OBJECTS	 += ${OBJDIR}jmemmgr.o
OBJECTS	 += ${OBJDIR}jmemnobs.o
OBJECTS	 += ${OBJDIR}jpegtran.o
OBJECTS	 += ${OBJDIR}jquant1.o
OBJECTS	 += ${OBJDIR}jquant2.o
OBJECTS	 += ${OBJDIR}jutils.o

DEPS	 += ${OBJDIR}jaricom.d
DEPS	 += ${OBJDIR}jcapimin.d
DEPS	 += ${OBJDIR}jcapistd.d
DEPS	 += ${OBJDIR}jcarith.d
DEPS	 += ${OBJDIR}jccoefct.d
DEPS	 += ${OBJDIR}jccolor.d
DEPS	 += ${OBJDIR}jcdctmgr.d
DEPS	 += ${OBJDIR}jchuff.d
DEPS	 += ${OBJDIR}jcinit.d
DEPS	 += ${OBJDIR}jcmainct.d
DEPS	 += ${OBJDIR}jcmarker.d
DEPS	 += ${OBJDIR}jcmaster.d
DEPS	 += ${OBJDIR}jcomapi.d
DEPS	 += ${OBJDIR}jcparam.d
DEPS	 += ${OBJDIR}jcprepct.d
DEPS	 += ${OBJDIR}jcsample.d
DEPS	 += ${OBJDIR}jctrans.d
DEPS	 += ${OBJDIR}jdapimin.d
DEPS	 += ${OBJDIR}jdapistd.d
DEPS	 += ${OBJDIR}jdarith.d
DEPS	 += ${OBJDIR}jdatadst.d
DEPS	 += ${OBJDIR}jdatasrc.d
DEPS	 += ${OBJDIR}jdcoefct.d
DEPS	 += ${OBJDIR}jdcolor.d
DEPS	 += ${OBJDIR}jddctmgr.d
DEPS	 += ${OBJDIR}jdhuff.d
DEPS	 += ${OBJDIR}jdinput.d
DEPS	 += ${OBJDIR}jdmainct.d
DEPS	 += ${OBJDIR}jdmarker.d
DEPS	 += ${OBJDIR}jdmaster.d
DEPS	 += ${OBJDIR}jdmerge.d
DEPS	 += ${OBJDIR}jdpostct.d
DEPS	 += ${OBJDIR}jdsample.d
DEPS	 += ${OBJDIR}jdtrans.d
DEPS	 += ${OBJDIR}jerror.d
DEPS	 += ${OBJDIR}jfdctflt.d
DEPS	 += ${OBJDIR}jfdctfst.d
DEPS	 += ${OBJDIR}jfdctint.d
DEPS	 += ${OBJDIR}jidctflt.d
DEPS	 += ${OBJDIR}jidctfst.d
DEPS	 += ${OBJDIR}jidctint.d
DEPS	 += ${OBJDIR}jmemmgr.d
DEPS	 += ${OBJDIR}jmemnobs.d
DEPS	 += ${OBJDIR}jpegtran.d
DEPS	 += ${OBJDIR}jquant1.d
DEPS	 += ${OBJDIR}jquant2.d
DEPS	 += ${OBJDIR}jutils.d



#====== TAGET Define =====
TARGET	=	$(OUTPUTDIR)../../../lib/libjpeg.a

BINARY	=	$(TARGET).bin

HEX	=	$(TARGET).hex

DISASSEMBLE	=	$(TARGET).dis

ALL	:	$(TARGET) 

all	:	$(TARGET)

binary	:	$(BINARY)

bin	:	$(BINARY)

hex	:	$(HEX)

disassemble	:	$(DISASSEMBLE)

dis	:	$(DISASSEMBLE)

clean	:
	@$(ECHO) 'Deleting intermediate and output files... '
	rm -f $(OBJECTS) $(TARGET) $(DISASSEMBLE) $(BINARY) $(DEPS)
	@$(ECHO) ' '

cleanall	:
	@$(ECHO) 'Deleting intermediate and output files... '
	rm -f $(OBJECTS) $(TARGET) $(DISASSEMBLE) $(BINARY) $(DEPS)
	@$(ECHO) ' '

cleanobj	:
	@$(ECHO) 'Deleting intermediate files... '
	rm -f $(OBJECTS) 
	@$(ECHO) ' '

cleanbin	:
	rm -f $(BINARY)
	@$(ECHO) ' '

cleandis	:
	rm -f $(DISASSEMBLE)
	@$(ECHO) ' '

ifeq ($(FULLMSG),no)
ECHOMARK = @
else
ECHOMARK = 
endif

-include ${OBJDIR}jaricom.d
-include ${OBJDIR}jcapimin.d
-include ${OBJDIR}jcapistd.d
-include ${OBJDIR}jcarith.d
-include ${OBJDIR}jccoefct.d
-include ${OBJDIR}jccolor.d
-include ${OBJDIR}jcdctmgr.d
-include ${OBJDIR}jchuff.d
-include ${OBJDIR}jcinit.d
-include ${OBJDIR}jcmainct.d
-include ${OBJDIR}jcmarker.d
-include ${OBJDIR}jcmaster.d
-include ${OBJDIR}jcomapi.d
-include ${OBJDIR}jcparam.d
-include ${OBJDIR}jcprepct.d
-include ${OBJDIR}jcsample.d
-include ${OBJDIR}jctrans.d
-include ${OBJDIR}jdapimin.d
-include ${OBJDIR}jdapistd.d
-include ${OBJDIR}jdarith.d
-include ${OBJDIR}jdatadst.d
-include ${OBJDIR}jdatasrc.d
-include ${OBJDIR}jdcoefct.d
-include ${OBJDIR}jdcolor.d
-include ${OBJDIR}jddctmgr.d
-include ${OBJDIR}jdhuff.d
-include ${OBJDIR}jdinput.d
-include ${OBJDIR}jdmainct.d
-include ${OBJDIR}jdmarker.d
-include ${OBJDIR}jdmaster.d
-include ${OBJDIR}jdmerge.d
-include ${OBJDIR}jdpostct.d
-include ${OBJDIR}jdsample.d
-include ${OBJDIR}jdtrans.d
-include ${OBJDIR}jerror.d
-include ${OBJDIR}jfdctflt.d
-include ${OBJDIR}jfdctfst.d
-include ${OBJDIR}jfdctint.d
-include ${OBJDIR}jidctflt.d
-include ${OBJDIR}jidctfst.d
-include ${OBJDIR}jidctint.d
-include ${OBJDIR}jmemmgr.d
-include ${OBJDIR}jmemnobs.d
-include ${OBJDIR}jpegtran.d
-include ${OBJDIR}jquant1.d
-include ${OBJDIR}jquant2.d
-include ${OBJDIR}jutils.d

#============= TARGET ====================
${TARGET} : Makefile.mk ${OBJECTS} ${LIBS}
	@$(ECHO) 'Linking...'
	$(ECHOMARK)$(AR) $(ARFLAGS) $(TARGET) $(OBJECTS) $(LIBS)
	@$(ECHO) 'Finished building : $@ '
	@$(ECHO) ' '

$(BINARY): $(TARGET)
	@$(ECHO) 'Generating binary file...'
	$(ECHOMARK)$(OBJCOPY) -O binary $(TARGET) $@
	@$(ECHO) 'Finished building : $@ '
	@$(ECHO) ' '
	@$(ECHO) 'Program section information '
	$(ECHOMARK)size $(TARGET)
	@$(ECHO) ' '

$(DISASSEMBLE) : $(TARGET)
	@$(ECHO) 'Generating disassemble file...'
	$(ECHOMARK)$(OBJDUMP) $(DISFLAGS) $(TARGET) > $@ 
	@$(ECHO) 'Finished building : $@ '
	@$(ECHO) ' '

${OBJDIR}jaricom.o : jaricom.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jcapimin.o : jcapimin.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jcapistd.o : jcapistd.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jcarith.o : jcarith.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jccoefct.o : jccoefct.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jccolor.o : jccolor.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jcdctmgr.o : jcdctmgr.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jchuff.o : jchuff.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jcinit.o : jcinit.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jcmainct.o : jcmainct.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jcmarker.o : jcmarker.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jcmaster.o : jcmaster.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jcomapi.o : jcomapi.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jcparam.o : jcparam.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jcprepct.o : jcprepct.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jcsample.o : jcsample.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jctrans.o : jctrans.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jdapimin.o : jdapimin.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jdapistd.o : jdapistd.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jdarith.o : jdarith.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jdatadst.o : jdatadst.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jdatasrc.o : jdatasrc.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jdcoefct.o : jdcoefct.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jdcolor.o : jdcolor.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jddctmgr.o : jddctmgr.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jdhuff.o : jdhuff.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jdinput.o : jdinput.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jdmainct.o : jdmainct.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jdmarker.o : jdmarker.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jdmaster.o : jdmaster.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jdmerge.o : jdmerge.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jdpostct.o : jdpostct.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jdsample.o : jdsample.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jdtrans.o : jdtrans.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jerror.o : jerror.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jfdctflt.o : jfdctflt.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jfdctfst.o : jfdctfst.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jfdctint.o : jfdctint.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jidctflt.o : jidctflt.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jidctfst.o : jidctfst.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jidctint.o : jidctint.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jmemmgr.o : jmemmgr.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jmemnobs.o : jmemnobs.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jpegtran.o : jpegtran.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jquant1.o : jquant1.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jquant2.o : jquant2.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"

${OBJDIR}jutils.o : jutils.c
	@$(ECHO) 'Compile : $<'
	$(ECHOMARK)$(CC) $(CFLAGS) $(INCDIR) -c -fmessage-length=0 -MMD -MP -MF"$(@:.o=.d)" -MT"$(@:.o=.d)" -o "$@" "$<"
