set(SOURCE_FILES source/wxLocalDropTarget.cpp
                 source/wxRemoteDropTarget.cpp
                 source/wxTreeDropTarget.cpp)	
				 
set(HEADERS headers/wxLocalDropTarget.h
            headers/wxRemoteDropTarget.h
            headers/wxTreeDropTarget.h)
			
add_library(wxTreeDropTarget)

target_sources(wxTreeDropTarget
  PRIVATE
    ${SOURCE_FILES} 
	${HEADERS})
target_include_directories(wxTreeDropTarget PUBLIC headers)



