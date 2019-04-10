# NetOnZeroDXC package

A software package for assessment of networks out of time series. This package contains a set of tools that implement the analysis algorithm described in A. Perinelli, D. E. Chiari and L. Ricci, _Correlation in brain networks at different time scale resolution_, Chaos __28__(6):063127, 2018.

## License

This package is free software. It is distributed under the terms of the GNU General Public License (GPL), version 3.0 - see the LICENSE.txt file for details.
This package makes use of the GNU Scientific Libraries, distributed under the terms of GPL (see https://www.gnu.org/software/gsl/ for further details).
This package makes use of the wxWidgets library, distributed under the terms of the "wxWindows Library Licence" (see https://www.wxwidgets.org/about/licence/ for further details).

## Authors

- Alessio Perinelli (1)
- Leonardo Ricci (1,2)

(1) Department of Physics, University of Trento, Trento, Italy  
(2) CIMeC, Center for Mind/Brain Sciences, University of Trento, Rovereto, Italy

alessio.perinelli@unitn.it
leonardo.ricci@unitn.it

If the package turns out to be useful for your research, please cite our paper:
	A. Perinelli, D. E. Chiari and L. Ricci, _Correlation in brain networks at different time scale resolution_, Chaos __28__(6):063127, 2018.

## Package stucture

The package consists of two GUI apps and two command-line programs. See `/docs/manual.pdf` for details on the programs functionalities. All source code is under `/src`.
The two GUI apps require the wxWidgets library to provide a graphic interface. All the programs require the GNU Scientific Libraries to provide random number generation and fast Fourier transform routines.

Details on how to install the package under Linux and Windows can be found in README files within `/setup/Linux` and `/setup/Windows`, respectively. For Windows users, we provide a binary version of the package programs, so that the aforementioned libraries are not necessary unless the user wishes to re-compile the package.

### Programs

- `netOnZeroDXC_analysis`: GUI app to carry out the analysis steps.
- `netOnZeroDXC_merge`: GUI app to merge results from different recordings.
- `netOnZeroDXC_diagram`: command line program to perform the first of the analysis step.
- `netOnZeroDXC_efficiency`: command line program to perform the second analysis step.
