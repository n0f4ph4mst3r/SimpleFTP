set(SOURCE_FILES source/wxILogCtrl.cpp)		 
set(HEADERS headers/wxILogCtrl.h)

add_library(wxILogCtrl)

target_sources(wxILogCtrl
  PRIVATE
    ${SOURCE_FILES} 
	${HEADERS})
target_include_directories(wxILogCtrl PUBLIC headers)



