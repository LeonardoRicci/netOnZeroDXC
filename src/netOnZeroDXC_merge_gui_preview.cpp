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
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>

#ifndef INCLUDED_MAINAPP
	#include "netOnZeroDXC_merge_main.hpp"
	#define INCLUDED_MAINAPP
#endif
#ifndef INCLUDED_COLORS
	#include "netOnZeroDXC_gui_colors.hpp"
	#define INCLUDED_COLORS
#endif

// Event table to handle automatically-generated events (resize, refresh)
wxBEGIN_EVENT_TABLE(PlotFrame, wxFrame)
wxEND_EVENT_TABLE()
wxBEGIN_EVENT_TABLE(PanelPlot, wxPanel)
wxEND_EVENT_TABLE()
wxBEGIN_EVENT_TABLE(PanelColorbox, wxPanel)
wxEND_EVENT_TABLE()

// PlotFrame constructor: defines the layout of the preview window
PlotFrame::PlotFrame (const wxString& title, GuiFrame *parent, bool merge_is_global)
: wxFrame(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxFRAME_FLOAT_ON_PARENT | wxCLOSE_BOX | wxCAPTION | wxRESIZE_BORDER)
{
	results_workspace = parent->data_container;
	preview_all = merge_is_global;
	variable_eta = results_workspace->multiple_eta;
	parent_window = parent;

	system_index = 0;
	if (!preview_all) {
		system_index = parent_window->getSystemIndex();
		if (system_index == wxNOT_FOUND) {
			system_index = 0;
		}
	}

	int	nr_pixels = results_workspace->number_of_nodes;

	slider_thr_efficiency = new wxSlider(this, EVENT_SLIDER_THR_EFF, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL | wxSL_MIN_MAX_LABELS);
	statictext_slider_thr_efficiency = new wxStaticText(this, wxID_ANY, wxT("Efficiency threshold (\%):"), wxDefaultPosition, wxDefaultSize, 0);
	Connect(EVENT_SLIDER_THR_EFF, wxEVT_SLIDER, wxCommandEventHandler(PlotFrame::OnSlide));
	slider_thr_efficiency->SetValue((int) (100 * parent->spinner_thr_efficiency->GetValue()));
	if (!variable_eta) {
		slider_thr_efficiency->SetRange(0, 1);
		slider_thr_efficiency->SetValue(0);
		slider_thr_efficiency->Disable();
	}

	int	slider_rnk_recordings_max;
	if (results_workspace->number_of_recordings == 1) {
		slider_rnk_recordings_max = 2;
	} else {
		slider_rnk_recordings_max = results_workspace->number_of_recordings;
	}
	slider_rnk_recordings = new wxSlider(this, EVENT_SLIDER_RNK_REC, 1, 1, results_workspace->number_of_recordings, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL | wxSL_MIN_MAX_LABELS);
	statictext_slider_rnk_recordings = new wxStaticText(this, wxID_ANY, wxT("Rank across recordings (1 = smallest):"), wxDefaultPosition, wxDefaultSize, 0);
	Connect(EVENT_SLIDER_RNK_REC, wxEVT_SLIDER, wxCommandEventHandler(PlotFrame::OnSlide));
	slider_rnk_recordings->SetValue(parent_window->spinner_ranking_recordings->GetValue());
	if (results_workspace->number_of_recordings == 1) {
		slider_rnk_recordings->Disable();
	}

	int	slider_rnk_systems_max;
	if (!preview_all || (results_workspace->number_of_systems == 1)) {
		slider_rnk_systems_max = 2;
	} else {
		slider_rnk_systems_max = results_workspace->number_of_systems;
	}
	slider_rnk_systems = new wxSlider(this, EVENT_SLIDER_RNK_SYS, 1, 1, slider_rnk_systems_max, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL | wxSL_MIN_MAX_LABELS);
	statictext_slider_rnk_systems = new wxStaticText(this, wxID_ANY, wxT("Rank across systems (1 = smallest):"), wxDefaultPosition, wxDefaultSize, 0);
	Connect(EVENT_SLIDER_RNK_SYS, wxEVT_SLIDER, wxCommandEventHandler(PlotFrame::OnSlide));
	slider_rnk_systems->SetValue(parent_window->spinner_ranking_systems->GetValue());
	if (!preview_all || (results_workspace->number_of_systems == 1))
		slider_rnk_systems->Disable();

	plot_area = new PanelPlot(this, wxDefaultSize);
	colorbox_area = new PanelColorbox(this, wxDefaultSize);

	wxBoxSizer	*vbox_slider_eff = new wxBoxSizer(wxVERTICAL);
	vbox_slider_eff->Add(statictext_slider_thr_efficiency, 0, wxALIGN_CENTER_HORIZONTAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);
	vbox_slider_eff->Add(slider_thr_efficiency, 1, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);
	wxBoxSizer	*vbox_slider_rec = new wxBoxSizer(wxVERTICAL);
	vbox_slider_rec->Add(statictext_slider_rnk_recordings, 0, wxALIGN_CENTER_HORIZONTAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);
	vbox_slider_rec->Add(slider_rnk_recordings, 1, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);
	wxBoxSizer	*vbox_slider_sys = new wxBoxSizer(wxVERTICAL);
	vbox_slider_sys->Add(statictext_slider_rnk_systems, 0, wxALIGN_CENTER_HORIZONTAL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);
	vbox_slider_sys->Add(slider_rnk_systems, 1, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0);

	wxBoxSizer	*hbox_sliders = new wxBoxSizer(wxHORIZONTAL);
	hbox_sliders->Add(vbox_slider_eff, 1, wxEXPAND | wxLEFT | wxRIGHT | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 20);
	hbox_sliders->Add(vbox_slider_rec, 1, wxEXPAND | wxLEFT | wxRIGHT | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 20);
	hbox_sliders->Add(vbox_slider_sys, 1, wxEXPAND | wxLEFT | wxRIGHT | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 20);

	wxBoxSizer	*hbox_plot = new wxBoxSizer(wxHORIZONTAL);
	hbox_plot->Add(plot_area, 4, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);
	hbox_plot->Add(colorbox_area, 1, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 4);

	wxBoxSizer	*vbox_plot_all = new wxBoxSizer(wxVERTICAL);
	vbox_plot_all->Add(hbox_sliders, 0, wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);
	vbox_plot_all->Add(hbox_plot, 1, wxEXPAND | wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 2);

	this->SetSizer(vbox_plot_all);
	this->SetAutoLayout(true);

	int	frame_size;
	if (nr_pixels < 10) {
		frame_size = 40 * nr_pixels + 200;
	} else if (nr_pixels < 30) {
		frame_size = 20 * nr_pixels + 200;
	} else {
		frame_size = 12 * nr_pixels + 200;
	}

	SetSize(((frame_size < 800)? 800 : frame_size), frame_size);
	Show();
}

void PlotFrame::OnSlide (wxCommandEvent & WXUNUSED(event))
{
	int	eta_index = (variable_eta)? slider_thr_efficiency->GetValue() : -1;
	if (preview_all)
		results_workspace->evaluateMergedMatrix(slider_rnk_systems->GetValue(), slider_rnk_recordings->GetValue(), eta_index);
	else
		results_workspace->evaluateSystemMatrix(system_index, slider_rnk_recordings->GetValue(), eta_index);

	parent_window->updateSpinnerFromSliders(((double) slider_thr_efficiency->GetValue()) / 100.0, slider_rnk_recordings->GetValue(), slider_rnk_systems->GetValue());
	plot_area->Refresh();
}

PanelPlot::PanelPlot (PlotFrame *parent, wxSize panel_size)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, panel_size, wxBORDER_SIMPLE)
{
	parent_frame = parent;
	results_workspace = parent->results_workspace;

	Connect(wxEVT_PAINT, wxPaintEventHandler(PanelPlot::OnPaint));
	Connect(wxEVT_SIZE, wxSizeEventHandler(PanelPlot::OnResize));
}

void PanelPlot::OnResize (wxSizeEvent & WXUNUSED(event))
{
	this->Refresh();
}

void PanelPlot::OnPaint (wxPaintEvent & WXUNUSED(event))
{
	wxPaintDC dc(this);

	int	nr_pixels = results_workspace->ranked_matrix.size();
	double	w_max = results_workspace->window_widths.back();

	int	size_x, size_y;
	this->GetSize(&size_x, &size_y);
	int	size_displace = 50;
	int	size_m = ( (size_x > size_y)? size_y : size_x ) - 2*size_displace - 5;
	if (size_m < 1) {
		return;
	}
	int	pxsize = size_m / nr_pixels;
	int	i;
	for (i = 0; i < nr_pixels; i++) {
		dc.DrawLabel(results_workspace->node_labels[i], wxRect(wxPoint(0, size_displace + i*pxsize),wxSize(size_displace, pxsize)), wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	}

	dc.SetPen(*wxTRANSPARENT_PEN);
	int	j;
	double	w_to_draw;
	for (i = 0; i < nr_pixels; i++) {
		for (j = 0; j < nr_pixels; j++) {
			wxBrush brush1;
			w_to_draw = results_workspace->ranked_matrix[i][j] / w_max;
			brush1 = wxBrush(netOnZeroDXC_color_palette(w_to_draw));
			dc.SetBrush(brush1);
			dc.DrawRectangle(wxRect(size_displace + j*pxsize, size_displace + i*pxsize, pxsize, pxsize));
		}
	}

	return;
}

// PanelColorbox constructor: defines the color scale bar
PanelColorbox::PanelColorbox (PlotFrame *parent, wxSize panel_size)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, panel_size, wxBORDER_SIMPLE)
{
	parent_frame = parent;

	Connect(wxEVT_PAINT, wxPaintEventHandler(PanelColorbox::OnPaint));
	Connect(wxEVT_SIZE, wxSizeEventHandler(PanelColorbox::OnResize));
}

void PanelColorbox::OnPaint (wxPaintEvent & WXUNUSED(event))
{
	wxPaintDC dc(this);

	int	nr_widths = parent_frame->results_workspace->window_widths.size();

	int	size_x, size_y;
	this->GetSize(&size_x, &size_y);
	if (size_y < 1) {
		return;
	}
	int	size_displace = 25;
	int	pxsize = (size_y - 2*size_displace) / nr_widths;

	dc.DrawLabel("Time scale:", wxRect(wxPoint(10, 0), wxSize(pxsize, size_displace)), wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

	dc.SetPen(*wxTRANSPARENT_PEN);
	int	i;
	int	segment_spacer;
	for (i = 1; i <= nr_widths; i++) {
		wxBrush brush1(netOnZeroDXC_color_palette(i, nr_widths));
		dc.SetBrush(brush1);
		dc.DrawRectangle(wxRect(wxPoint(10, (i-1)*pxsize + size_displace), wxSize(40, pxsize)));
		if (nr_widths < 10) {
			segment_spacer = 1;
		} else {
			segment_spacer = nr_widths / 10;
		}
		if ((nr_widths - i) % segment_spacer == 0) {
		 	char	temp_label[64];
		 	sprintf(temp_label, "%.2e", parent_frame->results_workspace->window_widths[(nr_widths - i)]);
		 	std::string	label_to_draw(temp_label);
		 	dc.DrawLabel(label_to_draw, wxRect(wxPoint(52, (i-1)*pxsize + size_displace), wxSize(40, pxsize)), wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
		}
	}

	return;
}

void PanelColorbox::OnResize (wxSizeEvent & WXUNUSED(event))
{
	this->Refresh();
}
