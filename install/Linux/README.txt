####################################
### SETUP INSTRUCTIONS for Linux ###
####################################

To install the package on Linux distributions, it is necessary to compile the
component programs from source. A C++ compiler is required. The package
requires some libraries in order to properly compile:
	GSL (GNU Scientific Library, https://www.gnu.org/software/gsl/)
	wxWidgets library (https://www.wxwidgets.org/)

Both libraries can be either installed as packages for the target operating
system version, or can be compiled from source. Here we only present the former
path. To compile the libraries on your own, please refer to the official
websites.

############################
### INSTALLING LIBRARIES ###
############################

On Debian/Ubuntu distributions (commands should be run as root):

- Install the gsl binaries and development libraries:
	sudo apt install gsl-bin
	sudo apt install libgsl-dev

- Install the wxWidgets libraries:
	sudo apt install libgtk-3-dev
	sudo apt install libwxgtk3.0-dev
Alternatively, if you want to (or have to) install a more recent 
	versions of the library, please check out the official website.


On Fedora/CentOS/RedHat distributions:
	(commands should be run as root; these commands  were tested on 
	CentOS 7. Things might be different for different operating
	systems and versions):

- Install development tools to get a C compiler and Make (these
	packages may come already installed with your OS):
	sudo yum group install "Development Tools"

- Install the gsl development libraries:
	sudo yum install gsl-devel.x86_64

- RPM packages for wxWidgets are available for download on
	https://wiki.codelite.org/pmwiki.php/Main/WxWidgets31Binaries
	at the end of the page.
- Alternatively, some package in repositories exist. However, at least
	on CentOS, this requires adding the EPEL repository. You can
	also compile the library on your own (refer to the official
	website).


#############################
### COMPILING THE PACKAGE ###
#############################

When the libraries are installed, it is possible to compile the package.
To test whether the libraries are correctly set up, check the outcome of the
following commands:
	gsl-config --version
	wx-config --version
If you get the version numbers, everything is correctly set up.

Now go to the
	/install/Linux/build
directory, and compile with
	make
You can also compile only part of the package: each program can be separately
compiled with
	make <programname>
Executable files will be created in the current directory. They can be made
available to the whole system by using
	make binlink
Links will be created in /usr/bin/, targeting the current directory. If you
rather want to copy the executable themselves to the /usr/bin/ directory, use
	make bincopy
If you do this last operation, you can move or delete the package directory,
and the executable will still be working.


###############
### LICENSE ###
###############

This package is licensed under the GNU General Public License v3. You can find
a copy of the license in the root directory of this package.
