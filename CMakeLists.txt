cmake_minimum_required(VERSION 3.16)

# Homebrew's ITK/VTK on this machine are built against the Command Line Tools SDK, not whichever
# SDK Xcode bundles by default — mixing the two breaks libc++ header resolution (symptom: "<cstddef>
# tried including <stddef.h> but didn't find libc++'s <stddef.h> header", or isnan/isinf errors from
# VTK). IDEs like Qt Creator auto-detect the Xcode SDK by default, so pin the CLT one here instead of
# relying on every IDE/kit config to get this right. Respects an explicit -DCMAKE_OSX_SYSROOT override.
if(APPLE AND NOT DEFINED CMAKE_OSX_SYSROOT)
  execute_process(
    COMMAND env DEVELOPER_DIR=/Library/Developer/CommandLineTools xcrun --sdk macosx --show-sdk-path
    OUTPUT_VARIABLE _mrus_clt_sdk_path
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
  if(_mrus_clt_sdk_path)
    set(CMAKE_OSX_SYSROOT "${_mrus_clt_sdk_path}" CACHE PATH "Pinned to Command Line Tools SDK to match Homebrew ITK/VTK" FORCE)
  endif()
  unset(_mrus_clt_sdk_path)
endif()

project(mrus-fusion-viewer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(mrus-fusion-viewer
  src/main.cpp

  src/model/CaseRepository.h
  src/model/CaseRepository.cpp
  src/model/ImageSlicing.h
  src/model/ImageSlicing.cpp
  src/model/ImageConversion.h
  src/model/ImageConversion.cpp
  src/model/DenoiseService.h
  src/model/DenoiseService.cpp
  src/model/SegmentationService.h
  src/model/SegmentationService.cpp
  src/model/RegistrationService.h
  src/model/RegistrationService.cpp
  src/model/Types.h

  src/viewmodel/CaseViewModel.h
  src/viewmodel/CaseViewModel.cpp
  src/viewmodel/FilterViewModel.h
  src/viewmodel/FilterViewModel.cpp
  src/viewmodel/SegmentViewModel.h
  src/viewmodel/SegmentViewModel.cpp
  src/viewmodel/RegisterViewModel.h
  src/viewmodel/RegisterViewModel.cpp
  src/viewmodel/FusionViewModel.h
  src/viewmodel/FusionViewModel.cpp

  src/view/MainWindow.h
  src/view/MainWindow.cpp
  src/view/MainWindow.ui
  src/view/FilterTabView.h
  src/view/FilterTabView.cpp
  src/view/FilterTabView.ui
  src/view/SegmentTabView.h
  src/view/SegmentTabView.cpp
  src/view/SegmentTabView.ui
  src/view/RegisterTabView.h
  src/view/RegisterTabView.cpp
  src/view/RegisterTabView.ui
  src/view/FusionTabView.h
  src/view/FusionTabView.cpp
  src/view/FusionTabView.ui
  src/view/DisplayConstants.h
)

target_include_directories(mrus-fusion-viewer PRIVATE src)
target_link_libraries(mrus-fusion-viewer PRIVATE ${ITK_LIBRARIES} Qt6::Widgets)

# The project ships its own demo data (see data/) so the repo still runs after a fresh clone
# on another machine/path — never hardcode an absolute path in source, use this macro instead.
target_compile_definitions(mrus-fusion-viewer PRIVATE MRUS_VIEWER_DATA_DIR="${CMAKE_SOURCE_DIR}/data")
