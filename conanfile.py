from conans import ConanFile, CMake

class LXMaxConan(ConanFile):
	settings = "os", "compiler", "build_type", "arch"
	generators = "cmake_multi"
	
	requires = (
		"Poco/1.9.4@pocoproject/stable"
		)
		
	default_options = (
		"Poco:enable_xml=False", 
		"Poco:enable_json=False",
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
	