
# author: Ricardo Fernandes
# email: ricardo.fernandes@synchrotron.org.au
# copyright: (C) 2012 Australian Synchrotron
# version: 1.1
# date: 2013/FEB/19
# description: Makefile to automate the building and deployment of the EPICS Qt framework
# note: Although managed as part of the EPICS Qt framework, this makefile is intended to be run from outside the framework directories
# status: Production



# macro definitions
FRAMEWORK_DIRECTORY = epicsqt
PACKAGE_DIRECTORY = package
RPM_DIRECTORY = rpmbuild
SVN_REPOSITORY = https://epicsqt.svn.sourceforge.net/svnroot/epicsqt
EXECUTE_FILE = $(PACKAGE_DIRECTORY)/executeQEGui
SPEC_FILE = $(RPM_DIRECTORY)/SPECS/epicsqt.spec
SVN = svn
LDD = ldd
GREP = grep
SED = sed
RPM = rpmbuild
RPM_DEPLOY_DIRECTORY = /usr/local/epicsqt_bin
QMAKE = qmake-qt4



# make all; default target when nothing is specified when executing make
all: svn_checkout build_framework



# svn checkout/update framework target
.PHONY: svn_checkout
svn_checkout:
	@echo "Checking conditions for building the framework in directory '$(FRAMEWORK_DIRECTORY)'..."

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
		echo "Updating framework to the latest version from SVN repository to '$(FRAMEWORK_DIRECTORY)'..."; \
		$(SVN) update $(FRAMEWORK_DIRECTORY); \
		$(MAKE) -C $(FRAMEWORK_DIRECTORY) clean uninstall; \
		echo "...done updating framework!"; \
	else \
		echo "Checkout framework from SVN repository to '$(FRAMEWORK_DIRECTORY)'..."; \
		$(SVN) checkout $(SVN_REPOSITORY) $(FRAMEWORK_DIRECTORY); \
		echo "...done checkout framework!"; \
	fi



# build framework target
.PHONY: build_framework
build_framework:
	@echo "Building framework located in '$(FRAMEWORK_DIRECTORY)'..."
	@cd $(FRAMEWORK_DIRECTORY) ; $(QMAKE) epicsqt.pro -r -spec linux-g++
	@export LD_LIBRARY_PATH=$(EPICS_BASE)/lib/$(EPICS_HOST_ARCH) ; $(MAKE) -C $(FRAMEWORK_DIRECTORY) -w -j2
	@echo "...done building framework!"



# package framework target
.PHONY: package
package:
	@echo "Checking conditions for packaging the framework in directory '$(PACKAGE_DIRECTORY)'..."

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
		echo "The framework already exists in directory '$(FRAMEWORK_DIRECTORY)' so no need to build it."; \
	else \
		$(MAKE); \
	fi

	@echo "Packaging framework in directory '$(PACKAGE_DIRECTORY)'..."
	@mkdir -p $(PACKAGE_DIRECTORY)/lib $(PACKAGE_DIRECTORY)/bin
	@cp -r /usr/lib/qt4/plugins $(PACKAGE_DIRECTORY)/lib/.
	@cp $(FRAMEWORK_DIRECTORY)/applications/QEGuiApp/QEGui $(PACKAGE_DIRECTORY)/bin/.
	@cp $(FRAMEWORK_DIRECTORY)/framework/designer/libQEPlugin.so $(PACKAGE_DIRECTORY)/lib/.
	@cp $(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)/libca.so* $(PACKAGE_DIRECTORY)/lib/.
	@cp $(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)/libCom.so* $(PACKAGE_DIRECTORY)/lib/.
	@cp $(EPICS_BASE)/bin/$(EPICS_HOST_ARCH)/caget $(PACKAGE_DIRECTORY)/bin/.
	@cp $(EPICS_BASE)/bin/$(EPICS_HOST_ARCH)/caput $(PACKAGE_DIRECTORY)/bin/.
	@cp $(EPICS_BASE)/bin/$(EPICS_HOST_ARCH)/camonitor $(PACKAGE_DIRECTORY)/bin/.
	@cp $(EPICS_BASE)/bin/$(EPICS_HOST_ARCH)/cainfo $(PACKAGE_DIRECTORY)/bin/.
	@cd $(PACKAGE_DIRECTORY)/lib/plugins/designer ; ln -s -f ../../libQEPlugin.so
	@chmod u+x $(PACKAGE_DIRECTORY)/bin/QEGui
	@echo "#!/bin/bash" > script.tmp
	@echo "`$(LDD) $(PACKAGE_DIRECTORY)/bin/QEGui | $(GREP) 'libQt\|libqwt\|libreadline\|libncurses' | $(SED) s/lib.*\>/cp/ | $(SED) s/\(.*\)/$(PACKAGE_DIRECTORY)\\\/lib/`" >> script.tmp
	@chmod u+x script.tmp
	@./script.tmp
	@rm -f script.tmp
	@echo "#!/bin/bash" > $(EXECUTE_FILE)
	@echo "export LD_LIBRARY_PATH=\$$LD_LIBRARY_PATH:./lib:./lib/plugins/designer/" >> $(EXECUTE_FILE)
	@echo "export QT_PLUGIN_PATH=./lib/plugins" >> $(EXECUTE_FILE)
	@echo "./bin/QEGui \"\$$@\"" >> $(EXECUTE_FILE)
	@chmod u+x $(EXECUTE_FILE)
	@echo "Please note that a file called 'executeQEGui' (located in directory '$(PACKAGE_DIRECTORY)') was created in order to facilitate the launching of ui files. To use it, do the following in a terminal: ./executeQEGui XXX (where XXX is the ui file to be launched)."
	@echo "...done packaging framework!"



# rpm target
.PHONY: rpm
rpm:
	@echo "Checking conditions for creating RPM of the framework in directory '$(RPM_DIRECTORY)'..."

ifeq ($(shell which $(RPM) 2>/dev/null),)
	@$(error The program '$(RPM)' is not installed or could not be found in your machine (through the environment variable 'PATH'). Please, perform 'yum install rpm-build' in a terminal to install it and/or modify the environment variable 'PATH' to point to this program)
endif

ifeq ($(shell which $(SED) 2>/dev/null),)
	@$(error The program '$(SED)' is not installed or could not be found in your machine (through the environment variable 'PATH'). Please, perform 'yum install sed' in a terminal to install it and/or modify the environment variable 'PATH' to point to this program)
endif

	@echo "...done checking conditions!"

	@if test -d $(PACKAGE_DIRECTORY); then \
		echo "The package already exists in directory '$(PACKAGE_DIRECTORY)' so no need to build it."; \
	else \
		$(MAKE) package; \
	fi

	@echo "Creating RPM of the framework in directory '$(RPM_DIRECTORY)'..."
	@mkdir -p $(RPM_DIRECTORY) $(RPM_DIRECTORY)/SPECS $(RPM_DIRECTORY)/SOURCES $(RPM_DIRECTORY)/BUILD $(RPM_DIRECTORY)/BUILDROOT $(RPM_DIRECTORY)/RPMS $(RPM_DIRECTORY)/SRPMS
	@echo "%_topdir $(PWD)/rpmbuild" > $(HOME)/.rpmmacros
	@echo "Name: epics_qt_framework" > $(SPEC_FILE)
	@echo "Summary: A layered framework based on Qt for accessing Experimental Physics and Industrial Control System (EPICS) data using Channel Access (CA). Designed for rapid development of control system graphical interfaces." >> $(SPEC_FILE)
	@echo "Version: 2.2" >> $(SPEC_FILE)
	@echo "Release: 23" >> $(SPEC_FILE)
	@echo "License: GPLv3" >> $(SPEC_FILE)
	@echo "AutoReqProv: no" >> $(SPEC_FILE)
	@echo "Prefix: /usr/local" >> $(SPEC_FILE)
	@echo "%define _builddir ./rpmbuild/" >> $(SPEC_FILE)
	@echo "" >> $(SPEC_FILE)
	@echo "%description" >> $(SPEC_FILE)
	@echo "This package installs the EPICS Qt Framework in a self-contained directory." >> $(SPEC_FILE)
	@echo "" >> $(SPEC_FILE)
	@echo "%prep" >> $(SPEC_FILE)
	@echo "mkdir -p ./BUILDROOT/%{name}-%{version}-%{release}.i386$(RPM_DEPLOY_DIRECTORY)" >> $(SPEC_FILE)
	@echo "cp -rf ../$(PACKAGE_DIRECTORY)/* ./BUILDROOT/%{name}-%{version}-%{release}.i386$(RPM_DEPLOY_DIRECTORY)" >> $(SPEC_FILE)
	@echo "#find ./BUILDROOT/%{name}-%{version}-%{release}.i386/epicsqt_bin -iname '*.so*' | while read filename ; do mv \"\$${filename}\" \"\`echo \"\$${filename}\" | sed -e 's/\.so/\.s_/'\`\"; done" >> $(SPEC_FILE)
	@echo "" >> $(SPEC_FILE)
	@echo "%files" >> $(SPEC_FILE)
	@echo "%attr(755, root, root) $(RPM_DEPLOY_DIRECTORY)/executeQEGui" >> $(SPEC_FILE)
	@echo "/*" >> $(SPEC_FILE)
	@echo "" >> $(SPEC_FILE)
	@echo "%post" >> $(SPEC_FILE)
	@echo "#find $(RPM_DEPLOY_DIRECTORY) -iname '*.s_*' | while read filename ; do mv \"\$${filename}\" \"\`echo \"\$${filename}\" | sed -e 's/\.s_/\.so/'\`\"; done" >> $(SPEC_FILE)
	@echo "" >> $(SPEC_FILE)
	@$(RPM) -bb $(SPEC_FILE)
	@echo "Please note that a RPM was created and it is located in './$(RPM_DIRECTORY)/RPMS/i386'."



# clean all target
.PHONY: clean
clean: clean_framework clean_package clean_rpm



# clean framework target
.PHONY: clean_framework
clean_framework:
	@echo "Cleaning framework located in directory '$(FRAMEWORK_DIRECTORY)'..."
	@rm -rf $(FRAMEWORK_DIRECTORY)
	@echo "...done cleaning framework!"



# clean package target
.PHONY: clean_package
clean_package:
	@echo "Cleaning package located in directory '$(PACKAGE_DIRECTORY)'..."
	@rm -rf $(PACKAGE_DIRECTORY)
	@echo "...done cleaning package!"



# clean rpm target
.PHONY: clean_rpm
clean_rpm:
	@echo "Cleaning RPM located in directory '$(RPM_DIRECTORY)'..."
	@rm -rf $(RPM_DIRECTORY)
	@echo "...done cleaning RPM!"



