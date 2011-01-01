## Check the Console library selection
ifeq ($(findstring __LIB_TCPIP__,$(LIB_OPT)) , __LIB_TCPIP__)
INCLUDE_TCPIP = YES
endif

## Check if it's selected all libs inclusion
ifeq ($(findstring __BUILD_ALL_LIBS__,$(EEOPT)) , __BUILD_ALL_LIBS__)
INCLUDE_TCPIP = YES
endif

##
## If the library is required
##
ifeq ($(INCLUDE_TCPIP), YES)

##
## Library code
##
## Add the inc path to the include pathlist
ifeq ($(findstring __RTD_CYGWIN__,$(EEOPT)), __RTD_CYGWIN__) 
ALLINCPATH += -I"$(shell cygpath -w $(EEBASE)/contrib/microchip/tcpip_520/inc)"
else
ALLINCPATH += -I$(EEBASE)/contrib/microchip/tcpip_520/inc
endif

## Add each file individually
EE_SRCS_TCPIP := $(shell ls $(EEBASE)/contrib/microchip/tcpip_520/libsrc/*.c)
## If the file list is not empty, create the tcpip lib
ifneq ($(EE_SRCS_TCPIP),)
EE_OBJS_TCPIP := $(addprefix $(OBJDIR)/, $(patsubst %.c,%.o,$(patsubst %.S,%.o, $(EE_SRCS_TCPIP))))
LIBSRCS += $(EE_SRCS_TCPIP)
libtcpip.a: $(EE_OBJS_TCPIP)
	@echo $(EE_SRC_TCPIP)
	@printf "AR  libtcpip.a\n" ;
	$(QUIET)$(EE_AR) rs libtcpip.a $(EE_OBJS_TCPIP)
OPT_LIBS += -ltcpip
ALL_LIBS += libtcpip.a
endif

endif