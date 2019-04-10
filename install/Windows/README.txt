###############################################
### SETUP INSTRUCTIONS for Windows (64-bit) ###
###############################################

Inside the folder /install/Windows you can find "netOnZeroDXC_setup.exe", an installer
for the netOnZeroDXC package. Launch "netOnZeroDXC_setup.exe" to start the setup.

The installer will guide you through few steps. You can choose where to
install the software, and whether to add start menu entries or not.
When the software is installed, the installation folder is added to path,
meaning that the programs can be run from anywhere inside the system.

As a last step, the installer will launch the setup of a necessary
library, the Visual C++ 2017 Redistributable Runtime Library. This
library is required to run the program, and is provided freely by Microsoft.
The library installer will also be copied in the installation folder.
If the library is already installed, the setup will apparently give an error:
just close the error message and carry on with the package setup.

You can uninstall the package at any time by using the uninstall executable
provided in the installation folder, or from the Control Panel as for other
programs.

The program was tested in a Windows 10 and a Windows 7 environment.
Other versions of Windows might not be fully supported.
The package executables are compiled for 64-bit architectures. To run the
package on a 32-bit machine, re-compilation from source is required.


###############
### LICENSE ###
###############

This package is licensed under the GNU General Public License v3. You can find
a copy of the license in the root folder of this package.
The license file is also copied in the installation folder.
The GSL (GNU Scientific Library) and the wxWidgets library
required by the package programs are statically linked in the provided
precompiled executables.

- GSL is licensed under the terms of the GNU General Public License (GPL), see
https://www.gnu.org/software/gsl/

- wxWidgets is licensed under wxWindows Library Licence, see
https://www.wxwidgets.org/about/licence/
