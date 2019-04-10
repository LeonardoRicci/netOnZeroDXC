// --------------------------------------------------------------------------
//
// This file is part of the NetOnZeroDXC software package.
//
// Version 1.0 - April 2019
//
//
// The NetOnZeroDXC package is free software; you can use it, redistribute it,
// and/or modify it under the terms of the GNU General Public License
// version 3 as published by the Free Software Foundation. The full text
// of the license can be found in the file LICENSE.txt at the top level of
// the package distribution.
//
// Authors:
//		Alessio Perinelli and Leonardo Ricci
//		Department of Physics, University of Trento
//		I-38123 Trento, Italy
//		alessio.perinelli@unitn.it
//		leonardo.ricci@unitn.it
//		https://github.com/LeonardoRicci/netOnZeroDXC
//
//
// If you use the NetOnZeroDXC package for your analyses, please cite:
//
//	A. Perinelli, D. E. Chiari and L. Ricci,
//	"Correlation in brain networks at different time scale resolution".
//	Chaos 28 (6):063127, 2018
//
// --------------------------------------------------------------------------

#ifndef INCLUDED_PAIR
	#include "netOnZeroDXC_pair.hpp"
	#define INCLUDED_PAIR
#endif

int netOnZeroDXC_load_multi_sequences(std::vector < std::vector <double> > &, std::vector <std::string> &, wxArrayString &, char, char, int);
int netOnZeroDXC_load_multi_diagrams(std::vector < std::vector < std::vector <double> > > &, std::vector <PairOfLabels> &, wxArrayString &, char, char);
int netOnZeroDXC_load_multi_efficiencies(std::vector < std::vector <double> > &, std::vector <PairOfLabels> &, std::vector <double> &, wxArrayString &, char, char);
