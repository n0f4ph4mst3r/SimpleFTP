set(SOURCE_FILES source/wxLogMessageItem.cpp
                 source/wxFTPWrapper.cpp)	
				 
set(HEADERS headers/wxTransferTransaction.h
            headers/wxLogMessageItem.h
            headers/wxFTPWrapper.h)
			
add_library(wxFTPWrapper)	

target_sources(wxFTPWrapper
  PRIVATE
    ${SOURCE_FILES} 
	${HEADERS})
target_include_directories(wxFTPWrapper PUBLIC headers)



