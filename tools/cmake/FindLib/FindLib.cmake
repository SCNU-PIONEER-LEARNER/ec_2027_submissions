include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/.deps)

set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/ThirdParty)

# CMake 下载依赖用的是内置 libcurl，它只认 http_proxy / https_proxy 环境变量，
# 不读 git 的 http.proxy 配置。国内网络如通过本地代理科学上网，配置一次即可：
#   cmake -B build -G Ninja -DPINYCORE_PROXY=http://127.0.0.1:10808
# 下载子进程（.deps/*-subbuild 中的 cmake -P）由 configure 进程派生，会继承这里的设置。
set(PINYCORE_PROXY
    ""
    CACHE STRING "下载第三方库时使用的 HTTP(S) 代理，例如 http://127.0.0.1:10808")

if(PINYCORE_PROXY)
  set(ENV{http_proxy} "${PINYCORE_PROXY}")
  set(ENV{https_proxy} "${PINYCORE_PROXY}")
  set(ENV{HTTP_PROXY} "${PINYCORE_PROXY}")
  set(ENV{HTTPS_PROXY} "${PINYCORE_PROXY}")
  message(STATUS "download proxy: ${PINYCORE_PROXY}")
endif()

message("-------------------- find lib -----------------------")

include("${CMAKE_SOURCE_DIR}/tools/cmake/FindLib/FreeRTOS.cmake")
include("${CMAKE_SOURCE_DIR}/tools/cmake/FindLib/Segger.cmake")
include("${CMAKE_SOURCE_DIR}/tools/cmake/FindLib/CMSIS_DSP.cmake")
# include("${CMAKE_SOURCE_DIR}/tools/cmake/FindLib/TinyMPC.cmake")
include("${CMAKE_SOURCE_DIR}/tools/cmake/FindLib/etl.cmake")
include("${CMAKE_SOURCE_DIR}/tools/cmake/FindLib/magicEnum.cmake")

