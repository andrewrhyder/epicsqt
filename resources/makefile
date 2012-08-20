
# author: Ricardo Fernandes
# email: ricardo.fernandes@synchrotron.org.au
# copyright: (C) 2012 Australian Synchrotron
# version: 1.0
# date: 2012/AUG/07
# description: Makefile to automate the building and deployment of the EPICS Qt framework
# status: Production



# macro definitions
FRAMEWORK_DIRECTORY = epicsqt
PACKAGE_DIRECTORY = package
SVN_REPOSITORY = https://epicsqt.svn.sourceforge.net/svnroot/epicsqt
SVN = svn
LDD = ldd
GREP = grep
SED = sed
QMAKE = qmake-qt4




# make all; default target when nothing is specified when executing make
all: svncheckout buildframework




# svn checkout/update framework target
.PHONY: svncheckout
svncheckout:
	@echo "Checking conditions for building the framework..."

ifeq ($(strip $(QWT_INCLUDE_PATH)),)
	@$(error The environment variable 'QWT_INCLUDE_PATH' should be defined and pointing to the right QWT include directory (example: export QWT_INCLUDE_PATH=/usr/include/qwt). If QWT is not installed in your machine, please perform 'yum install qwt qwt-devel' in a terminal)
endif

ifeq ($(strip $(EPICS_BASE)),)
	@$(error The environment variable 'EPICS_BASE' should be defined and pointing to the right EPICS base directory (example: export EPICS_BASE=/epics/base-3-14-12-2). If EPICS is not installed in your machine, please download it from 'http://www.aps.anl.gov/epics/download/index.php')
endif

ifeq ($(strip $(EPICS_HOST_ARCH)),)
	@$(error The environment variable 'EPICS_HOST_ARCH' should be defined and with the right host architecture (example: export EPICS_HOST_ARCH=linux-x86))
endif

ifeq ($(shell which $(SVN) 2>/dev/null),)
	@$(error The program '$(SVN)' is not installed or could not be found in your machine (through the environment variable 'PATH'). Please, perform 'yum install svn' in a terminal to install it and/or modify the environment variable 'PATH' to point to this program)
endif

ifeq ($(shell which $(QMAKE) 2>/dev/null),)
	@$(error The program '$(QMAKE)' is not installed or could not be found in your machine (through the environment variable 'PATH'). Please, perform 'yum install qt4 qt4-devel' in a terminal to install it and/or modify the environment variable 'PATH' to point to this program)
endif

	@echo "...done checking conditions!"

	@if test -d $(FRAMEWORK_DIRECTORY); then \
		echo "Updating framework to the latest version from SVN repository to '"$(FRAMEWORK_DIRECTORY)"'..."; \
		$(SVN) update $(FRAMEWORK_DIRECTORY); \
		$(MAKE) -C $(FRAMEWORK_DIRECTORY) clean uninstall; \
		echo "...done updating framework!"; \
	else \
		echo "Checkout framework from SVN repository to '"$(FRAMEWORK_DIRECTORY)"'..."; \
		$(SVN) checkout $(SVN_REPOSITORY) $(FRAMEWORK_DIRECTORY); \
		echo "...done checkout framework!"; \
	fi




# build framework target
.PHONY: buildframework
buildframework:
	@echo "Building framework located in '"$(FRAMEWORK_DIRECTORY)"'..."
	@cd $(FRAMEWORK_DIRECTORY) ; $(QMAKE) epicsqt.pro -r -spec linux-g++
	@export LD_LIBRARY_PATH=$(EPICS_BASE)/lib/$(EPICS_HOST_ARCH) ; $(MAKE) -C $(FRAMEWORK_DIRECTORY) -w
	@echo "...done building framework!"





# package framework target
.PHONY: package
package:
	@echo "Checking conditions for packaging the framework..."

ifeq ($(strip $(EPICS_BASE)),)
	@$(error The environment variable 'EPICS_BASE' should be defined and pointing to the right EPICS base directory (example: export EPICS_BASE=/epics/base-3-14-12-2). If EPICS is not installed in your machine, please download it from 'http://www.aps.anl.gov/epics/download/index.php')
endif

ifeq ($(strip $(EPICS_HOST_ARCH)),)
	@$(error The environment variable 'EPICS_HOST_ARCH' should be defined and with the right host architecture (example: export EPICS_HOST_ARCH=linux-x86))
endif

ifeq ($(shell which $(LDD) 2>/dev/null),)
	@$(error The program '$(LDD)' is not installed or could not be found in your machine (through the environment variable 'PATH'). Please, perform 'yum install glibc-common' in a terminal to install it and/or modify the environment variable 'PATH' to point to this program)
endif

ifeq ($(shell which $(GREP) 2>/dev/null),)
	@$(error The program '$(GREP)' is not installed or could not be found in your machine (through the environment variable 'PATH'). Please, perform 'yum install grep' in a terminal to install it and/or modify the environment variable 'PATH' to point to this program)
endif

ifeq ($(shell which $(SED) 2>/dev/null),)
	@$(error The program '$(SED)' is not installed or could not be found in your machine (through the environment variable 'PATH'). Please, perform 'yum install sed' in a terminal to install it and/or modify the environment variable 'PATH' to point to this program)
endif

	@echo "...done checking conditions!"

	@if test -d $(FRAMEWORK_DIRECTORY); then \
		echo "The framework already exists in '"$(FRAMEWORK_DIRECTORY)"' so no need to build it."; \
	else \
		$(MAKE); \
	fi

	@echo "Packaging framework to '"$(PACKAGE_DIRECTORY)"'..."
	@mkdir -p $(PACKAGE_DIRECTORY)/lib
	@cp -r /usr/lib/qt4/plugins $(PACKAGE_DIRECTORY)/lib/.
	@cp $(FRAMEWORK_DIRECTORY)/applications/ASguiApp/ASgui $(PACKAGE_DIRECTORY)/.
	@cp $(FRAMEWORK_DIRECTORY)/framework/plugins/libQCaPlugin.so $(PACKAGE_DIRECTORY)/lib/.
	@cp $(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)/libca.so* $(PACKAGE_DIRECTORY)/lib/.
	@cp $(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)/libCom.so* $(PACKAGE_DIRECTORY)/lib/.
	@cd $(PACKAGE_DIRECTORY)/lib/plugins/designer ; ln -s -f ../../libQCaPlugin.so
	@chmod u+x $(PACKAGE_DIRECTORY)/ASgui
	@echo "#!/bin/bash" > script.tmp
	@echo "`$(LDD) $(PACKAGE_DIRECTORY)/ASgui | $(GREP) 'libQt\|libqwt\|libreadline\|libncurses' | $(SED) s/lib.*\>/cp/ | $(SED) s/\(.*\)/$(PACKAGE_DIRECTORY)\\\/lib/`" >> script.tmp
	@chmod u+x script.tmp
	@./script.tmp
	@rm -f script.tmp
	@echo "#!/bin/bash" > $(PACKAGE_DIRECTORY)/execute
	@echo "export LD_LIBRARY_PATH=\$$LD_LIBRARY_PATH:./lib:./lib/plugins/designer/" >> $(PACKAGE_DIRECTORY)/execute
	@echo "export QT_PLUGIN_PATH=./lib/plugins" >> $(PACKAGE_DIRECTORY)/execute
	@echo "./ASgui -m \$$1" >> $(PACKAGE_DIRECTORY)/execute
	@chmod u+x $(PACKAGE_DIRECTORY)/execute
	@echo "Please note that a file called 'execute' (located in '"$(PACKAGE_DIRECTORY)"') was created in order to facilitate the launching of ui files. To use it, do the following in a terminal: ./execute XXX (where XXX is the ui file to be launched)."
	@echo "...done packaging framework!"




# clean framework target
.PHONY: clean
clean:
	@echo "Cleaning framework located in '"$(FRAMEWORK_DIRECTORY)"' and package located in '"$(PACKAGE_DIRECTORY)"'..."
	@rm -rf $(FRAMEWORK_DIRECTORY) $(PACKAGE_DIRECTORY)
	@echo "...done cleaning framework and package!"



