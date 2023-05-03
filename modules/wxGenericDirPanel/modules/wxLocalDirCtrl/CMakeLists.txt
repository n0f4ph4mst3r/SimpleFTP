set(SOURCE_FILES source/wxLocalDirCtrl.cpp)			 
set(HEADERS headers/wxLocalDirCtrl.h)

add_library(wxLocalDirCtrl)

target_sources(wxLocalDirCtrl
  PRIVATE
    ${SOURCE_FILES} 
	${HEADERS})
target_include_directories(wxLocalDirCtrl PUBLIC headers)



