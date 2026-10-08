cmake_minimum_required(VERSION 3.16)
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
