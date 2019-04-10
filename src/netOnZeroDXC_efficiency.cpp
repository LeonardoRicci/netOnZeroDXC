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

#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "omp.h"

#ifndef INCLUDED_ALGORITHM
	#include "netOnZeroDXC_algorithm.hpp"
	#define INCLUDED_ALGORITHM
#endif
#ifndef INCLUDED_IOFUNCTIONS
	#include "netOnZeroDXC_io.hpp"
	#define INCLUDED_IOFUNCTIONS
#endif

void netOnZeroDXC_eff_help (char *);
int netOnZeroDXC_eff_parse_options (int, char **, bool &, bool &, double &, double &, std::string &, std::string &, char &);
int netOnZeroDXC_eff_check_diagram (const std::vector < std::vector <double> > &);

int main(int argc, char *argv[]) {

	bool	read_from_file = false;
	bool	write_to_file = false;
	double	threshold_significance = -1.0;
	double	window_basewidth = 1.0;
	char	separator_char = 't';
	std::string	selected_input_filename;
	std::string	selected_output_filename;

	int error;
	error = netOnZeroDXC_eff_parse_options (argc, argv, read_from_file, write_to_file, threshold_significance, window_basewidth, selected_input_filename,
					selected_output_filename, separator_char);
	if (error)
		exit(1);

	std::vector < std::vector <double> > 	loaded_diagram;

	if (read_from_file) {
		error = netOnZeroDXC_load_single_table(loaded_diagram, selected_input_filename, separator_char);
		if (error == 2) {
			std::cerr << "ERROR: cannot read the selected file '" << selected_input_filename << "'.\n";
			exit(1);
		}
		if (error == 3) {
			std::cerr << "ERROR: inconsistent sizes of diagram rows.\n";
			exit(1);
		}
	} else {
		netOnZeroDXC_load_stdin(loaded_diagram, separator_char);
	}
	error = netOnZeroDXC_eff_check_diagram(loaded_diagram);
	if (error)
		exit(1);


	std::vector <double>	window_widths;
	std::vector <double>	efficiency;
	int	i, j;
	double	temp_eta;
	for (i = 0; i < loaded_diagram.size(); i++) {
		temp_eta = 0.0;
		for (j = 0; j < loaded_diagram[i].size(); j++) {
			if (loaded_diagram[i][j] <= threshold_significance) {
				temp_eta += 1.0;
			}
		}
		temp_eta /= (double) loaded_diagram[i].size();
		window_widths.push_back(window_basewidth * (i+1));
		efficiency.push_back(temp_eta);
	}

	std::vector < std::vector <double> >	output_data;
	std::vector <double>			w_eta(2, 0.0);
	for (i = 0; i < window_widths.size(); i++) {
		w_eta[0] = window_widths[i];
		w_eta[1] = efficiency[i];
		output_data.push_back(w_eta);
	}

	if (write_to_file) {
		error = netOnZeroDXC_save_single_file(output_data, selected_output_filename, separator_char);
	} else {
		for (i = 0; i < output_data.size(); i++) {
			std::cout << output_data[i][0] << separator_char << output_data[i][1] << "\n";
		}
	}
	if (error) {
		std::cerr << "ERROR: i/o error when writing data on file '" << selected_output_filename << "'. Please check permissions.\n";
		exit(1);
	}

	return 0;
}

void netOnZeroDXC_eff_help (char *program_name)
{
	std::cerr << "Usage:\n";
	std::cerr << "\t" << program_name << " -a <#> (<Options>)\t<\t<vector stream>\n";
	std::cerr << "\nMandatory assignment:\n";
	std::cerr << "\t-a <#>\t\tset p value significance threshold;\n";

	std::cerr << "\nOptions:\n";
	std::cerr << "\t-w <#>\t\tset the base window width (corresponding to the first row of the diagram), default is 1.\n";

	std::cerr << "\nInput/output:\n";
	std::cerr << "\t-i <fname>\tread from file 'fname' instead of standard input;\n";
	std::cerr << "\t-o <fname>\twrite to file 'fname' instead of standard output;\n";
	std::cerr << "\t-s <@>\t\tselect label to choose column separator, default t (TAB); other valid options are s (space) or c (comma ',').\n";

	std::cerr << "\n\t-h or --help\tshow this help.\n";
}

int netOnZeroDXC_eff_parse_options (int argc, char *argv[], bool & read_from_file, bool & write_to_file, double & threshold, double & basewidth,
	 			std::string & input_filename, std::string & output_filename, char & separator_char)
{
	int	n = 1;
	while (n < argc) {
		if (strcmp(argv[n], "-a") == 0) {
			n++;
			threshold = atof(argv[n]);

		} else if (strcmp(argv[n], "-i") == 0) {
			read_from_file = true;
			n++;
			input_filename = argv[n];
		} else if (strcmp(argv[n], "-o") == 0) {
			write_to_file = true;
			n++;
			output_filename = argv[n];
		} else if (strcmp(argv[n], "-s") == 0) {
			n++;
			separator_char = argv[n][0];

		} else if( strcmp( argv[n], "-w" ) == 0 ) {
			n++;
			basewidth = atof(argv[n]);

		} else if ((strcmp("-h", argv[n]) == 0) || (strcmp("--help", argv[n]) == 0))  {
			netOnZeroDXC_eff_help(argv[0]);
			exit(0);
		}
		n++;
	}

	if (threshold <= 0) {
		std::cerr << "ERROR: mandatory significance threshold not correctly set. Use " << argv[0] << " -h for a list of options.\n";
		return 1;
	}
	if (basewidth <= 0) {
		std::cerr << "ERROR: base window width is invalid (it is required to be positive).\n";
		return 1;
	}
	if (separator_char == 's') {
		separator_char = ' ';
	} else if (separator_char == 'c') {
		separator_char = ',';
	} else {
		separator_char = '\t';
	}

	return 0;
}

int netOnZeroDXC_eff_check_diagram (const std::vector < std::vector <double> > & diagram)
{
	if (diagram.size() == 0) {
		std::cerr << "ERROR: loaded diagram is zero-sized.\n";
		return 1;
	}

	int	N = diagram[0].size();
	int	i;
	for (i = 0; i < diagram.size(); i++) {
		if (diagram[i].size() != N) {
			std::cerr << "ERROR: inconsistent sizes of diagram rows.\n";
			return 1;
		}
	}

	return 0;
}
