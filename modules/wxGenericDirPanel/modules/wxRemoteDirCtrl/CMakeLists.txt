set(SOURCE_FILES source/wxRemoteDirCtrl.cpp
                 source/wxRemoteTreeCtrlItemData.cpp)	
				 
set(HEADERS headers/wxRemoteDirCtrl.h
            headers/wxRemoteTreeCtrlItemData.h)
			
add_library(wxRemoteDirCtrl)

target_sources(wxRemoteDirCtrl
  PRIVATE
    ${SOURCE_FILES} 
	${HEADERS})
target_include_directories(wxRemoteDirCtrl PUBLIC headers)



