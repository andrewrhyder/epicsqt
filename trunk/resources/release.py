#!/usr/bin/python


__author__ = "Ricardo Fernandes"
__email__ = "ricardo.fernandes@synchrotron.org.au"
__copyright__ = "(C) 2013 Australian Synchrotron"
__version__ = "1.0"
__date__ = "2013/APR/30"
__description__ = "Script to automate the release of a new version of the EPICS Qt Framework"
__status__ = "Development"



import sys
import subprocess


__SVN_REPOSITORY__ = "https://epicsqt.svn.sourceforge.net/svnroot/epicsqt"

__TEMP_FILE__ = "/tmp/release.tmp"

__TEMP_DIRECTORY__ = '/tmp/epicsqt'



if __name__ == "__main__":

	print "You are about to release a new version of the EPICS Qt Framework. Please make sure that"
	print "you have updated the MAJOR, MINOR and/or RELEASE numbers in file 'QEFrameworkVersion.h'"
	print "and compiled the framework successfully."
	print ""


	try:
		# get summary
		summary_list = []
		while True:
			summary = raw_input("Summary for this new release: ")
			if len(summary) == 0:
				break
			else:
				summary_list.append(summary)
		print ""
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)


	# get version of the new release
	try:
		executable = "../applications/QEGuiApp/QEGui -v"
		print "Retrieving version of the new release by executing '%s'..." % executable
		command = "%s | grep \"Framework version: \" | sed 's/Framework\ version:\ //' | sed 's/\ .*//' > %s" % (executable, __TEMP_FILE__)
		#print command
		subprocess.call(command, shell = True)
		file = open(__TEMP_FILE__, "r")
		new_release_version = file.read()[:-1]
		file.close()
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)
	except:
		print "Error when retrieving version of the new release!"
		sys.exit(-1)


	print "New release version is '%s'." % new_release_version


	# get version of the last release
	try:
		print "Retrieving version of the last release from the SVN 'tags' directory..."
		command = "svn list %s/tags | sort -V | tail -2 | head -1 > %s" % (__SVN_REPOSITORY__, __TEMP_FILE__)
		#print command
		subprocess.call(command, shell = True)
		file = open(__TEMP_FILE__, "r")
		last_release_version = file.read()[:-2]
		file.close()
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)
	except:
		print "Error when retrieving version of the last release from the SVN 'tag' directory!"
		sys.exit(-1)


	print "Last release version is '%s'." % last_release_version


	# get SVN log entried since last release
	try:
		print "Retrieving SVN log entries since last release..."
		command = "svn list --verbose %s/tags | sort -V | tail -2 | head -1 | sed -r 's/ *([0-9]+).+/\\1/' > %s" % (__SVN_REPOSITORY__, __TEMP_FILE__)
		#print command
		subprocess.call(command, shell = True)
		file = open(__TEMP_FILE__, "r")
		revision_number = file.read()
		file.close()
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)
	except:
		print "Error when retrieving SVN log entries since last release!"
		sys.exit(-1)


	# get SVN log entries since revision number of the last release
	try:
		file = open(__TEMP_FILE__, "w")
		file.write("==================================================================================================\n")
		file.write("Changes between %s and %s\n" % (new_release_version, last_release_version))
		file.write("\n")
		file.write("SUMMARY:\n")
		for summary in summary_list:
			file.write("%s\n" % summary)
		file.write("\n")
		file.write("DETAIL:\n")
		file.close()
		command = "svn log -r %d:HEAD %s >> %s" % (int(revision_number) + 1, __SVN_REPOSITORY__, __TEMP_FILE__)
		#print command
		subprocess.call(command, shell = True)
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)
	except:
		print "Error when retrieving SVN log entries since last release!"
		sys.exit(-1)


	# make copy of the trunk
	try:
		print "Making copy of the 'trunk' of the framework into directory '%s'..." % __TEMP_DIRECTORY__
		command = "rm -rf %s ; mkdir %s ; cp -R ../../trunk %s ; find %s -type d -name .svn | xargs rm -rf" % (__TEMP_DIRECTORY__, __TEMP_DIRECTORY__, __TEMP_DIRECTORY__, __TEMP_DIRECTORY__)
		#print command
		subprocess.call(command, shell = True)
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)
	except:
		print "Error when copying 'trunk' of the framework into directory '%s'!" % __TEMP_DIRECTORY__
		sys.exit(-1)


	# insert SVN log entries in release notes
	try:
		print "Inserting SVN log entries in file '%s/trunk/releasenotes.txt'..." % __TEMP_DIRECTORY__
		command = "cat %s/trunk/releasenotes.txt >> %s ; cp -f %s %s/trunk/releasenotes.txt" % (__TEMP_DIRECTORY__, __TEMP_FILE__, __TEMP_FILE__, __TEMP_DIRECTORY__)
		#print command
		subprocess.call(command, shell = True)
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)
	except:
		print "Error when inserting SVN log entries in file '%s/trunk/releasenotes.txt'!" % __TEMP_DIRECTORY__
		sys.exit(-1)



	# update project number in Doxyfile
	try:
		print "Updating project number in file '%s/trunk/documentation/source/Doxyfile'..." % __TEMP_DIRECTORY__
		command = "sed 's/PROJECT_NUMBER         =.*/PROJECT_NUMBER         = %s/g' %s/trunk/documentation/source/Doxyfile > %s ; mv %s %s/trunk/documentation/source/Doxyfile" % (new_release_version, __TEMP_DIRECTORY__, __TEMP_FILE__, __TEMP_FILE__, __TEMP_DIRECTORY__)
		#print command
		subprocess.call(command, shell = True)
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)
	except:
		print "Error when updating project number in file '%s/trunk/Doxyfile'!" % __TEMP_DIRECTORY__
		sys.exit(-1)


	# generate Doxygen documentation
	try:
		print "Generating Doxygen documentation..."
		command = "cd %s/trunk/documentation/source ; doxygen Doxyfile 1>/dev/null 2>/dev/null" % __TEMP_DIRECTORY__
		#print command
		subprocess.call(command, shell = True)
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)
	except:
		print "Error when generating Doxygen documentation!"
		sys.exit(-1)


	# generate PDF file
	try:
		print "Generating PDF file..."
		command = "cd %s/trunk/documentation/latex ; make pdf 1>/dev/null 2>/dev/null" % __TEMP_DIRECTORY__
		#print command
		subprocess.call(command, shell = True)
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)
	except:
		print "Error when generating PDF file!"
		sys.exit(-1)


	# copy reference manual PDF file
	try:
		print "Copy reference manual PDF file..."
		command = "cp %s/trunk/documentation/latex/refman.pdf %s/trunk/documentation/QE_ReferenceManual.pdf" % (__TEMP_DIRECTORY__, __TEMP_DIRECTORY__)
		#print command
		subprocess.call(command, shell = True)
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)
	except:
		print "Error when copying reference manual PDF file!"
		sys.exit(-1)


	# ask the user to confirm commit of this new release into the SVN 'tags' repository
	try:
		print ""
		print "You are about to commit/upload new release '%s' into SVN 'tags' directory." % new_release_version
		print "Please check that the documentation was generated correctly before proceeding."
		while True:
			if raw_input("Proceed with commit (y/n): ").upper() == "Y":
				break
			if raw_input("Proceed with commit (y/n): ").upper() == "N":
				print "Release process interrupted by the user."
				sys.exit(0)
		print ""
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)


	# move new release from temporary trunk to tags
	try:
		print "Move new release from '%s/trunk' to '../../tags/%s'..." % (__TEMP_DIRECTORY__, new_release_version)
		command = "mkdir -p ../../tags ; mv %s/trunk ../../tags/%s" % (__TEMP_DIRECTORY__, new_release_version)
		#print command
		subprocess.call(command, shell = True)
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)
	except:
		print "Error when moving new release from '%s/trunk' to '../../tags/%s'..." % (__TEMP_DIRECTORY__, new_release_version)
		sys.exit(-1)


	# commit new release into SVN 'tags' directory
	try:
		print "Commiting new release '%s' into SVN 'tags' directory..." % new_release_version
		command = "svn add ../../tags ; svn commit ../../tags/%s -m \"Release version %s\"" % (new_release_version, new_release_version)
		print command
		subprocess.call(command, shell = True)
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)
	except:
		print "Error when commiting new release '%s' into SVN 'tags' directory..." % new_release_version
		sys.exit(-1)




