file(GLOB SRCFILES CONFIGURE_DEPENDS
	"*.cpp"
	"*.h"
	)

add_executable(WebApp ${SRCFILES})

target_include_directories(
	WebApp PUBLIC
	
	../dependencies/inja/include
	../dependencies/inja/third_party/include
)

target_link_libraries(
	WebApp PUBLIC
	
	bserv
)
