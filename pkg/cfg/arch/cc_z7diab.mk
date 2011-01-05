# ###*B*###
# ERIKA Enterprise - a tiny RTOS for small microcontrollers

# Copyright (C) 2002-2010  Evidence Srl

# This file is part of ERIKA Enterprise.

# ERIKA Enterprise is free software; you can redistribute it
# and/or modify it under the terms of the GNU General Public License
# version 2 as published by the Free Software Foundation,
# (with a special exception described below).

# Linking this code statically or dynamically with other modules is
# making a combined work based on this code.  Thus, the terms and
# conditions of the GNU General Public License cover the whole
# combination.

# As a special exception, the copyright holders of this library give you
# permission to link this code with independent modules to produce an
# executable, regardless of the license terms of these independent
# modules, and to copy and distribute the resulting executable under
# terms of your choice, provided that you also meet, for each linked
# independent module, the terms and conditions of the license of that
# module.  An independent module is a module which is not derived from
# or based on this library.  If you modify this code, you may extend
# this exception to your version of the code, but you are not
# obligated to do so.  If you do not wish to do so, delete this
# exception statement from your version.

# ERIKA Enterprise is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License version 2 for more details.

# You should have received a copy of the GNU General Public License
# version 2 along with ERIKA Enterprise; if not, write to the
# Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301 USA.
# ###*E*###

##
## DCC for PPC
## Author: 2010 Fabio Checconi
## 2010 Bernardo  Dal Seno
##

# BINDIR is the directory of assembler, compiler, linker...
BINDIR =

# Compilers
EE_LINK ?= dld
EE_ASM ?= das
EE_CC ?= dcc
EE_AR ?= dar


OPT_INCLUDE = $(foreach d,$(INCLUDE_PATH),$(addprefix -I,$(call native_path,$d)))

## OPT_CC are the options for compiler invocation
# -Xstruct-arg-warning: warn if a structure too big is passed by value
# -Xkeywords=4: enable the inline keyword
OPT_CC = -Xlicense-wait -Xstderr-fully-buffered -Xbss-common-off	\
	-Xeieio -g3 -Xdebug-dwarf1 -XO -Xsavefpr-avoid \
	-Xsmall-data=8 -Xswitch-table=0 -Xinline=40 -Xsmall-const=0 \
	-Xenum-is-best -Xunroll=4 -Xunroll-size=5 -Xsize-opt -Xsemi-is-comment \
	-Xstop-on-warning -Xkeywords=4 $(ALLINCPATH) -c $(CFLAGS)
# -Xforce-prototypes 

ifneq ($(findstring __BIN_DISTR,$(EEALLOPT)), __BIN_DISTR)
ifeq ($(findstring DEBUG,$(EEOPT)) , DEBUG)
OPT_CC += -g
endif
endif

## OPT_ASM are the options for asm invocation
OPT_ASM =

ifneq ($(findstring __BIN_DISTR,$(EEALLOPT)), __BIN_DISTR)
ifeq ($(findstring DEBUG,$(EEOPT)) , DEBUG)
OPT_ASM += -g
endif
endif

# OPT_LINK represents the options for ld invocation
ifeq ($(findstring __DEFAULT_LD_SCRIPT__,$(EEOPT)) , __DEFAULT_LD_SCRIPT__)
OPT_LINK += out/loc_gnu.ld -e __start
LINKDEP = out/loc_gnu.ld
else
OPT_LINK +=
endif

DEFS_ASM = $(addprefix -D, $(EEOPT) )
DEFS_CC  = $(addprefix -D, $(EEOPT) )

# Automatic dependency generation
ifeq ($(call iseeopt, NODEPS), yes)
DEPENDENCY_OPT = 
make-depend =
else # NODEPS
ifeq ($(call iseeopt, __RTD_CYGWIN__), yes)
# Create dependency for all headers, and add a target for each header
DEPENDENCY_OPT = -Xmake-dependency=d -Xmake-dependency-target=$@ -Xmake-dependency-savefile=$(call native_path,$(subst .o,.d_tmp,$@))
# Dependencies on Windows need path translation
make-depend = @sed -e 's_\\\(.\)_/\1_g' -e 's_\<\([a-zA-Z]\):/_/cygdrive/\l\1/_g' < $3_tmp > $3 && rm $3_tmp
else # __RTD_CYGWIN__
# Create dependency for all headers, and add a target for each header
DEPENDENCY_OPT = -Xmake-dependency=d -Xmake-dependency-target=$@ -Xmake-dependency-savefile=$(subst .o,.d,$@)
make-depend =
endif # __RTD_CYGWIN__
endif # NODEPS
