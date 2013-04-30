#!/usr/bin/python


__author__ = "Ricardo Fernandes"
__email__ = "ricardo.fernandes@synchrotron.org.au"
__copyright__ = "(C) 2013 Australian Synchrotron"
__version__ = "1.0"
__date__ = "2013/APR/30"
__description__ = "Script to automate the release of a new version of the EPICS Qt Framework"
__status__ = "Production"



import os
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


	path =  os.path.dirname(os.path.abspath(sys.argv[0])).split("/")
	base_path = ""
	for i in range(0, len(path) - 2):
		if len(path[i]) > 0:
			base_path = "%s/%s" % (base_path, path[i])


	try:
		# get summary
		summary_list = []
		summary = raw_input("Summary for this new release   : ")
		while len(summary) > 0:
			summary_list.append(summary)
			summary = raw_input("(Press ENTER to finish summary): ")
		print ""
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)


	# get version of the new release
	try:
		executable = "/trunk/applications/QEGuiApp/QEGui -v"
		print "Retrieving version of the new release by executing '%s%s'..." % (base_path, executable)
		command = "%s%s | grep \"Framework version: \" | sed 's/Framework\ version:\ //' | sed 's/\ .*//' > %s" % (base_path, executable, __TEMP_FILE__)
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
		print "Retrieving version of the last release from SVN 'tags' directory..."
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
		print "Error when retrieving version of the last release from SVN 'tag' directory!"
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
		print "Making copy of the framework trunk '%s/trunk' into directory '%s/trunk'..." % (base_path, __TEMP_DIRECTORY__)
		command = "rm -rf %s ; mkdir %s ; cd %s/trunk ; make clean ; cp -R %s/trunk %s ; find %s -type d -name .svn | xargs rm -rf" % (__TEMP_DIRECTORY__, __TEMP_DIRECTORY__, base_path, base_path, __TEMP_DIRECTORY__, __TEMP_DIRECTORY__)
		#print command
		subprocess.call(command, shell = True)
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)
	except:
		print "Error when copying 'trunk' of the framework into directory '%s/trunk'!" % __TEMP_DIRECTORY__
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
		print "Copy reference manual PDF file to '%s/trunk/documentation/QE_ReferenceManual.pdf'..." % __TEMP_DIRECTORY__
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
			answer = raw_input("Proceed with commit (y/n): ")
			if answer.upper() == "Y":
				break
			if answer.upper() == "N":
				print "Release process interrupted by the user."
				sys.exit(0)
		print ""
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)


	# move new release from temporary trunk to tags
	try:
		print "Move new release from '%s/trunk' to '%s/tags/%s'..." % (__TEMP_DIRECTORY__, base_path, new_release_version)
		command = "mkdir -p %s/tags ; rm -rf %s/tags/%s ; mv %s/trunk %s/tags/%s" % (base_path, base_path, new_release_version, __TEMP_DIRECTORY__, base_path, new_release_version)
		#print command
		subprocess.call(command, shell = True)
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)
	except:
		print "Error when moving new release from '%s/trunk' to '%s/tags/%s'!" % (__TEMP_DIRECTORY__, base_path, new_release_version)
		sys.exit(-1)


	# commit new release into SVN 'tags' directory
	try:
		print "Commiting new release '%s' into SVN 'tags' directory..." % new_release_version
		command = "svn add %s/tags 1>/dev/null; svn commit %s/tags/%s -m \"Release version %s\" 1>/dev/null" % (base_path, base_path, new_release_version, new_release_version)
		#print command
		subprocess.call(command, shell = True)
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)
	except:
		print "Error when commiting new release '%s' into SVN 'tags' directory!" % new_release_version
		sys.exit(-1)


	# create tar file
	try:
		tar_file = "epicsqt-%s-src.tar.gz" % new_release_version
		print "Creating tar file '%s/trunk/resources/%s'..." % (base_path, tar_file)
		command = "rm -f %s/trunk/resources/%s ; tar -cvzf %s/trunk/resources/%s -C %s/tags/%s . 1>/dev/null" % (base_path, tar_file, base_path, tar_file, base_path, new_release_version)
		#print command
		subprocess.call(command, shell = True)
		print "Please, upload tar file '%s/trunk/resources/%s' into the EPICS Qt Framework SourceForge download area." % (base_path, tar_file)
	except KeyboardInterrupt:
		print ""
		print "Release process interrupted by the user."
		sys.exit(0)
	except:
		print "Error when creating tar file '%s/trunk/resources/%s'!" % (base_path, tar_file)
		sys.exit(-1)


	print ""
	print "Release done!"


