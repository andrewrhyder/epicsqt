#!/usr/bin/python


__author__ = "Ricardo Fernandes"
__email__ = "ricardo.fernandes@synchrotron.org.au"
__copyright__ = "(C) 2013 Australian Synchrotron"
__version__ = "1.5"
__date__ = "2014/MAR/21"
__description__ = "Script to automate the release of a new version of the EPICS Qt Framework"
__status__ = "Production"



# ===========================================================
#  IMPORT PACKAGES
# ===========================================================
import os
import sys
import subprocess
import tempfile



# ===========================================================
#  GLOBAL VARIABLES
# ===========================================================
__SVN_REPOSITORY__ = "https://svn.code.sf.net/p/epicsqt/code"
__SOURCEFORGE_HOSTNAME__ = "shell.sourceforge.net"
__SOURCEFORGE_DIRECTORY__ = "/home/frs/project/epicsqt"
__TEMP_FILE__ = "%s/release.tmp" % tempfile.gettempdir()
__TEMP_DIRECTORY__ = "%s/epicsqt" % tempfile.gettempdir()
__USER_NAME__ = None
__DEBUG__ = False



# ===========================================================
#  GET TAG NAME (IF SPECIFIED)
# ===========================================================
tag_name = None
if len(sys.argv) > 1:
	if sys.argv[1]. upper().startswith("TAG="):
		tag_name = sys.argv[1][4:].strip()



# ===========================================================
#  PRESENT SOME INFO
# ===========================================================
try:
	print
	if tag_name is None:
		print "You are about to release a new version of the EPICS Qt Framework located in the trunk."
		print "Please make sure that you have updated the MAJOR, MINOR, RELEASE and the QE_VERSION_STAGE definitions"
		print "in file 'QEFrameworkVersion.h' and compiled the framework successfully (a build is required as 'qegui'"
		print "is run to dump the version number later in this process). Please, DO NOT commit this file if you have"
		print "changed it (it will be automatically commited later after taging is complete)."

	else:
		print "You are about to release a new version of the EPICS Qt Framework tagged '%s'." % tag_name
		print "Please make sure that you have updated the MAJOR, MINOR, RELEASE and the QE_VERSION_STAGE definitions"
		print "in file 'QEFrameworkVersion.h'."
	print
	while True:
		answer = raw_input("Proceed (y/n): ")
		if answer.upper() == "Y":
			break
		if answer.upper() == "N":
			sys.exit(0)
	print
	path = os.path.dirname(os.path.abspath(sys.argv[0])).split("/")
	base_path = ""
	for i in range(0, len(path) - 2):
		if len(path[i]) > 0:
			base_path = "%s/%s" % (base_path, path[i])



	# ===========================================================
	#  PREPARE NEW RELEASE BY COPYING IT FROM THE TRUNK, A BRANCH OR ANOTHER TAG
	# ===========================================================
	if tag_name is not None:

		while True:
			answer = raw_input("(1) Copy existing tag/branch/trunk to new tag '%s' prior to make changes or (2) release changes in tag '%s'? " % (tag_name, tag_name))
			if answer == "1":
				break
			if answer == "2":
				break
		print
		if answer == "1":
			try:
				while True:
					location = raw_input("Specify the SVN location from where the tag '%s' will be copied from: " % tag_name).strip()
					if len(location) > 0:
						break

				print "Copying SVN location '%s/%s' into SVN tag '%s/tags/%s'..." % (__SVN_REPOSITORY__, location, __SVN_REPOSITORY__, tag_name)
				print
				command = "svn copy --parents %s/%s %s/tags/%s -m \"Prepare for release version %s\"" % (__SVN_REPOSITORY__, location, __SVN_REPOSITORY__, tag_name, tag_name)
				if __DEBUG__:
					print command
				subprocess.call(command, shell = True)
				command = "svn checkout %s/tags/%s %s/tags/%s" % (__SVN_REPOSITORY__, tag_name, base_path, tag_name)
				if __DEBUG__:
					print command
				subprocess.call(command, shell = True)
				print "The new release tagged '%s' can be found in '%s/tags/%s'" % (tag_name, base_path, tag_name)
				print
				sys.exit(0)
			except:
				print "Error when copying SVN '%s/%s' into SVN tag '%s/tags/%s'..." % (__SVN_REPOSITORY__, location, __SVN_REPOSITORY__, tag_name)
				sys.exit(-1)



	# ===========================================================
	#  GET NEW RELEASE VERSION BY RUNNING qegui
	# ===========================================================
	if tag_name is None:
		try:
			executable = "/trunk/applications/QEGuiApp/bin/qegui -v"
			print "Retrieving new release version by running '%s%s'..." % (base_path, executable)
			print
			command = "%s%s | head -2 | tail -1 | sed 's/Framework\ version:\ //' | sed 's/\ .*//' > %s" % (base_path, executable, __TEMP_FILE__)
			if __DEBUG__:
				print command
			subprocess.call(command, shell = True)
			file = open(__TEMP_FILE__, "r")
			new_release_version = file.read()[:-1]
			file.close()
			print "New release version is '%s'." % new_release_version
			print
		except:
			print "Error when retrieving new release version!"
			sys.exit(-1)



	# ===========================================================
	#  GET LAST RELEASE VERSION FROM SVN 'tags' DIRECTORY
	# ===========================================================
	try:
		print "Retrieving last release version from SVN 'tags' directory..."
		print
		if tag_name is None:
			command = "svn list %s/tags | sort -b -g -r | head -1 > %s" % (__SVN_REPOSITORY__, __TEMP_FILE__)
		else:
			command = "svn list %s/tags | sort -b -g -r | head -2 | tail -1 > %s" % (__SVN_REPOSITORY__, __TEMP_FILE__)
		if __DEBUG__:
			print command
		subprocess.call(command, shell = True)
		file = open(__TEMP_FILE__, "r")
		last_release_version = file.read()[:-2]
		file.close()
		print "Last release version is '%s'." % last_release_version
		print
	except:
		print "Error when retrieving last release version from SVN 'tags' directory!"
		sys.exit(-1)



	# ===========================================================
	#  GET REVISION NUMBER AND VERSION OF THE LAST RELEASE FROM SVN 'tags' DIRECTORY
	# ===========================================================
	try:
		print "Retrieving SVN revision number of last release..."
		print
		command = "svn list --verbose %s/tags | sort -b -g -r | head -1 | sed -r 's/ *([0-9]+).+/\\1/' > %s" % (__SVN_REPOSITORY__, __TEMP_FILE__)
		if __DEBUG__:
			print command
		subprocess.call(command, shell = True)
		file = open(__TEMP_FILE__, "r")
		revision_number = file.read()
		file.close()
	except:
		print "Error when retrieving SVN log entries since last release!"
		sys.exit(-1)



	# ===========================================================
	#  DISPLAY SVN LOG ENTRIES SINCE LAST RELEASE
	# ===========================================================
	try:
		command = "svn log -r %d:HEAD %s > %s" % (int(revision_number) + 1, __SVN_REPOSITORY__, __TEMP_FILE__)
		if __DEBUG__:
			print command
		subprocess.call(command, shell = True)
		command = "cat %s" % __TEMP_FILE__
		if __DEBUG__:
			print command
		subprocess.call(command, shell = True)
		print
	except:
		print "Error when displaying SVN log entries!"
		sys.exit(-1)



	# ===========================================================
	#  GET SUMMARY FOR THE NEW RELEASE
	# ===========================================================
	summaryList = []
	summaryList.append("==================================================================================================")
	if tag_name is None:
		summaryList.append("Changes between %s and %s (this includes changes to all branches and tags and may include changes not related to this release)" % (new_release_version, last_release_version))
	else:
		summaryList.append("Changes between %s and %s (this includes changes to all branches and tags and may include changes not related to this release)" % (tag_name, last_release_version))
	summaryList.append("")
	summaryList.append("SUMMARY:")
	print "You can enter the summary one line at a time, or paste multiple lines as long as there are no empty lines."
	summary = raw_input("Summary for this new release: ")
	while len(summary) > 0:
		summaryList.append(summary)
		summary = raw_input("(More summary, or press ENTER to finish summary): ")
	summaryList.append("")
	summaryList.append("DETAIL:")
	print ""



	# ===========================================================
	#  CREATE LOG ENTRIES FILE
	# ===========================================================
	try:
		file = open(__TEMP_FILE__, "r")
		for line in file:
			summaryList.append(line[:-1])
		file.close()
		file = open(__TEMP_FILE__, "w")
		for summary in summaryList:
			file.write(summary + "\n")
		file.close()
	except:
		print "Error when creating log entries file!"
		sys.exit(-1)



	# ===========================================================
	#  COPY FRAMEWORK TRUNK INTO A TEMPORARY DIRECTORY
	# ===========================================================
	try:
		if tag_name is None:
			print "Copying the trunk '%s/trunk' into directory '%s/trunk'..." % (base_path, __TEMP_DIRECTORY__)
			command = "rm -rf %s ; mkdir -p %s ; cp -R %s/trunk %s" % (__TEMP_DIRECTORY__, __TEMP_DIRECTORY__, base_path, __TEMP_DIRECTORY__)
		else:
			print "Copying the tag '%s/tags/%s' into directory '%s/tags/%s'..." % (base_path, tag_name,__TEMP_DIRECTORY__, tag_name)
			command = "rm -rf %s ; mkdir -p %s/tags ; cp -R %s/tags/%s %s/tags" % (__TEMP_DIRECTORY__, __TEMP_DIRECTORY__, base_path, tag_name, __TEMP_DIRECTORY__)
		print
		if __DEBUG__:
			print command
		subprocess.call(command, shell = True)
	except:
		if tag_name is None:
			print "Error when copying 'trunk' of the framework into directory '%s/trunk'!" % __TEMP_DIRECTORY__
		else:
			print "Error when copying tag '%s' of the framework into directory '%s/tags/%s'!" % (tag_name, __TEMP_DIRECTORY__, tag_name)
		sys.exit(-1)



	# ===========================================================
	#  INSERT SVN LOG ENTRIES IN RELEASE NOTES
	# ===========================================================
	try:
		if tag_name is None:
			print "Inserting SVN log entries in file '%s/trunk/releasenotes.txt'..." % __TEMP_DIRECTORY__
			command = "cat %s/trunk/releasenotes.txt >> %s ; cp -f %s %s/trunk/releasenotes.txt" % (__TEMP_DIRECTORY__, __TEMP_FILE__, __TEMP_FILE__, __TEMP_DIRECTORY__)
		else:
			print "Inserting SVN log entries in file '%s/tags/%s/releasenotes.txt'..." % (__TEMP_DIRECTORY__, tag_name)
			command = "cat %s/tags/%s/releasenotes.txt >> %s ; cp -f %s %s/tags/%s/releasenotes.txt" % (__TEMP_DIRECTORY__, tag_name, __TEMP_FILE__, __TEMP_FILE__, __TEMP_DIRECTORY__, tag_name)
		print
		if __DEBUG__:
			print command
		subprocess.call(command, shell = True)
	except:
		if tag_name is None:
			print "Error when inserting SVN log entries in file '%s/trunk/releasenotes.txt'!" % __TEMP_DIRECTORY__
		else:
			print "Error when inserting SVN log entries in file '%s/tags/%s/releasenotes.txt'!" % (__TEMP_DIRECTORY__, tag_name)
		sys.exit(-1)



	# ===========================================================
	#  UPDATE PROJECT NUMBER IN DOXYGEN
	# ===========================================================
	try:
		if tag_name is None:
			print "Updating project number in file '%s/trunk/documentation/source/Doxyfile'..." % __TEMP_DIRECTORY__
			command = "sed 's/PROJECT_NUMBER         =.*/PROJECT_NUMBER         = %s/g' %s/trunk/documentation/source/Doxyfile > %s ; mv %s %s/trunk/documentation/source/Doxyfile" % (new_release_version, __TEMP_DIRECTORY__, __TEMP_FILE__, __TEMP_FILE__, __TEMP_DIRECTORY__)
		else:
			print "Updating project number in file '%s/tags/%s/documentation/source/Doxyfile'..." % (__TEMP_DIRECTORY__, tag_name)
			command = "sed 's/PROJECT_NUMBER         =.*/PROJECT_NUMBER         = %s/g' %s/tags/%s/documentation/source/Doxyfile > %s ; mv %s %s/tags/%s/documentation/source/Doxyfile" % (tag_name, __TEMP_DIRECTORY__, tag_name, __TEMP_FILE__, __TEMP_FILE__, __TEMP_DIRECTORY__, tag_name)
		print
		if __DEBUG__:
			print command
		subprocess.call(command, shell = True)
	except:
		if tag_name is None:
			print "Error when updating project number in file '%s/trunk/Doxyfile'!" % __TEMP_DIRECTORY__
		else:
			print "Error when updating project number in file '%s/tags/%s/Doxyfile'!" % (__TEMP_DIRECTORY__, tag_name)
		sys.exit(-1)



	# ===========================================================
	#  GENERATE DOXYGEN DOCUMENTATION
	# ===========================================================
	try:
		print "Generating Doxygen documentation..."
		print
		if tag_name is None:
			command = "cd %s/trunk/documentation/source ; doxygen Doxyfile 1>/dev/null 2>/dev/null" % __TEMP_DIRECTORY__
		else:
			command = "cd %s/tags/%s/documentation/source ; doxygen Doxyfile 1>/dev/null 2>/dev/null" % (__TEMP_DIRECTORY__, tag_name)
		if __DEBUG__ :
			print command
		subprocess.call(command, shell = True)
	except:
		print "Error when generating Doxygen documentation!"
		sys.exit(-1)



	# ===========================================================
	#  GENERATE PDF FILE
	# ===========================================================
	try:
		print "Generating PDF file..."
		print
		if tag_name is None:
			command = "cd %s/trunk/documentation/latex ; make pdf 1>/dev/null 2>/dev/null" % __TEMP_DIRECTORY__
		else:
			command = "cd %s/tags/%s/documentation/latex ; make pdf 1>/dev/null 2>/dev/null" % (__TEMP_DIRECTORY__, tag_name)
		if __DEBUG__ :
			print command
		subprocess.call(command, shell = True)
	except:
		print "Error when generating PDF file!"
		sys.exit(-1)



	# ===========================================================
	#  COPY PDF FILE
	# ===========================================================
	try:
		if tag_name is None:
			print "Copying PDF file to '%s/trunk/documentation/QE_ReferenceManual.pdf'..." % __TEMP_DIRECTORY__
			command = "cp %s/trunk/documentation/latex/refman.pdf %s/trunk/documentation/QE_ReferenceManual.pdf" % (__TEMP_DIRECTORY__, __TEMP_DIRECTORY__)
		else:
			print "Copying PDF file to '%s/tags/%s/documentation/QE_ReferenceManual.pdf'..." % (__TEMP_DIRECTORY__, tag_name)
			command = "cp %s/tags/%s/documentation/latex/refman.pdf %s/tags/%s/documentation/QE_ReferenceManual.pdf" % (__TEMP_DIRECTORY__, tag_name, __TEMP_DIRECTORY__, tag_name)

		print
		if __DEBUG__:
			print command
		subprocess.call(command, shell = True)
	except:
		print "Error when copying PDF file!"
		sys.exit(-1)



	# ===========================================================
	#  ASK THE USER TO CHECK DOCUMENTATION
	# ===========================================================
	print "Check that the following documentation was generated correctly before proceeding:"
	if tag_name is None:
		print "  - %s/trunk/releasenotes.txt" % __TEMP_DIRECTORY__
		print "  - %s/trunk/documentation/QE_ReferenceManual.pdf" % __TEMP_DIRECTORY__
	else:
		print "  - %s/tags/%s/releasenotes.txt" % (__TEMP_DIRECTORY__, tag_name)
		print "  - %s/tags/%s/documentation/QE_ReferenceManual.pdf" % (__TEMP_DIRECTORY__, tag_name)
	print
	while True:
		answer = raw_input("Proceed (y/n): ")
		if answer.upper() == "Y":
			break
		if answer.upper() == "N":
			sys.exit(0)
	print



	# ===========================================================
	#  ASK THE USER TO CONFIRM COMMIT CHANGES INTO THE SVN REPOSITORY
	# ===========================================================
	print "You are about to commit changes into the SVN repository '%s'." % __SVN_REPOSITORY__
	print
	while True:
		answer = raw_input("Proceed (y/n): ")
		if answer.upper() == "Y":
			break
		if answer.upper() == "N":
			sys.exit(0)
	print



	# ===========================================================
	#  COMMIT TEMPORARY TRUNK/TAG DIRECTORY
	# ===========================================================
	try:
		print "Commiting changes into SVN repository '%s'..." % __SVN_REPOSITORY__
		print
		if tag_name is None:
			command = "svn commit %s/trunk -m \"Prepare for release version %s\"" % (__TEMP_DIRECTORY__, new_release_version)
		else:
			command = "svn commit %s/tags/%s -m \"Release version %s\"" % (__TEMP_DIRECTORY__, tag_name, tag_name)
		if __DEBUG__:
			print command
		subprocess.call(command, shell = True)
		print
	except:
		print "Error when commiting changes into SVN repository '%s'!" % __SVN_REPOSITORY__
		sys.exit(-1)



	# ===========================================================
	#  MAKE A COPY OF TRUNK/TAG DIRECTORY INTO THE TAG DIRECTORY
	# ===========================================================
	if tag_name is None:
		try:
			print "Copying SVN trunk '%s/trunk' into SVN tag '%s/tags/%s'..." % (__SVN_REPOSITORY__, __SVN_REPOSITORY__, new_release_version)
			print
			command = "svn copy --parents %s/trunk %s/tags/%s -m \"Release version %s\"" % (__SVN_REPOSITORY__, __SVN_REPOSITORY__, new_release_version, new_release_version)
			if __DEBUG__:
				print command
			subprocess.call(command, shell = True)
			print
		except:
			print "Error when copying SVN trunk '%s' into SVN tag '%s/tags/%s'..." % (__SVN_REPOSITORY__, __SVN_REPOSITORY__, new_release_version)
			sys.exit(-1)



	# ===========================================================
	#  GET LAST RELEASE VERSION FROM SVN 'tags' DIRECTORY
	# ===========================================================
	if tag_name is None:
		try:
			print "Retrieving version '%s' from SVN 'tags' directory..." % new_release_version
			print
			command = "svn export %s/tags/%s %s/tags/%s" % (__SVN_REPOSITORY__, new_release_version, __TEMP_DIRECTORY__, new_release_version)
			if __DEBUG__:
				print command
			subprocess.call(command, shell = True)
			print
		except:
			print "Error when retrieving version '%s' from SVN 'tags' directory..." % new_release_version
			sys.exit(-1)



	# ===========================================================
	#  CREATE TAR FILE
	# ===========================================================
	try:
		if tag_name is None:
			tar_file = "epicsqt-%s-src.tar.gz" % new_release_version
			command = "rm -f %s/%s ; tar -cvzf %s/%s -C %s/tags %s 1>/dev/null" % (__TEMP_DIRECTORY__, tar_file, __TEMP_DIRECTORY__, tar_file, __TEMP_DIRECTORY__, new_release_version)
		else:
			tar_file = "epicsqt-%s-src.tar.gz" % tag_name
			command = "rm -f %s/%s ; tar -cvzf %s/%s -C %s/tags %s 1>/dev/null" % (__TEMP_DIRECTORY__, tar_file, __TEMP_DIRECTORY__, tar_file, __TEMP_DIRECTORY__, tag_name)
		print "Creating TAR file '%s/%s'..." % (__TEMP_DIRECTORY__, tar_file)
		print
		if __DEBUG__:
			print command
		subprocess.call(command, shell = True)
	except:
		print "Error when creating TAR file '%s/%s'!" % (__TEMP_DIRECTORY__, tar_file)
		sys.exit(-1)



	# ===========================================================
	#  UPLOAD TAR FILE INTO THE SOURCEFORGE DOWNLOAD AREA
	# ===========================================================
	print "You are about to upload the TAR file '%s/%s' into the SourceForge download area." % (__TEMP_DIRECTORY__, tar_file)
	print
	while True:
		answer = raw_input("Proceed (y/n): ")
		if answer.upper() == "Y":
			if __USER_NAME__ is None:
				print
				__USER_NAME__ = raw_input("Please, provide the user name in SourceForge: ")
				print "Creating temporary shell in SourceForge for user '%s'..." % __USER_NAME__
				command = "ssh %s,epicsqt@%s create" % (__USER_NAME__, __SOURCEFORGE_HOSTNAME__)
				if __DEBUG__:
					print command
				subprocess.call(command, shell = True)
			print
			try:
				print "Uploading TAR file '%s/%s' into the SourceForge download area..." % (__TEMP_DIRECTORY__, tar_file)
				print
				command = "scp %s/%s %s,epicsqt@%s:%s/ 1>/dev/null" % (__TEMP_DIRECTORY__, tar_file, __USER_NAME__, __SOURCEFORGE_HOSTNAME__, __SOURCEFORGE_DIRECTORY__)
				if __DEBUG__:
					print command
				subprocess.call(command, shell = True)
			except:
				print "Error when uploading TAR file '%s/%s' into the SourceForge download area!" % (__TEMP_DIRECTORY__, tar_file)
				sys.exit(-1)
			break
		if answer.upper() == "N":
			break
	print



	# ===========================================================
	#  UPLOAD DOCUMENTATION INTO THE SOURCEFORGE DOWNLOAD AREA
	# ===========================================================
	print "You are about to upload the documentation into the SourceForge download area."
	print
	while True:
		answer = raw_input("Proceed (y/n): ")
		if answer.upper() == "Y":
			if __USER_NAME__ is None:
				print
				__USER_NAME__ = raw_input("Please, provide the user name in SourceForge: ")
				print "Creating temporary shell in SourceForge for user '%s'..." % __USER_NAME__
				command = "ssh %s,epicsqt@%s create" % (__USER_NAME__, __SOURCEFORGE_HOSTNAME__)
				if __DEBUG__:
					print command
				subprocess.call(command, shell = True)
			print
			try:
				print "Uploading documentation into the SourceForge download area..."
				print
				if tag_name is None:
					command = "scp %s/trunk/releasenotes.txt %s/trunk/documentation/QE_ReferenceManual.pdf %s,epicsqt@%s:%s/documentation/ 1>/dev/null" % (__TEMP_DIRECTORY__, __TEMP_DIRECTORY__, __USER_NAME__, __SOURCEFORGE_HOSTNAME__, __SOURCEFORGE_DIRECTORY__)
				else:
					command = "scp %s/tags/%s/releasenotes.txt %s/tags/%s/documentation/QE_ReferenceManual.pdf %s,epicsqt@%s:%s/documentation/ 1>/dev/null" % (__TEMP_DIRECTORY__, tag_name, __TEMP_DIRECTORY__, tag_name, __USER_NAME__, __SOURCEFORGE_HOSTNAME__, __SOURCEFORGE_DIRECTORY__)
				if __DEBUG__:
					print command
				subprocess.call(command, shell = True)
			except:
				print "Error when uploading documentation into the SourceForge download area!"
				sys.exit(-1)
			break
		if answer.upper() == "N":
			break
	print


except KeyboardInterrupt:
	print ""
	print "Release process interrupted by the user."
	sys.exit(0)


print "Please do not forget to update the MAJOR, MINOR, RELEASE and the QE_VERSION_STAGE definitions"
print "in file 'QEFrameworkVersion.h' since a new cycle of development has started."
print
print "Release done!"
print

