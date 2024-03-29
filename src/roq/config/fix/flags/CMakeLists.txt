set(TARGET_NAME ${PROJECT_NAME}-flags)

set(SOURCES flags.cpp)

include(RoqAutogen)

set(NAMESPACE "roq/config/fix/flags")

set(AUTOGEN_SCHEMAS flags.json)

if(BUILD_DOCS)

  roq_autodoc(
    OUTPUT
    AUTOGEN_DOCS
    NAMESPACE
    ${NAMESPACE}
    OUTPUT_TYPE
    "doc"
    SOURCES
    ${AUTOGEN_SCHEMAS})

  add_custom_target(flags-autogen-docs ALL DEPENDS ${AUTOGEN_DOCS})

endif()

roq_autogen(
  OUTPUT
  AUTOGEN_HEADERS
  NAMESPACE
  ${NAMESPACE}
  OUTPUT_TYPE
  "flags"
  FILE_TYPE
  "hpp"
  SOURCES
  ${AUTOGEN_SCHEMAS})

add_custom_target(${TARGET_NAME}-autogen-headers ALL DEPENDS ${AUTOGEN_HEADERS})

roq_autogen(
  OUTPUT
  AUTOGEN_SOURCES
  NAMESPACE
  ${NAMESPACE}
  OUTPUT_TYPE
  "flags"
  FILE_TYPE
  "cpp"
  SOURCES
  ${AUTOGEN_SCHEMAS})

roq_gitignore(
  OUTPUT
  .gitignore
  SOURCES
  ${TARGET_NAME}
  ${AUTOGEN_DOCS}
  ${AUTOGEN_HEADERS}
  ${AUTOGEN_SOURCES})

add_library(${TARGET_NAME} OBJECT ${SOURCES} ${AUTOGEN_SOURCES})

add_dependencies(${TARGET_NAME} ${TARGET_NAME}-autogen-headers)

target_link_libraries(${TARGET_NAME} absl::flags)
