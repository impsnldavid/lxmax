/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <Poco/Channel.h>
#include <Poco/Message.h>
#include "c74_min.h"

/// @brief Implementation of Poco::Channel to allow Poco logging framework to post to Max console
/// 
class max_console_channel : public Poco::Channel
{
	c74::max::t_object* _object = nullptr;

public:
	max_console_channel(c74::max::t_object* object)
		: _object(object)
	{
		
	}

	void log(const Poco::Message& msg) override
	{
		switch (msg.getPriority())
		{
			case Poco::Message::PRIO_FATAL:
			case Poco::Message::PRIO_CRITICAL:
				c74::max::object_error_obtrusive(_object, msg.getText().c_str());
				break;
			case Poco::Message::PRIO_ERROR:
				c74::max::object_error(_object, msg.getText().c_str());
				break;
			case Poco::Message::PRIO_WARNING:
				c74::max::object_warn(_object, msg.getText().c_str());
				break;
			case Poco::Message::PRIO_NOTICE:
			case Poco::Message::PRIO_INFORMATION:
			default:
				c74::max::object_post(_object, msg.getText().c_str());
				break;
			case Poco::Message::PRIO_DEBUG:
			case Poco::Message::PRIO_TRACE:
				c74::max::object_post(_object, "DEBUG %s - %i: %s", msg.getSourceFile(), msg.getSourceLine(),  msg.getText().c_str());
				break;
		}
		
	}
};
