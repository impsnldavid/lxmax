/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <vector>
#include <algorithm>
#include "c74_min.h"


enum class column_type
{
	_static,
	_text,
	_toggle,
	_enum,
	_int
};


class dict_edit_column
{
    const c74::min::symbol k_sym_static { "static" };
    const c74::min::symbol k_sym_text { "text" };
    const c74::min::symbol k_sym_toggle { "toggle" };

	c74::min::symbol _name;
	column_type _type;
	int _width;
	c74::min::atoms _options;

	int _range_min;
	int _range_max;

public:
	dict_edit_column(const c74::min::symbol& name, int width, column_type columnType)
		: _name(name),
		  _type(columnType),
		  _width(width), _range_min(0), _range_max(1)
	{
	}

	dict_edit_column(const c74::min::symbol& name, int width, int rangeMin, int rangeMax)
		: _name(name),
		  _type(column_type::_int),
		  _width(width),
		  _range_min(rangeMin),
		  _range_max(rangeMax)
	{
	}

	dict_edit_column(const c74::min::symbol& name, int width, const c74::min::atoms& options)
		: _name(name),
		  _type(column_type::_enum),
		  _width(width),
		  _options(options),
		  _range_min(0),
		  _range_max(1)
	{
	}


	const c74::min::symbol& name() const
	{
		return _name;
	}

	column_type type() const
	{
		return _type;
	}

	c74::min::atoms type_atoms() const
	{
		switch (_type)
		{
			case column_type::_static:
                return { k_sym_static };
			case column_type::_text:
				return { k_sym_text };
			case column_type::_toggle:
				return { k_sym_toggle };
			case column_type::_enum:
				return _options;
			case column_type::_int:
				return { _range_min, _range_max };
			default:
                    assert(false);
				return{};
			
		}
	}

	int width() const
	{
		return _width;
	}

	const c74::min::atoms& options() const
	{
		return _options;
	}

	int range_min() const
	{
		return _range_min;
	}

	int range_max() const
	{
		return _range_max;
	}
};


class dict_edit
{
    const c74::min::symbol k_sym_name { "name" };
    const c74::min::symbol k_sym_type { "type" };
    const c74::min::symbol k_sym_width { "width" };
    const c74::min::symbol k_sym_integer { "integer" };

    c74::min::dict _dictionary;

	std::vector<dict_edit_column> _columns;

    void initialize()
    {
        std::vector<c74::max::t_atom> names(_columns.size());
        std::vector<c74::max::t_atom> types(_columns.size());
        std::vector<c74::max::t_atom> widths(_columns.size());
        
        for (int i = 0; i < _columns.size(); ++i)
        {
            c74::max::atom_setsym(&names[i], _columns[i].name());
            
            auto type_atoms = _columns[i].type_atoms();
            
            std::vector<c74::max::t_atom> argv;
            argv.reserve(type_atoms.size());
            
            std::for_each(std::begin(type_atoms), std::end(type_atoms),
                          [&](const c74::min::atom& a) { argv.push_back(a); });
            
            switch(_columns[i].type())
            {
                case column_type::_static:
                case column_type::_text:
                case column_type::_toggle:
                    c74::max::atom_setsym(&types[i], c74::max::atom_getsym(&argv[0]));
                    break;
                    
                case column_type::_enum:
                {
                    auto array = c74::max::atomarray_new(argv.size(), argv.data());
                    c74::max::atom_setobj(&types[i], array);
                }
                    break;
                    
                case column_type::_int:
                {
                    c74::max::t_dictionary* d = c74::max::dictionary_new();
                    c74::max::dictionary_appendatoms(d, k_sym_integer, argv.size(), argv.data());
                    c74::max::atom_setobj(&types[i], d);
                }
                    break;
            }
            
            c74::max::atom_setlong(&widths[i], _columns[i].width());
        }
        
        c74::max::dictionary_appendatoms(_dictionary, k_sym_name, names.size(), names.data());
        c74::max::dictionary_appendatoms(_dictionary, k_sym_type, types.size(), types.data());
        c74::max::dictionary_appendatoms(_dictionary, k_sym_width, widths.size(), widths.data());
    }
    
public:

	dict_edit(const c74::min::symbol& dict_name, const std::vector<dict_edit_column>& columns)
		: _dictionary(dict_name),
        _columns(columns)
	{
        // TODO: Change _dictionary constructor to take symbol directly when bug is fixed
        
        initialize();
	}
    
    void read_from_max_preferences(const std::string& filename)
    {
        _dictionary.clear();
        
        short pref_path;
        c74::max::preferences_path(nullptr, true, &pref_path);
         
        c74::max::t_dictionary* d;
        c74::max::dictionary_read((char*)filename.c_str(), pref_path, &d);
        
        if (d != nullptr)
        {
            c74::min::dict to_import { d };
            _dictionary.copyunique(to_import);
        }
        
        initialize();
    }
    
    void write_to_max_preferences(const std::string& filename)
    {
        c74::min::dict output;
        output.copyunique(_dictionary);
        
        c74::max::dictionary_deleteentry(output, k_sym_name);
        c74::max::dictionary_deleteentry(output, k_sym_type);
        c74::max::dictionary_deleteentry(output, k_sym_width);
        
        short pref_path;
        c74::max::preferences_path(nullptr, true, &pref_path);
        c74::max::dictionary_write(output, (char*)filename.c_str(), pref_path);
    }
    
    void add_entry(int index, const c74::min::atoms& atoms)
    {
        std::vector<c74::max::t_atom> argv;
        argv.reserve(atoms.size());
        
        std::for_each(std::begin(atoms), std::end(atoms),
                      [&](const c74::min::atom& a) { argv.push_back(a); });
        
        auto array = c74::max::atomarray_new(argv.size(), argv.data());
        
        c74::max::dictionary_appendobject(_dictionary, c74::min::symbol(index), array);
    }
    
    void remove_entry(int index)
    {
        c74::max::dictionary_deleteentry(_dictionary, c74::min::symbol(index));
    }

    std::map<int, c74::min::atoms> get_entries()
    {
		long num_keys;
    	c74::max::t_symbol** keys = nullptr;
		c74::max:: dictionary_getkeys(_dictionary, &num_keys, &keys);

        std::map<int, c74::min::atoms> entries;
		
    	for(long i = 0; i < num_keys; ++i)
    	{
            c74::min::symbol key = keys[i];
    		
    		if (key == k_sym_name || key == k_sym_type || key == k_sym_width)
                continue;
            
            entries.insert(std::pair(std::stoi(key), _dictionary.at(key)));
    	}

		return entries;
	}
    
    void clear_entries()
    {
        _dictionary.clear();
        initialize();
    }
    
    int entry_count()
    {
        long num_keys;
        c74::max::t_symbol** keys = nullptr;
        c74::max::dictionary_getkeys(_dictionary, &num_keys, &keys);
        
        return num_keys - 3;
    }
    
    int highest_index()
    {
        long num_keys;
        c74::max::t_symbol** keys = nullptr;
        c74::max::dictionary_getkeys(_dictionary, &num_keys, &keys);
        
        int highest_key = 0;
        
        for (long i = 0; i < num_keys; ++i)
        {
            c74::min::symbol key = keys[i];
            
            if (key == k_sym_name || key == k_sym_type || key == k_sym_width)
                continue;
            
            int key_value = std::stoi(key);
            
            if (key_value > highest_key)
                highest_key = key_value;
        }
        
        return highest_key;
    }
    

    
    operator c74::min::dict() const
    {
        return _dictionary;
    }
    
    operator c74::max::t_object*() const
    {
        return _dictionary;
    }
};
