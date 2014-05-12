/*
    Copyright (C) 2012 Waves Audio Ltd.  

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef __device_connection_control_h__
#define __device_connection_control_h__

#include <inttypes.h>
#include <gtkmm/layout.h>
#include "waves_ui.h"

class XMLTree;

class DeviceConnectionControl : public Gtk::Layout
{
  public:
	enum ConnectionNumber {
		NoNumber = 0
	};

	DeviceConnectionControl (std::string device_capture_name, bool active, uint16_t capture_number, std::string track_name);
	DeviceConnectionControl (std::string device_playback_name, bool active, uint16_t playback_number);
	DeviceConnectionControl (std::string midi_capture_name, bool active);
	DeviceConnectionControl (bool active);

	bool build_layout (std::string file_name);
	void set_number (uint16_t number);
	void set_active (bool active);
	sigc::signal2<void, DeviceConnectionControl*, bool> signal_active_changed;

  private:
	void init(std::string name, bool active, uint16_t number, std::string track_name="");
	void on_active_on(WavesButton*);
	void on_active_off(WavesButton*);

	WavesUI::WidgetMap _children;
	WavesButton* _active_on_button; 
	WavesButton* _active_off_button;
	Gtk::Label* _name_label;
	Gtk::Label* _number_label;
	Gtk::Label* _track_name_label;
};

#endif // __device_connection_control_h__
