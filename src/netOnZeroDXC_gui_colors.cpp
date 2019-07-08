// --------------------------------------------------------------------------
//
// This file is part of the NetOnZeroDXC software package.
//
// Version 1.1 - July 2019
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

#include <cmath>

#ifndef INCLUDED_COLORS
	#include "netOnZeroDXC_gui_colors.hpp"
	#define INCLUDED_COLORS
#endif

wxColour netOnZeroDXC_color_palette (int i, int nr_levels)
{
	return wxColour(255.0*(nr_levels - i + 1)/nr_levels, 255*sin((3.14159*(nr_levels - i + 1))/nr_levels), 255*cos((3.14159*(nr_levels - i + 1))/(2.0*nr_levels)));
}

wxColour netOnZeroDXC_color_palette (double level)
{
	if (level == 0.0) {
		return wxColour(0, 0, 0);
	} else if (level < 0) {
		return wxColour(255, 255, 255);
	} else {
		return wxColour(255.0 * level, 255.0 * sin(3.14159 * level), 255.0 * cos(3.14159 * level / 2.0));
	}
}
