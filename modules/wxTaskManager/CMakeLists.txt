set(SOURCE_FILES source/wxLogCtrl.cpp
                 source/wxTaskManager.cpp)
				 
set(HEADERS headers/wxLogCtrl.h
            headers/wxTaskManager.h)
			
add_library(wxTaskManager)

target_sources(wxTaskManager
  PRIVATE
    ${SOURCE_FILES} 
	${HEADERS})
target_include_directories(wxTaskManager PUBLIC headers)



