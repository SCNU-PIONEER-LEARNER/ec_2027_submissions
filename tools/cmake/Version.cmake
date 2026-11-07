# 版本号来源：最近的 git tag；仓库还没有任何 tag 时退化为 commit 号。
# 注意：无 tag 时 git describe --tags 会失败并输出空串，若直接交给 project(VERSION ...)
# 会触发 "VERSION keyword not followed by a value" 警告，所以这里必须兜底。
execute_process(
  COMMAND git describe --tags --always --dirty
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_VERSION
  ERROR_QUIET
  OUTPUT_STRIP_TRAILING_WHITESPACE)

if(NOT GIT_VERSION)
  set(GIT_VERSION "unknown")
endif()

# 只从形如 v1.2.3 / v1.2.3-4-gabc1234 的 tag 中提取数字版本。
# commit 号（如 9d5dcf4）构不成合法 version，此时使用默认版本 0.0.0。
string(REGEX MATCH "^v?([0-9]+(\\.[0-9]+)*)" _version_match "${GIT_VERSION}")
set(PROJECT_VERSION "${CMAKE_MATCH_1}")

if(NOT PROJECT_VERSION)
  set(PROJECT_VERSION "0.0.0")
endif()

string(REGEX REPLACE "^v" "" GIT_VERSION "${GIT_VERSION}")
