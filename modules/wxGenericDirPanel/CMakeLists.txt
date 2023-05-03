set(SOURCE_FILES source/wxGenericDirPanel.cpp)				 
set(HEADERS headers/wxGenericDirPanel.h)

add_library(wxGenericDirPanel)

target_sources(wxGenericDirPanel
  PRIVATE
    ${SOURCE_FILES} 
	${HEADERS})
target_include_directories(wxGenericDirPanel PUBLIC headers)



