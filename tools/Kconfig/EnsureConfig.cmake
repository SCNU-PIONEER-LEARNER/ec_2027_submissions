if(EXISTS "${SOURCE_DIR}/.config")
  return()
endif()

if(NOT PYTHON_EXECUTABLE)
  message(FATAL_ERROR "Python was not found; cannot create ${SOURCE_DIR}/.config")
endif()

execute_process(
  COMMAND "${PYTHON_EXECUTABLE}" -m alldefconfig "${KCONFIG_FILE}"
  WORKING_DIRECTORY "${SOURCE_DIR}"
  RESULT_VARIABLE CONFIG_RESULT
  OUTPUT_VARIABLE CONFIG_OUTPUT
  ERROR_VARIABLE CONFIG_ERROR)

if(NOT CONFIG_RESULT EQUAL 0)
  message(FATAL_ERROR
    "Failed to create ${SOURCE_DIR}/.config with alldefconfig:\n${CONFIG_ERROR}")
endif()
