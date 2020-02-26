/// @file
/// @ingroup     lxmax
/// @copyright    Copyright 2019 David Butler / The Impersonal Stereo. All rights reserved.
/// @license    Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "c74_min_flags.h"
#include "common.hpp"
#include "dict_edit.hpp"

#include "dmx_output_service.hpp"


using namespace c74::min;

class lxmax;
static minwrap<lxmax>* s_lxmax_instance {};

class lxmax : public object<lxmax> {
    
    const string k_preferences_filename { "lxmaxpreferences.json" };
    const string k_preferences_dict_name { "___lxmax_preferences" };
    
    const vector<dict_edit_column> k_editor_columns
    {
        { "Label", 200, column_type::_text },
        { "Type", 70, atoms{ "Output", "Input" } },
        { "On", 40, column_type::_toggle },
        { "Protocol / Device", 155, atoms{ "Art-Net", "sACN" } },
        { "Internal Universe", 90, 1, 65535 },
        { "Settings", 200, column_type::_static }
    };
    
    dict_edit _prefences { k_preferences_dict_name, k_editor_columns };
    void* _registered_obj = nullptr;

    void write_preferences()
    {
        _prefences.write_to_max_preferences(k_preferences_filename);
    }
    
public:
	MIN_DESCRIPTION	{"LXMax service object."};
	MIN_TAGS		{"lxmax"};
	MIN_AUTHOR		{"David Butler / The Impersonal Stereo"};
	MIN_RELATED		{"lx.dimmer, lx.colorfixture"};
    
    MIN_FLAGS       { behavior_flags::nobox };

    lxmax(const atoms& args = {})
    {
        _prefences.read_from_max_preferences(k_preferences_filename);
            
        _registered_obj = c74::max::object_register(k_lxmax_namespace, k_lxmax_service_registration, this->maxobj());
    }
            
    ~lxmax()
    {
        write_preferences();
        
        if (_registered_obj != nullptr)
            c74::max::object_unregister(_registered_obj);
    }

    message<> add_universe { this, "add_universe", "Adds a universe to the configuration",
        MIN_FUNCTION {
            
            int next_index = _prefences.highest_index() + 1;
            
            _prefences.add_entry(next_index, { "Test", "Output", 1, "Art-Net", 1, "[prefs]"});
            return {};
        }
    };
    
    message<> remove_universe { this, "remove_universe", "Removes a universe from the configuration",
        MIN_FUNCTION {
            
            int index = args[0];
            _prefences.remove_entry(index);
            
            return {};
        }
    };
            
    message<> clear_universes { this, "clear_universes", "Removes all universes from the configuration",
        MIN_FUNCTION {
            
            _prefences.clear_entries();
            
            return {};
        }
    };

private:
	message<> maxclass_setup = { this, "maxclass_setup", 
		MIN_FUNCTION {

			cout << "LXMax - v0.0.1 - David Butler / The Impersonal Stereo" << endl;

			s_lxmax_instance = (minwrap<lxmax>*)c74::max::object_new_typed(symbol("nobox"), symbol("lxmax"), 0, nullptr);
			return {};
		}
	};
};

void on_max_quit(void* a)
{
    c74::max::object_free(a);
}

MIN_EXTERNAL(lxmax)

//void ext_main(void* r)
//{
//    c74::min::wrap_as_max_external<lxmax>("lxmax", "lxmax.cpp", r);
//    
//    c74::max::t_object* obj = c74::max::object_new(c74::max::CLASS_NOBOX, symbol("lxmax"));
//    assert(obj);
//    
//    c74::max::quittask_install((c74::max::method)on_max_quit, obj);
//}
