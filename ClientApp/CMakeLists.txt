set(SOURCE_FILES source/main.cpp
				 source/ClientApp.cpp
				 )
				 
set(HEADERS headers/main.h 
			headers/ClientApp.h 
			)	
			
set(RESOURCES res/app_icon.rc)

set(XPM res/app_icon.XPM)

if(WIN32)
add_executable(ClientApp WIN32)
target_sources(ClientApp PRIVATE ${RESOURCES})
source_group("Resource files" FILES ${RESOURCES})	
else()
add_executable(ClientApp)			  
endif()

target_sources(ClientApp PRIVATE
                          ${SOURCE_FILES} 
	                      ${HEADERS}
						  ${XPM})	
						  
source_group("XPM files" FILES ${XPM})						  
						  
target_include_directories(ClientApp PUBLIC headers res)