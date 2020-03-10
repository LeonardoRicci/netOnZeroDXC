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

#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>

#ifndef INCLUDED_IOFUNCTIONS
	#include "netOnZeroDXC_io.hpp"
	#define INCLUDED_IOFUNCTIONS
#endif

int netOnZeroDXC_read_data_table (std::vector < std::vector <double> > & data_table, std::string file_path, char separator)
{
	data_table.clear();
	std::ifstream	selected_file_stream;
	selected_file_stream.open(file_path.c_str(), std::ifstream::in);
	if (selected_file_stream.fail())
		return 1;

	char *buffered_line = new char[16384];
	std::string	line;
	std::vector <double>	temp_vector;
	while(selected_file_stream.getline(buffered_line, 16384)) {
		line = buffered_line;
		if ((line.size()) && (line[0] != '#')) {
			netOnZeroDXC_parse_line(temp_vector, line, separator);
			data_table.push_back(temp_vector);
		}
	}
	selected_file_stream.close();

	delete	buffered_line;

	return 0;
}

int netOnZeroDXC_parse_line (std::vector <double> & data_line, std::string & text_line, char separator)
{
	size_t	found = text_line.find_first_of(separator);
        while (found != std::string::npos) {
                text_line[found]='\t';
                found = text_line.find_first_of(separator,found + 1);
        }

	std::istringstream	string_buffer_stream;
        found = text_line.find_first_of("\t");
        while ((found != std::string::npos) && ((found + 1) != text_line.length())) {
                if(text_line[found+1] != '\t') {
                        found = text_line.find_first_of("\t",found + 1);
                        continue;
                }
                text_line.erase(text_line.begin() + found + 1);
        }
        string_buffer_stream.str(text_line);

	double	x;
	std::string	temp_string_token;
	data_line.clear();
        while (!string_buffer_stream.eof()) {
                getline(string_buffer_stream, temp_string_token, '\t');
                x = atof(temp_string_token.c_str());
                if ((x != 0 && x/x != x/x) || (x != x))
			x = std::numeric_limits<double>::quiet_NaN();
                data_line.push_back(x);
	}

	return 0;
}

int netOnZeroDXC_parse_configuration_line (std::string & system_name, std::string & recording_name, bool & matrix_mode, std::string & file_name, std::string & text_line, char separator)
{
	size_t found = text_line.find_first_of(separator);
	if (found == std::string::npos)
		return 1;

	while (found != std::string::npos) {
                text_line[found]='\t';
                found = text_line.find_first_of(separator,found + 1);
        }

	std::istringstream	string_buffer_stream;
        found = text_line.find_first_of("\t");
        while ((found != std::string::npos) && ((found + 1) != text_line.length())) {
                if(text_line[found+1] != '\t') {
                        found = text_line.find_first_of("\t", found + 1);
                        continue;
                }
                text_line.erase(text_line.begin() + found + 1);
        }
        string_buffer_stream.str(text_line);

	std::string	temp_string_token;
	int	i;
	for (i = 0; i < 4; i++) {
		if (string_buffer_stream.eof())
	 		return 1;

		getline(string_buffer_stream, temp_string_token, '\t');
		if (temp_string_token.size() == 0)
			return 1;

		switch (i) {
			case 0:
				system_name = temp_string_token;
				break;
			case 1:
				recording_name = temp_string_token;
				break;
			case 2:
				if (temp_string_token == "m")
					matrix_mode = true;
				else if (temp_string_token == "e")
					matrix_mode = false;
				else
					return 2;
				break;
			case 3:
				file_name = temp_string_token;
				break;
			default:
				break;
		}
	}

	return 0;
}

int netOnZeroDXC_load_single_file (std::vector < std::vector <double> > & data_table, std::vector <std::string> & node_labels, std::string file_name, char separator_char)
{
	int	error = 0;
	std::vector < std::vector <double> >	temp_table;
	error = netOnZeroDXC_read_data_table(temp_table, file_name, separator_char);
	if (error)
		return 2;

	error = netOnZeroDXC_check_linear_sizes(temp_table);
	if (error)
		return 3;

	int	i, j;
	char	temp_label[16];
	std::vector <double>	temp_sequence;
	for (i = 0; i < temp_table[0].size(); i++) {
		temp_sequence.clear();
		for (j = 0; j < temp_table.size(); j++) {
			temp_sequence.push_back(temp_table[j][i]);
		}
		data_table.push_back(temp_sequence);
		if ((i+1) < 10) {
			sprintf(temp_label, "00%d", i + 1);
		} else if ((i+1) < 100) {
			sprintf(temp_label, "0%d", i + 1);
		} else {
			sprintf(temp_label, "%d", i + 1);
		}
		std::string label_string(temp_label);
		node_labels.push_back(label_string);
	}

	if (data_table.size() < 2)
		return 5;

	return 0;
}

int netOnZeroDXC_load_stdin (std::vector < std::vector <double> > & data_table, char separator_char)
{
	data_table.clear();

	char *buffered_line = new char[16384];
	std::string	line;
	std::vector <double>	temp_vector;
	while(std::cin.getline(buffered_line, 16384)) {
		line = buffered_line;
		if ((line.size()) && (line[0] != '#')) {
			netOnZeroDXC_parse_line(temp_vector, line, separator_char);
			data_table.push_back(temp_vector);
		}
	}
	delete	buffered_line;

	return 0;
}

int netOnZeroDXC_load_single_matrix (std::vector < std::vector <double> > & matrix, std::string file_name, char separator_char)
{
	int	error = 0;
	std::vector < std::vector <double> >	temp_table;
	error = netOnZeroDXC_read_data_table(temp_table, file_name, separator_char);
	if (error)
		return 2;

	if (temp_table.size() < 2)
		return 3;

	if (temp_table[0].size() != temp_table.size())
		return 3;

	int	i, j;
	std::vector <double>	temp_row;
	matrix.clear();
	for (i = 0; i < temp_table.size(); i++) {
		temp_row.clear();
		for (j = 0; j < temp_table[i].size(); j++) {
			temp_row.push_back(temp_table[i][j]);
		}
		matrix.push_back(temp_row);
	}

	return 0;
}

int netOnZeroDXC_load_single_table (std::vector < std::vector <double> > & data_table, std::string file_name, char separator_char)
{
	int	error = 0;
	error = netOnZeroDXC_read_data_table(data_table, file_name, separator_char);
	if (error)
		return 2;

	return 0;
}

int netOnZeroDXC_read_dictionary (std::vector <std::string> & string_list, std::string file_path, char separator)
{
	string_list.clear();
	std::ifstream	selected_file_stream;
	selected_file_stream.open(file_path.c_str(), std::ifstream::in);
	if (selected_file_stream.fail())
		return 1;

	char *buffered_line = new char[256];
	std::string	line;
	std::string	temp_string;
	int		temp_index;
	std::vector <std::string>	temp_string_list;
	std::vector <int>		indexes_list;
	while(selected_file_stream.getline(buffered_line, 256)) {
		line = buffered_line;
		if ((line.size()) && (line[0] != '#')) {
			netOnZeroDXC_parse_dictionary_line(temp_string, temp_index, line, separator);
			temp_string_list.push_back(temp_string);
			indexes_list.push_back(temp_index);
		}
	}
	selected_file_stream.close();
	delete	buffered_line;

	if (netOnZeroDXC_check_list_labels(temp_string_list))
		return 2;

	bool	starts_from_one;
	if (netOnZeroDXC_check_list_numbers(starts_from_one, indexes_list))
		return 3;

	int	i;
	int	s = (starts_from_one)? -1 : 0;
	string_list.resize(indexes_list.size(), "");
	for (i = 0; i < indexes_list.size(); i++) {
		string_list[indexes_list[i] + s] = temp_string_list[i];
	}

	return 0;
}

int netOnZeroDXC_parse_dictionary_line (std::string & label, int & index, std::string & stream_line, char separator)
{
	size_t	found = stream_line.find_first_of(separator);
        while (found != std::string::npos) {
                stream_line[found]='\t';
                found = stream_line.find_first_of(separator,found + 1);
        }

	std::istringstream	string_buffer_stream;
        found = stream_line.find_first_of("\t");
        while ((found != std::string::npos) && ((found + 1) != stream_line.length())) {
                if(stream_line[found+1] != '\t') {
                        found = stream_line.find_first_of("\t",found + 1);
                        continue;
                }
                stream_line.erase (stream_line.begin() + found + 1);
        }
        string_buffer_stream.str(stream_line);

	label = "";
	std::string	temp_string_token;
	if (!string_buffer_stream.eof()) {
		getline(string_buffer_stream, temp_string_token, '\t');
		index = atof(temp_string_token.c_str());
		if ((index != 0 && index/index != index/index) || (index != index))
			return 1;
		getline(string_buffer_stream, temp_string_token, '\t');
		label = temp_string_token;
	}

	return 0;
}

int netOnZeroDXC_load_labels_dictionary (std::vector <std::string> & labels, std::string file_name, char separator)
{
	int	error = 0;
	std::vector < std::vector <double> >	temp_table;
	error = netOnZeroDXC_read_dictionary(labels, file_name, separator);
	return error;
}

int netOnZeroDXC_parse_filename_2labels (PairOfLabels & parsed_label_pair, std::string & file_name, char filename_delimiter)
{
	std::string	temp_filename(file_name);
	temp_filename.erase(temp_filename.find_last_of("."), std::string::npos);

	size_t last_delimiter_found = temp_filename.find_last_of(filename_delimiter);
	if (last_delimiter_found == std::string::npos)
		return 1;

	std::string	temp_string_label(temp_filename, last_delimiter_found + 1, std::string::npos);
	int	error = netOnZeroDXC_check_label_format(temp_string_label);
	if (error)
		return 1;
	parsed_label_pair.label_b = temp_string_label;

	temp_filename.erase(last_delimiter_found, std::string::npos);
	last_delimiter_found = temp_filename.find_last_of(filename_delimiter);
	if (last_delimiter_found == std::string::npos)
		return 1;

	temp_string_label.clear();
	temp_string_label = temp_filename.substr(last_delimiter_found + 1, std::string::npos);
	error = netOnZeroDXC_check_label_format(temp_string_label);
	if (error)
		return 1;
	parsed_label_pair.label_a = temp_string_label;

	return 0;
}

int netOnZeroDXC_parse_filename_1label(std::string & node_label, std::string & file_name, char filename_delimiter)
{
	std::string	temp_filename(file_name);
	temp_filename.erase(temp_filename.find_last_of("."), std::string::npos);

	size_t last_delimiter_found = temp_filename.find_last_of(filename_delimiter);
	if (last_delimiter_found == std::string::npos)
		return 1;

	std::string	temp_string_label (temp_filename, last_delimiter_found + 1, std::string::npos);
	int	error = netOnZeroDXC_check_label_format(temp_string_label);
	if (error)
		return 1;

	node_label = temp_string_label;

	return 0;
}

int netOnZeroDXC_check_table_sizes (const std::vector < std::vector < std::vector <double> > > & diagram_data_table)
{
	int	n = diagram_data_table.size();
	if (n == 0)
		return 1;

	int	m = diagram_data_table[0].size();
	if (m == 0)
		return 1;

	int	l = diagram_data_table[0][0].size();
	if (l == 0)
		return 1;

	int	i, j;
	for (i = 0; i < n; i++) {
		if (diagram_data_table[i].size() != m)
			return 1;
		for (j = 0; j < m; j++) {
			if (diagram_data_table[i][j].size() != l)
				return 1;
		}
	}

	return 0;
}

int netOnZeroDXC_check_linear_sizes (const std::vector < std::vector <double> > & linear_data_table)
{
	int	n = linear_data_table.size();
	if (n == 0)
		return 1;

	int	m = linear_data_table[0].size();
	if (m == 0)
		return 1;

	int	i;
	for (i = 0; i < n; i++) {
		if (linear_data_table[i].size() != m)
			return 1;
	}

	return 0;
}

int netOnZeroDXC_check_list_pairs (const std::vector <PairOfLabels> & list_pairs)
{
	int	i, j;
	for (i = 0; i < list_pairs.size() - 1; i++) {
		for (j = i + 1; j < list_pairs.size(); j++) {
			if ((list_pairs[i].label_a == list_pairs[j].label_a) && (list_pairs[i].label_b == list_pairs[j].label_b))
				return 1;
		}
	}

	return 0;
}

int netOnZeroDXC_check_list_labels (const std::vector <std::string> & list_labels)
{
	int	i, j;
	for (i = 0; i < list_labels.size() - 1; i++) {
		for (j = i + 1; j < list_labels.size(); j++) {
			if (list_labels[i] == list_labels[j])
				return 1;
		}
	}

	return 0;
}

int netOnZeroDXC_check_list_numbers (bool & starts_from_one, const std::vector <int> & list_numbers)
{
	starts_from_one = true;
	int	i, j;
	for (i = 0; i < list_numbers.size() - 1; i++) {
		for (j = i + 1; j < list_numbers.size(); j++) {
			if (list_numbers[i] == list_numbers[j])
				return 1;
		}
		if (list_numbers[i] < 0) {
			return 1;
		} else if (list_numbers[i] == 0) {
			starts_from_one = false;
		}
	}

	std::vector <int>	temp_list_numbers = list_numbers;
	sort(temp_list_numbers.begin(), temp_list_numbers.end());

	int	s = (starts_from_one)? 1 : 0;
	for (i = 0; i < temp_list_numbers.size(); i++) {
		if ((i+s) != temp_list_numbers[i])
			return 1;
	}

	return 0;
}

int netOnZeroDXC_check_label_format (const std::string & label)
{
	int	i;
	for (i = 0; i < label.size(); i++) {
		if (!isalnum(label[i]))
			return 1;
	}

	return 0;
}

int netOnZeroDXC_fill_list_pairs (std::vector <PairOfLabels> & list_pairs, const std::vector <std::string> & list_labels)
{
	list_pairs.clear();

	int	i, j;
	PairOfLabels	temp_pair;
	for (i = 0; i < list_labels.size() - 1; i++) {
		for (j = i + 1; j < list_labels.size(); j++) {
			temp_pair.label_a = list_labels[i];
			temp_pair.label_b = list_labels[j];
			list_pairs.push_back(temp_pair);
		}
	}

	return 0;
}

int netOnZeroDXC_postfill_list_labels (std::vector <std::string> & list_labels, const std::vector <PairOfLabels> & list_pairs)
{
	list_labels.clear();

	if (list_pairs.size() == 0)
		return 1;

	list_labels.push_back(list_pairs[0].label_a);
	list_labels.push_back(list_pairs[0].label_b);

	int		i, j;
	bool		already_known;
	std::string	candidate;
	for (i = 1; i < list_pairs.size(); i++) {
		candidate = list_pairs[i].label_a;
		already_known = false;
		for (j = 0; j < list_labels.size(); j++) {
			if (candidate == list_labels[j]) {
				already_known = true;
				break;
			}
		}
		if (!already_known)
			list_labels.push_back(candidate);

		candidate = list_pairs[i].label_b;
		already_known = false;
		for (j = 0; j < list_labels.size(); j++) {
			if (candidate == list_labels[j]) {
				already_known = true;
				break;
			}
		}
		if (!already_known)
			list_labels.push_back(candidate);
	}

	sort(list_labels.begin(), list_labels.end());

	return 0;
}

int netOnZeroDXC_associate_index_of_pair (const std::vector <PairOfLabels> & node_pairs, const std::vector <std::string> & node_labels, int i, int j)
{
	std::string	label_a = node_labels[i];
	std::string	label_b = node_labels[j];

	int	k;
	for (k = 0; k < node_pairs.size(); k++) {
		if ((node_pairs[k].label_a == label_a) && (node_pairs[k].label_b == label_b)) {
			return k;
		} else if ((node_pairs[k].label_b == label_a) && (node_pairs[k].label_a == label_b)) {
			return k;
		}
	}

	return -1;
}

int netOnZeroDXC_check_new_label_pairs (std::vector <PairOfLabels> & temp_label_pairs, const std::vector <PairOfLabels> & already_known)
{
	int	i, j;
	for (i = 0; i < already_known.size(); i++) {
		for (j = 0; j < temp_label_pairs.size(); j++) {
			if ((already_known[i].label_a == temp_label_pairs[j].label_a) && (already_known[i].label_b == temp_label_pairs[j].label_b)) {
				temp_label_pairs.erase(temp_label_pairs.begin() + j);
				break;
			}
		}
	}
	if (temp_label_pairs.size() > 0) {
		return 1;
	}

	return 0;
}

int netOnZeroDXC_save_diagram (const std::vector < std::vector <double> > & diagram, std::string path, std::string prefix, std::string label,
			char delimiter, std::string label_a, std::string label_b, char separator)
{
	std::string		filename;
	FILE *			file_pointer;

	filename = netOnZeroDXC_generate_filepath(path, prefix, label, delimiter, label_a, label_b);
	file_pointer = fopen(filename.c_str(), "w");
	if (!file_pointer)
		return 1;

	int	i, j;
	for (i = 0; i < diagram.size(); i++) {
		fprintf(file_pointer, "%.3f", diagram[i][0]);
		for (j = 1; j < diagram[i].size(); j++) {
			fprintf(file_pointer, "%c%.3f", separator, diagram[i][j]);
		}
		fprintf(file_pointer, "\n");
	}

	if(fclose(file_pointer) == EOF)
		return 1;

	return 0;
}

int netOnZeroDXC_save_linear_data (const std::vector <double> & x, const std::vector <double> & y, std::string path, std::string prefix, std::string label,
				char delimiter, std::string label_a, std::string label_b, char separator)
{
	if (x.size() != y.size())
		return 1;

	std::string		filename;
	FILE *			file_pointer;

	filename = netOnZeroDXC_generate_filepath(path, prefix, label, delimiter, label_a, label_b);
	file_pointer = fopen(filename.c_str(), "w");
	if (!file_pointer)
		return 1;

	int	i, j;
	for (i = 0; i < x.size(); i++)
		fprintf(file_pointer, "%.3f\t%.3f\n", x[i], y[i]);

	if(fclose(file_pointer) == EOF)
		return 1;

	return 0;
}

int netOnZeroDXC_save_single_file (const std::vector < std::vector <double> > & data_table, std::string file_name, char separator)
{
	FILE *	file_pointer;
	file_pointer = fopen(file_name.c_str(), "w");
	if (!file_pointer)
		return 1;

	int	i, j;
	for (i = 0; i < data_table.size(); i++) {
		fprintf(file_pointer, "%.3f", data_table[i][0]);
		for (j = 1; j < data_table[i].size(); j++) {
			fprintf(file_pointer, "%c%.3f", separator, data_table[i][j]);
		}
		fprintf(file_pointer, "\n");
	}

	if(fclose(file_pointer) == EOF)
		return 1;

	return 0;
}

int netOnZeroDXC_save_log_file (const std::stringstream & content, std::string file_name)
{
	FILE *		file_pointer;
	file_pointer = fopen(file_name.c_str(), "w");
	if (!file_pointer)
		return 1;

	std::string	content_string = content.str();
	fprintf(file_pointer, "%s", content_string.c_str());

	if(fclose(file_pointer) == EOF)
		return 1;

	return 0;
}

std::string netOnZeroDXC_generate_filepath (std::string path, std::string prefix, std::string label, char delimiter, std::string label_a, std::string label_b)
{
	std::stringstream	filename_buffer_stream;

	filename_buffer_stream << path;

	char	directory_char = '/';	// Windows should accept "/" as directory separator
	if (path.find_first_of('\\') != std::string::npos)	// Nevertheless, we should keep consistency with the loaded string
		directory_char = '\\';

	if (path.find_last_of(directory_char) != path.size())
		filename_buffer_stream << directory_char;

	if (prefix != "")
		filename_buffer_stream << prefix << delimiter;

	filename_buffer_stream << label;
	if (label_a.size() && label_b.size()) {
		filename_buffer_stream << delimiter << label_a;
		filename_buffer_stream << delimiter << label_b;
	}
	filename_buffer_stream << ".dat";

	return filename_buffer_stream.str();;
}
