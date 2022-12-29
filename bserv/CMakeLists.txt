add_library(
	bserv
	
	pch.cpp
	bserv.cpp
	client.cpp
	database.cpp
	session.cpp
	utils.cpp
)

target_include_directories(
	bserv PUBLIC

	/usr/local/include
	
	../dependencies
	../dependencies/libpqxx/include
	include
)

target_link_libraries(
	bserv PUBLIC
	
	pthread
	/usr/local/lib/libboost_thread.dylib
	/usr/local/lib/libboost_context.dylib
	/usr/local/lib/libboost_coroutine.dylib
	/usr/local/lib/libboost_system.dylib
	/usr/local/lib/libboost_filesystem.dylib
	/usr/local/lib/libboost_log.dylib
	/usr/local/lib/libboost_log_setup.dylib
	/usr/local/lib/libboost_json.dylib
	"${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/cryptopp/libcryptopp.a"
	"${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/libpqxx/src/.libs/libpqxx.a"
	/usr/local/Cellar/postgresql@14/14.5_4/lib/postgresql@14/libpq.dylib
)
