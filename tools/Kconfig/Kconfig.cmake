find_package(Python REQUIRED COMPONENTS Interpreter)

# kconfiglib 以 Python 模块形式提供 menuconfig/defconfig/alldefconfig 等工具。
# 这里直接通过 ${Python_EXECUTABLE} 运行对应模块，避免 PATH 中残留的旧版可执行
# 文件与当前 Python 解释器不一致（例如缺失 curses 支持）导致 menuconfig 无法启动。
set(KCONFIG_MCONF_CMD      "${Python_EXECUTABLE}" -m menuconfig)
set(KCONFIG_DEFCONF_CMD    "${Python_EXECUTABLE}" -m defconfig)
set(KCONFIG_ALLDEFCONF_CMD "${Python_EXECUTABLE}" -m alldefconfig)

execute_process(
    COMMAND "${Python_EXECUTABLE}" -c "import kconfiglib"
    RESULT_VARIABLE KCONFIGLIB_RESULT
    OUTPUT_QUIET
    ERROR_QUIET
)
if(NOT KCONFIGLIB_RESULT EQUAL 0)
    message(FATAL_ERROR "Could not find kconfiglib,\
                        please use 'pip install kconfiglib' to install it")
endif()
message(STATUS "Found menuconfig via ${Python_EXECUTABLE}")

set(PYTHON_SCRIPT "${CMAKE_SOURCE_DIR}/tools/script/Kconfig2h.py")
set(ENSURE_CONFIG_SCRIPT "${CMAKE_SOURCE_DIR}/tools/Kconfig/EnsureConfig.cmake")

if(EXISTS "${CMAKE_SOURCE_DIR}/.config_editing")
    set(EDIT_CONFIG TRUE)
else()
    set(EDIT_CONFIG FALSE)
endif()

add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove .config
    COMMAND ${CMAKE_COMMAND} -E remove "${CMAKE_SOURCE_DIR}/.config_editing"
    COMMENT "Removing build/ and .config"
)

add_custom_target(menuconfig
  COMMAND ${CMAKE_COMMAND}
          -DPYTHON_EXECUTABLE="${Python_EXECUTABLE}"
          -DKCONFIG_FILE=${CMAKE_SOURCE_DIR}/Kconfig
          -DSOURCE_DIR=${CMAKE_SOURCE_DIR}
          -P ${ENSURE_CONFIG_SCRIPT}
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/.config" "${CMAKE_SOURCE_DIR}/.config_editing"
  COMMAND ${KCONFIG_MCONF_CMD} "${CMAKE_SOURCE_DIR}/Kconfig"
  COMMAND ${Python_EXECUTABLE} ${PYTHON_SCRIPT}
  COMMAND ${CMAKE_COMMAND} -B "${CMAKE_BINARY_DIR}" -G Ninja
  COMMAND ${CMAKE_COMMAND} -E remove "${CMAKE_SOURCE_DIR}/.config_editing"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Launching menuconfig (Kconfig interface)"
  USES_TERMINAL
)

function (apply_config config_name)
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/${config_name}")
        message(FATAL_ERROR "${CMAKE_SOURCE_DIR}/${config_name} not found!")
    endif()
    message(STATUS "Applying configuration: ${config_name}")
    execute_process(
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND ${KCONFIG_DEFCONF_CMD} ${config_name}
        ERROR_VARIABLE kconfig_error
        RESULT_VARIABLE kconfig_result
    )
    if(NOT kconfig_result EQUAL 0)
        message(FATAL_ERROR 
            "Failed to apply configuration: ${config_name}\n"
            "Error: ${kconfig_error}"
        )
    endif()
    message(STATUS "Successfully generate ${CMAKE_SOURCE_DIR}/.config")

    execute_process(
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND ${Python_EXECUTABLE} ${PYTHON_SCRIPT} "${CMAKE_SOURCE_DIR}/.config"
        ERROR_VARIABLE py_error
        RESULT_VARIABLE py_result
    )
    if(NOT py_result EQUAL 0)
        message(FATAL_ERROR 
            "Failed to generate sdkconfig files\n"
            "Error: ${py_error}"
        )
    endif()
    unset(CONFIG_NAME CACHE)
endfunction()

if(DEFINED CONFIG_NAME AND NOT EDIT_CONFIG)
  apply_config(${CONFIG_NAME})
elseif(NOT EXISTS "${CMAKE_BINARY_DIR}/build.ninja"
     AND NOT EXISTS "${CMAKE_SOURCE_DIR}/Src/Config/sdkconfig.h")
  message(WARNING "No found build, Using default config")

    execute_process(
        COMMAND ${KCONFIG_ALLDEFCONF_CMD} ${CMAKE_SOURCE_DIR}/Kconfig
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    execute_process(
        COMMAND ${Python_EXECUTABLE} ${PYTHON_SCRIPT} "${CMAKE_SOURCE_DIR}/.config"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        ERROR_VARIABLE kconfig_error
        RESULT_VARIABLE kconfig_result
    )
    if(NOT kconfig_result EQUAL 0)
        message(FATAL_ERROR 
            "Failed to apply configuration from ${CMAKE_SOURCE_DIR}/Kconfig\n"
            "Error: ${kconfig_error}"
        )
    endif()
endif()
