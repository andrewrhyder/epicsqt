# author: Ricardo Fernandes
# email: ricardo.fernandes@synchrotron.org.au
# copyright: (C) 2012 Australian Synchrotron
# version: 1.5
# date: 2013/AUG/13
# description: Makefile to automate the building and deployment of the EPICS Qt framework
# note: Although managed as part of the EPICS Qt framework, this makefile is intended to be run from outside the framework directories
# status: Production



# ================================================
#  MACRO DEFINITIONS
# ================================================
FRAMEWORK_DIRECTORY = epicsqt
PACKAGE_DIRECTORY = package
RPM_DIRECTORY = rpmbuild
SVN_REPOSITORY = https://svn.code.sf.net/p/epicsqt/code
EXECUTE_FILE = $(PACKAGE_DIRECTORY)/executeQEGui
SPEC_FILE = $(RPM_DIRECTORY)/SPECS/epicsqt.spec
SOURCEFORGE_HOSTNAME = "shell.sourceforge.net"
SOURCEFORGE_DIRECTORY = "/home/frs/project/epicsqt"
SVN = svn
LDD = ldd
GREP = grep
SED = sed
PRELINK = prelink
RPM_DEPLOY_DIRECTORY = /usr/local/epicsqt_bin
QMAKE = qmake-qt4



# ================================================
#  DEFINE FRAMEWORK_VERSION MACRO FOR trunk OR tag (IF SPECIFIED)
# ================================================
ifdef tag
	FRAMEWORK_VERSION = tags/$(tag)
else
	FRAMEWORK_VERSION = trunk
endif

ifdef TAG
	FRAMEWORK_VERSION = tags/$(TAG)
endif



# ================================================
#  MAKE ALL; DEFAULT TARGET WHEN NOTHING IS SPECIFIED WHEN EXECUTING make
# ================================================
all: checkout framework



# ================================================
#  SVN CHECKOUT/UPDATE FRAMEWORK TARGET
# ================================================
.PHONY: checkout
checkout:
	@echo "Checking conditions for building the framework in directory '$(FRAMEWORK_DIRECTORY)/$(FRAMEWORK_VERSION)'..."

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

	@if test -d $(FRAMEWORK_DIRECTORY)/$(FRAMEWORK_VERSION); then \
		echo "Updating framework from SVN repository to '$(FRAMEWORK_DIRECTORY)/$(FRAMEWORK_VERSION)'..."; \
		$(SVN) update $(FRAMEWORK_DIRECTORY)/$(FRAMEWORK_VERSION); \
		$(MAKE) -C $(FRAMEWORK_DIRECTORY)/$(FRAMEWORK_VERSION) clean uninstall; \
		echo "...done updating framework!"; \
	else \
		echo "Checkout framework from SVN repository to '$(FRAMEWORK_DIRECTORY)/$(FRAMEWORK_VERSION)'..."; \
		$(SVN) checkout $(SVN_REPOSITORY)/$(FRAMEWORK_VERSION) $(FRAMEWORK_DIRECTORY)/$(FRAMEWORK_VERSION); \
		echo "...done checkout framework!"; \
	fi



# ================================================
#  BUILD FRAMEWORK TARGET
# ================================================
.PHONY: framework
framework:
	@echo "Building framework located in '$(FRAMEWORK_DIRECTORY)/$(FRAMEWORK_VERSION)'..."
	@cd $(FRAMEWORK_DIRECTORY)/$(FRAMEWORK_VERSION) ; $(QMAKE) epicsqt.pro -r -spec linux-g++
	@export LD_LIBRARY_PATH=$(EPICS_BASE)/lib/$(EPICS_HOST_ARCH) ; $(MAKE) -C $(FRAMEWORK_DIRECTORY)/$(FRAMEWORK_VERSION) -w -j2 ; $(MAKE) -C $(FRAMEWORK_DIRECTORY)/$(FRAMEWORK_VERSION)/framework install_headers
	@echo "...done building framework!"



# ================================================
#  PACKAGE FRAMEWORK TARGET
# ================================================
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

	@if test -d $(FRAMEWORK_DIRECTORY)/$(FRAMEWORK_VERSION); then \
		echo "The framework already exists in directory '$(FRAMEWORK_DIRECTORY)/$(FRAMEWORK_VERSION)' so no need to build it."; \
	else \
		$(MAKE); \
	fi

	@echo "Packaging framework in directory '$(PACKAGE_DIRECTORY)'..."
	@mkdir -p $(PACKAGE_DIRECTORY)/lib $(PACKAGE_DIRECTORY)/bin
	@cp -r /usr/lib/qt4/plugins $(PACKAGE_DIRECTORY)/lib/.
	@cp $(FRAMEWORK_DIRECTORY)/$(FRAMEWORK_VERSION)/applications/QEGuiApp/qegui $(PACKAGE_DIRECTORY)/bin/.
	@cp $(FRAMEWORK_DIRECTORY)/$(FRAMEWORK_VERSION)/framework/designer/libQEPlugin.so $(PACKAGE_DIRECTORY)/lib/.
	@cp $(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)/libca.so* $(PACKAGE_DIRECTORY)/lib/.
	@cp $(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)/libCom.so* $(PACKAGE_DIRECTORY)/lib/.
	@cp $(EPICS_BASE)/bin/$(EPICS_HOST_ARCH)/caget $(PACKAGE_DIRECTORY)/bin/.
	@cp $(EPICS_BASE)/bin/$(EPICS_HOST_ARCH)/caput $(PACKAGE_DIRECTORY)/bin/.
	@cp $(EPICS_BASE)/bin/$(EPICS_HOST_ARCH)/camonitor $(PACKAGE_DIRECTORY)/bin/.
	@cp $(EPICS_BASE)/bin/$(EPICS_HOST_ARCH)/cainfo $(PACKAGE_DIRECTORY)/bin/.
	@cd $(PACKAGE_DIRECTORY)/lib/plugins/designer ; ln -s -f ../../libQEPlugin.so
	@chmod u+x $(PACKAGE_DIRECTORY)/bin/qegui
	@echo "#!/bin/bash" > script.tmp
	@echo "`$(LDD) $(PACKAGE_DIRECTORY)/bin/qegui | $(GREP) 'libQt\|libqwt\|libreadline\|libncurses' | $(SED) s/lib.*\>/cp/ | $(SED) s/\(.*\)/$(PACKAGE_DIRECTORY)\\\/lib/`" >> script.tmp
	@chmod u+x script.tmp
	@./script.tmp
	@rm -f script.tmp
	@echo "#!/bin/bash" > $(EXECUTE_FILE)
	@echo "export LD_LIBRARY_PATH=$(RPM_DEPLOY_DIRECTORY)/lib:./lib:$(RPM_DEPLOY_DIRECTORY)/lib/plugins/designer/:./lib/plugins/designer/:\$$LD_LIBRARY_PATH" >> $(EXECUTE_FILE)
	@echo "export QT_PLUGIN_PATH=$(RPM_DEPLOY_DIRECTORY)/lib/plugins:./lib/plugins" >> $(EXECUTE_FILE)
	@echo "$(RPM_DEPLOY_DIRECTORY)/bin/qegui \"\$$@\" || ./bin/qegui \"\$$@\"" >> $(EXECUTE_FILE)
	@chmod u+x $(EXECUTE_FILE)
	@echo "Please note that a file called 'executeQEGui' (located in directory '$(PACKAGE_DIRECTORY)') was created in order to facilitate the launching of ui files. To use it, do the following in a terminal: ./executeQEGui XXX (where XXX is the ui file to be launched)."
	@echo "...done packaging framework!"



# ================================================
#  RPM TARGET
# ================================================
.PHONY: rpm
rpm:
	@echo "Checking conditions for creating RPM of the framework in directory '$(RPM_DIRECTORY)'..."

ifeq ($(shell which $(GREP) 2>/dev/null),)
	@$(error The program '$(GREP)' is not installed or could not be found in your machine (through the environment variable 'PATH'). Please, perform 'yum install grep' in a terminal to install it and/or modify the environment variable 'PATH' to point to this program)
endif

ifeq ($(shell which $(SED) 2>/dev/null),)
	@$(error The program '$(SED)' is not installed or could not be found in your machine (through the environment variable 'PATH'). Please, perform 'yum install sed' in a terminal to install it and/or modify the environment variable 'PATH' to point to this program)
endif

ifeq ($(shell which $(RPM_DIRECTORY) 2>/dev/null),)
	@$(error The program '$(RPM_DIRECTORY)' is not installed or could not be found in your machine (through the environment variable 'PATH'). Please, perform 'yum install rpm-build' in a terminal to install it and/or modify the environment variable 'PATH' to point to this program)
endif

ifeq ($(shell which $(PRELINK) 2>/dev/null),)
	@$(error The program '$(PRELINK)' is not installed or could not be found in your machine (through the environment variable 'PATH'). Please, perform 'yum install prelink' in a terminal to install it and/or modify the environment variable 'PATH' to point to this program)
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
	@cd $(PACKAGE_DIRECTORY) ; ./executeQEGui -v | $(GREP) "Framework version: " | $(SED) 's/Framework\ version:\ //' | $(SED) 's/\ .*//' | $(SED) 's/\([0-9]*\).\([0-9]*\).\([0-9]*\)/Version: \1.\2/' >> ../$(SPEC_FILE) ; cd ..
	@cd $(PACKAGE_DIRECTORY) ; ./executeQEGui -v | $(GREP) "Framework version: " | $(SED) 's/Framework\ version:\ //' | $(SED) 's/\ .*//' | $(SED) 's/\([0-9]*\).\([0-9]*\).\([0-9]*\)/Release: \3%{?dist}/' >> ../$(SPEC_FILE) ; cd ..
	@echo "License: GPLv3" >> $(SPEC_FILE)
	@echo "AutoReqProv: no" >> $(SPEC_FILE)
	@echo "Prefix: /usr/local" >> $(SPEC_FILE)
	@echo "%define _builddir ./rpmbuild/" >> $(SPEC_FILE)
	@echo "" >> $(SPEC_FILE)
	@echo "%description" >> $(SPEC_FILE)
	@echo "This RPM package installs the EPICS Qt Framework in a self-contained directory." >> $(SPEC_FILE)
	@echo "" >> $(SPEC_FILE)
	@echo "%prep" >> $(SPEC_FILE)
	@echo "mkdir -p ./BUILDROOT/%{name}-%{version}-%{release}.%{_arch}$(RPM_DEPLOY_DIRECTORY)" >> $(SPEC_FILE)
	@echo "cp -rf ../$(PACKAGE_DIRECTORY)/* ./BUILDROOT/%{name}-%{version}-%{release}.%{_arch}$(RPM_DEPLOY_DIRECTORY)" >> $(SPEC_FILE)
	@echo "find ./BUILDROOT/%{name}-%{version}-%{release}.%{_arch}$(RPM_DEPLOY_DIRECTORY) -iname '*.so*' | while read filename ; do prelink --undo \"\$${filename}\" || true; done" >> $(SPEC_FILE)
	@echo "" >> $(SPEC_FILE)
	@echo "%files" >> $(SPEC_FILE)
	@echo "%attr(755, root, root) $(RPM_DEPLOY_DIRECTORY)/executeQEGui" >> $(SPEC_FILE)
	@echo "/*" >> $(SPEC_FILE)
	@echo "" >> $(SPEC_FILE)
	@echo "%post" >> $(SPEC_FILE)
	@echo "echo \"The EPICS Qt Framework was installed in '$(RPM_DEPLOY_DIRECTORY)'. Please note that a file called 'executeQEGui' exists in this directory to facilitate the launching of ui files. To use it, do the following in a terminal: ./executeQEGui XXX (where XXX is the ui file to be launched).\"" >> $(SPEC_FILE)
	@echo "" >> $(SPEC_FILE)
	@$(RPM_DIRECTORY) -bb $(SPEC_FILE)
	@echo "Please note that a RPM was created and it is located in './$(RPM_DIRECTORY)/RPMS'. To install this RPM, perform 'rpm -i XXX.rpm' in a terminal (the framework will then be installed in directory '$(RPM_DEPLOY_DIRECTORY)')."



# ================================================
#  UPLOAD RPM TARGET
# ================================================
.PHONY: upload_rpm
upload_rpm:
	@echo "Checking conditions for uploading RPM of the framework in SourceForge download area..."

	@if test -d $(RPM_DIRECTORY); then \
		echo "The RPM already exists in directory '$(RPM_DIRECTORY)' so no need to build it."; \
	else \
		$(MAKE) rpm; \
	fi

	@echo "Please, provide the username of your account in Sourceforge: "; \
	read user_name; \
	echo "Uploading RPM located in directory '$(RPM_DIRECTORY)' in SourceForge download area..."; \
	ssh $$user_name,epicsqt@$(SOURCEFORGE_HOSTNAME) create; \
	scp $(RPM_DIRECTORY)/RPMS/*/epics_qt_framework*.rpm $$user_name,epicsqt@$(SOURCEFORGE_HOSTNAME):$(SOURCEFORGE_DIRECTORY)/RPM/
	@echo "...done uploading RPM in SourceForge download area!"



# ================================================
#  CLEAN ALL TARGET
# ================================================
.PHONY: clean
clean: clean_framework clean_package clean_rpm



# ================================================
#  CLEAN FRAMEWORK TARGET
# ================================================
.PHONY: clean_framework
clean_framework:
	@echo "Cleaning framework located in directory '$(FRAMEWORK_DIRECTORY)'..."
	@rm -rf $(FRAMEWORK_DIRECTORY)
	@echo "...done cleaning framework!"



# ================================================
#  CLEAN PACKAGE TARGET
# ================================================
.PHONY: clean_package
clean_package:
	@echo "Cleaning package located in directory '$(PACKAGE_DIRECTORY)'..."
	@rm -rf $(PACKAGE_DIRECTORY)
	@echo "...done cleaning package!"



# ================================================
#  CLEAN RPM TARGET
# ================================================
.PHONY: clean_rpm
clean_rpm:
	@echo "Cleaning RPM located in directory '$(RPM_DIRECTORY)'..."
	@rm -rf $(RPM_DIRECTORY)
	@echo "...done cleaning RPM!"



