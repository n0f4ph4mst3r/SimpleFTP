set(SOURCE_FILES source/FTPrespone.cpp
                 source/IAsioClientFTP.cpp)	
				 
set(HEADERS headers/enumMessage.h
            headers/FTPresponse.h
            headers/IAsioClientFTP.h)
			
add_library(IAsioClientFTP)	

target_sources(IAsioClientFTP
  PRIVATE
    ${SOURCE_FILES} 
	${HEADERS})
	
target_include_directories(IAsioClientFTP PUBLIC headers)



