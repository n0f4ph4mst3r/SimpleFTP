cmake_minimum_required(VERSION 3.26.0)


if(NOT DEFINED ${CMAKE_TOOLCHAIN_FILE})
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")
endif()

project(SimpleFTP)

add_subdirectory(ClientApp)

if(CMAKE_GENERATOR MATCHES "Visual Studio")
   set_directory_properties(
      PROPERTIES
         VS_STARTUP_PROJECT ClientApp
   )
endif()

add_subdirectory(modules/IAsioClientFTP)
find_package(Boost REQUIRED COMPONENTS system thread regex locale)
target_link_libraries(IAsioClientFTP Boost::boost Boost::system Boost::thread Boost::regex)
target_compile_features(IAsioClientFTP PUBLIC cxx_std_20)

find_package(wxWidgets CONFIG REQUIRED)
add_subdirectory(modules/wxFTPWrapper)
target_link_libraries(wxFTPWrapper IAsioClientFTP wx::core wx::base)

add_subdirectory(modules/wxGenericDirPanel/modules/wxRemoteDirCtrl)
target_link_libraries(wxRemoteDirCtrl wxFTPWrapper Boost::locale)

add_subdirectory(modules/wxGenericDirPanel/modules/wxLocalDirCtrl)
target_link_libraries(wxLocalDirCtrl wx::core wx::base)
target_compile_features(wxLocalDirCtrl PUBLIC cxx_std_20)

add_subdirectory(modules/wxGenericDirPanel/modules/wxTreeDropTarget)
target_link_libraries(wxTreeDropTarget wxRemoteDirCtrl wxLocalDirCtrl)

add_subdirectory(modules/wxGenericDirPanel)
target_link_libraries(wxGenericDirPanel wxTreeDropTarget)

add_subdirectory(modules/wxILogCtrl)
target_link_libraries(wxILogCtrl wx::core wx::base)
target_compile_features(wxILogCtrl PUBLIC cxx_std_20)

add_subdirectory(modules/wxTaskManager)
target_link_libraries(wxTaskManager wxFTPWrapper wxILogCtrl)

target_link_libraries(ClientApp wxGenericDirPanel wxTaskManager)
install(TARGETS ClientApp DESTINATION ${CMAKE_SOURCE_DIR}/bin)