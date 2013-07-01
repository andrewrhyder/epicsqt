#!/usr/bin/python


__author__ = "Ricardo Fernandes"
__email__ = "ricardo.fernandes@synchrotron.org.au"
__copyright__ = "(C) 2013 Australian Synchrotron"
__version__ = "1.2"
__date__ = "2013/JUL/01"
__description__ = "Script to automate the release of a new version of the EPICS Qt Framework"
__status__ = "Production"



# ===========================================================
#  IMPORT PACKAGE
# ===========================================================
import os
import sys
import subprocess



# ===========================================================
#  GLOBAL VARIABLES
# ===========================================================
__SVN_REPOSITORY__ = "https://svn.code.sf.net/p/epicsqt/code"
__SOURCEFORGE_HOSTNAME__ = "shell.sourceforge.net"
__SOURCEFORGE_DIRECTORY__ = "/home/frs/project/epicsqt"
__TEMP_FILE__ = "/tmp/release.tmp"
__TEMP_DIRECTORY__ = '/tmp/epicsqt'
__USER_NAME__ = None
__DEBUG__ = False



# ===========================================================
#  MAIN SCRIPT
# ===========================================================
if __name__ == "__main__":


	try:
		print
		print "You are about to release a new version of the EPICS Qt Framework. Please make sure that you have updated"
		print "the MAJOR, MINOR, RELEASE and the QE_VERSION_STAGE definitions in file 'QEFrameworkVersion.h' and compiled"
		print "the framework successfully (a build is required as 'qegui' is run to dump the version number later in this"
		print "process). Please, DO NOT commit this file if you have changed it (it will be automatically commited later"
		print "after taging is complete)."
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
		#  GET NEW RELEASE VERSION BY RUNNING qegui
		# ===========================================================
		try:
			executable = "/trunk/applications/QEGuiApp/qegui -v"
			print "Retrieving new release version by running '%s%s'..." % (base_path, executable)
			print
			command = "%s%s | tail -1 | sed 's/Framework\ version:\ //' | sed 's/\ .*//' > %s" % (base_path, executable, __TEMP_FILE__)
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
			command = "svn list %s/tags | sort -b -r | head -1 > %s" % (__SVN_REPOSITORY__, __TEMP_FILE__)
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
			command = "svn list --verbose %s/tags | sort -b -r | head -1 | sed -r 's/ *([0-9]+).+/\\1/' > %s" % (__SVN_REPOSITORY__, __TEMP_FILE__)
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
		summaryList.append("Changes between %s and %s" % (new_release_version, last_release_version))
		summaryList.append("")
		summaryList.append("SUMMARY:")
		summary = raw_input("Summary for this new release   : ")
		while len(summary) > 0:
			summaryList.append(summary)
			summary = raw_input("(Press ENTER to finish summary): ")
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
			print "Copying the trunk '%s/trunk' into directory '%s/trunk'..." % (base_path, __TEMP_DIRECTORY__)
			print
			command = "rm -rf %s ; mkdir -p %s ; cp -R %s/trunk %s" % (__TEMP_DIRECTORY__, __TEMP_DIRECTORY__, base_path, __TEMP_DIRECTORY__)
			if __DEBUG__:
				print command
			subprocess.call(command, shell = True)
		except:
			print "Error when copying 'trunk' of the framework into directory '%s/trunk'!" % __TEMP_DIRECTORY__
			sys.exit(-1)



		# ===========================================================
		#  INSERT SVN LOG ENTRIES IN RELEASE NOTES
		# ===========================================================
		try:
			print "Inserting SVN log entries in file '%s/trunk/releasenotes.txt'..." % __TEMP_DIRECTORY__
			print
			command = "cat %s/trunk/releasenotes.txt >> %s ; cp -f %s %s/trunk/releasenotes.txt" % (__TEMP_DIRECTORY__, __TEMP_FILE__, __TEMP_FILE__, __TEMP_DIRECTORY__)
			if __DEBUG__:
				print command
			subprocess.call(command, shell = True)
		except:
			print "Error when inserting SVN log entries in file '%s/trunk/releasenotes.txt'!" % __TEMP_DIRECTORY__
			sys.exit(-1)



		# ===========================================================
		#  UPDATE PROJECT NUMBER IN DOXYGEN
		# ===========================================================
		try:
			print "Updating project number in file '%s/trunk/documentation/source/Doxyfile'..." % __TEMP_DIRECTORY__
			print
			command = "sed 's/PROJECT_NUMBER         =.*/PROJECT_NUMBER         = %s/g' %s/trunk/documentation/source/Doxyfile > %s ; mv %s %s/trunk/documentation/source/Doxyfile" % (new_release_version, __TEMP_DIRECTORY__, __TEMP_FILE__, __TEMP_FILE__, __TEMP_DIRECTORY__)
			if __DEBUG__:
				print command
			subprocess.call(command, shell = True)
		except:
			print "Error when updating project number in file '%s/trunk/Doxyfile'!" % __TEMP_DIRECTORY__
			sys.exit(-1)



		# ===========================================================
		#  GENERATE DOXYGEN DOCUMENTATION
		# ===========================================================
		try:
			print "Generating Doxygen documentation..."
			print
			command = "cd %s/trunk/documentation/source ; doxygen Doxyfile 1>/dev/null 2>/dev/null" % __TEMP_DIRECTORY__
			if __DEBUG__:
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
			command = "cd %s/trunk/documentation/latex ; make pdf 1>/dev/null 2>/dev/null" % __TEMP_DIRECTORY__
			if __DEBUG__:
				print command
			subprocess.call(command, shell = True)
		except:
			print "Error when generating PDF file!"
			sys.exit(-1)



		# ===========================================================
		#  COPY PDF FILE
		# ===========================================================
		try:
			print "Copying PDF file to '%s/trunk/documentation/QE_ReferenceManual.pdf'..." % __TEMP_DIRECTORY__
			command = "cp %s/trunk/documentation/latex/refman.pdf %s/trunk/documentation/QE_ReferenceManual.pdf" % (__TEMP_DIRECTORY__, __TEMP_DIRECTORY__)
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
		print "  - %s/trunk/releasenotes.txt" % __TEMP_DIRECTORY__
		print "  - %s/trunk/documentation/QE_ReferenceManual.pdf" % __TEMP_DIRECTORY__
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
		#  COMMIT TEMPORARY TRUNK DIRECTORY
		# ===========================================================
		try:
			print "Commiting changes into SVN repository '%s'..." % __SVN_REPOSITORY__
			print
			command = "svn commit %s/trunk -m \"Release version %s\"" % (__TEMP_DIRECTORY__, new_release_version)
			if __DEBUG__:
				print command
			subprocess.call(command, shell = True)
			print
		except:
			print "Error when commiting changes into SVN repository '%s'!" % __SVN_REPOSITORY__
			sys.exit(-1)



		# ===========================================================
		#  MAKE A COPY OF TRUNK DIRECTORY INTO THE TAG DIRECTORY
		# ===========================================================
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
			tar_file = "epicsqt-%s-src.tar.gz" % new_release_version
			print "Creating TAR file '%s/%s'..." % (__TEMP_DIRECTORY__, tar_file)
			command = "rm -f %s/%s ; tar -cvzf %s/%s -C %s/tags/%s . 1>/dev/null" % (__TEMP_DIRECTORY__, tar_file, __TEMP_DIRECTORY__, tar_file, __TEMP_DIRECTORY__, new_release_version)
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
				try:
					print
					print "Uploading TAR file '%s/%s' into the SourceForge download area..." % (__TEMP_DIRECTORY__, tar_file)
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
				try:
					print
					print "Uploading documentation into the SourceForge download area..."
					command = "scp %s/trunk/releasenotes.txt %s/trunk/documentation/QE_ReferenceManual.pdf %s,epicsqt@%s:%s/documentation/ 1>/dev/null" % (__TEMP_DIRECTORY__, __TEMP_DIRECTORY__, __USER_NAME__, __SOURCEFORGE_HOSTNAME__, __SOURCEFORGE_DIRECTORY__)
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



		# ===========================================================
		#  CREATE RPM FILE
		# ===========================================================
		try:
			rpm_file = "epics_qt_framework-%s.%s-%s.i386.rpm" % (new_release_version.split(".")[0], new_release_version.split(".")[1], new_release_version.split(".")[2])
			print "Creating RPM file '%s/trunk/resources/rpmbuild/RPMS/i386/%s'..." % (__TEMP_DIRECTORY__, rpm_file)
			command = "cd %s/trunk/resources ; make rpm 1>/dev/null" % __TEMP_DIRECTORY__
			print
			if __DEBUG__:
				print command
			subprocess.call(command, shell = True)
		except:
			print "Error when creating RPM file '%s/trunk/resources/rpmbuild/RPMS/i386/%s'..." % (__TEMP_DIRECTORY__, rpm_file)
			sys.exit(-1)



		# ===========================================================
		#  UPLOAD RPM FILE INTO THE SOURCEFORGE DOWNLOAD AREA
		# ===========================================================
		print "You are about to upload the RPM file '%s/trunk/resources/rpmbuild/RPMS/i386/%s' into the SourceForge download area." % (__TEMP_DIRECTORY__, rpm_file)
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
				try:
					print
					print "Uploading RPM file '%s/trunk/resources/rpmbuild/RPMS/i386/%s' into the SourceForge download area..." % (__TEMP_DIRECTORY__, rpm_file)
					command = "scp %s/trunk/resources/rpmbuild/RPMS/i386/%s %s,epicsqt@%s:%s/RPM/ 1>/dev/null" % (__TEMP_DIRECTORY__, rpm_file, __USER_NAME__, __SOURCEFORGE_HOSTNAME__, __SOURCEFORGE_DIRECTORY__)
					if __DEBUG__:
						print command
					subprocess.call(command, shell = True)
				except:
					print "Error when uploading RPM file '%s/trunk/resources/rpmbuild/RPMS/i386/%s' into the SourceForge download area!" % (__TEMP_DIRECTORY__, rpm_file)
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


