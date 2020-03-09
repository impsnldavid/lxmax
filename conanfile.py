# Copyright 2020 David Butler. All rights reserved.
# Use of this source code is governed by the MIT License found in the LICENSE file.

from conans import ConanFile, CMake

class LXMaxConan(ConanFile):
	settings = "os", "compiler", "build_type", "arch"
	generators = "cmake_multi"
	
	requires = (
		"poco/1.10.1"
		)
		
	default_options = (
		"Poco:enable_xml=False",
		"Poco:enable_mongodb=False",
		"Poco:enable_netssl=False",
		"Poco:enable_netssl_win=False", 
		"Poco:enable_crypto=False",
		"Poco:enable_data=False",
		"Poco:enable_data_sqlite=False",
		"Poco:enable_zip=False",
		"Poco:force_openssl=False", 
		"Poco:cxx_14=True"
		)
	