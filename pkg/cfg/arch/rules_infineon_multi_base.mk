# ###*B*###
# ERIKA Enterprise - a tiny RTOS for small microcontrollers

# Copyright (C) 2002-2012  Evidence Srl

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

## Author: 2012 Errico Guidieri

#Used by start scipts
TEMPDIR ?= C:/Temp
#used to export symbols to slave cpus
GLOBAL_LINKSCRIPT := shared_sym.$(CC_LD_SUFFIX)
#Slave CPU List
SLAVE_CPUS := $(filter-out CPU_MASTER, $(CPU_LIST))
#Master ELF Target
MASTER_ELF = $(CPU_MASTER_DIR)/$(CPU_MASTER_ELF)

.PHONY: all clean
.DEFAULT_GOAL := all

all: $(foreach c, $(CPU_LIST), $(c)-all) tc27x_mc_start.bat tc27x_mc_start.sh config_tc27x_mc.t32 t32_tc27x_mc.cmm

clean: $(foreach c, $(CPU_LIST), $(c)-clean)

# Create a pattern rule for each CPU; one pattern for `all', and one for `clean'
define all-clean-template
 .PHONY: $(1)-all $(1)-clean
 $(1)-all $(1)-clean: $(1)-%:
	$(MAKE) -C $($(1)_DIR) ELFNAME=$($(1)_ELF)  		\
		CPU_NUMID=$($(1)_ID) 							\
		GLOBAL_LINKSCRIPT=../$(GLOBAL_LINKSCRIPT) $$*
endef

$(foreach c, $(CPU_LIST), $(eval $(call all-clean-template,$c)))

$(foreach s, $(SLAVE_CPUS), $(s)-all): $(GLOBAL_LINKSCRIPT)

config_tc27x_mc.t32: %: $(PKGBASE)/mcu/infineon_$(TRICORE_MODEL)/cfg/multicore/%
	@echo CP $@
	$(QUIET) cp $< $@

tc27x_mc_start.bat tc27x_mc_start.sh: %: $(PKGBASE)/mcu/infineon_$(TRICORE_MODEL)/cfg/multicore/%
	@echo GEN $@
	$(QUIET) sed -e 's:#T32SYS#:$(T32SYS):g'	\
		-e 's:#TEMPDIR#:$(TEMPDIR):g'			\
		$< > $@

t32_tc27x_mc.cmm: %: $(PKGBASE)/mcu/infineon_$(TRICORE_MODEL)/cfg/multicore/%
	@echo GEN $@
	$(QUIET) sed -e 's:#MASTER_ELF#:$(MASTER_ELF):g'  		\
		$(foreach s, $(SLAVE_CPUS),							\
			-e 's:#CPU1_DIR#:$($(s)_DIR):g'					\
			-e 's:#CPU1_EXE_NAME#:$($(s)_ELF):g'			\
		) $< > $@

$(MASTER_ELF): CPU_MASTER-all

$(GLOBAL_LINKSCRIPT): $(MASTER_ELF)
	@echo Building shared symbol table
	$(QUIET) $() -F2 -s ee_mcglobalc -s ee_mcglobald -s ee_mcglobalu	\
			-s ee_fast_mcglobalc -s ee_fast_mcglobald					\
			-s ee_fast_mcglobalu $<										\
		| awk '/^[0-9a-fA-F]+ ......O/ {								\
			match($$0, "^([0-9a-fA-F]+) .+ ([^ ]+)$$", m);				\
			printf("%s = 0x%s;\n", m[2], m[1]) }' > $@
