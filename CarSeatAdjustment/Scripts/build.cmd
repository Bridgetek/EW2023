
set PICO_SDK_PATH=C:\\tuan.nguyen\\ftdi\\4.external_git\\pico-sdk
set PICO_TOOLCHAIN_PATH=C:\\Program Files (x86)\\GNU Arm Embedded Toolchain\\10 2020-q4-major\\bin
mkdir build
mkdir build\\build_mm2040
cd build\\build_mm2040
"C:\Program Files\CMake\bin\cmake.exe" -G "NMake Makefiles" -DEVE_APPS_PLATFORM=RP2040 -DEVE_APPS_GRAPHICS=BT817 -DEVE_APPS_DISPLAY=EVE_DISPLAY_USP -DCMAKE_BUILD_TYPE=Debug ..\\.. && nmake
