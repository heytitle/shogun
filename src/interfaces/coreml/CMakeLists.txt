set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_library(shogun-coreml SHARED CoreMLModel.cc CoreMLConverter.cc KernelConverter.cc SVMConverter.cc)
set_property(TARGET shogun-coreml PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(shogun-coreml shogun::shogun)

IF (PROTOBUF_FOUND AND ENABLE_PROTOBUF)
	FILE(GLOB COREML_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")
	FILE(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
	SET(COREML_SOURCES)
	FOREACH (SRC_FILE ${COREML_SRC})
		GET_FILENAME_COMPONENT(SRC_FILE_WE ${SRC_FILE} NAME_WE)
		SET(COMPILED_SRC "${CMAKE_CURRENT_BINARY_DIR}/${SRC_FILE_WE}")
		SET(COMPILED_SRC_H "${COMPILED_SRC}.pb.h")
		SET(COMPILED_SRC_CPP "${COMPILED_SRC}.pb.cc")
		ADD_CUSTOM_COMMAND(
		    OUTPUT "${COMPILED_SRC_H}" "${COMPILED_SRC_CPP}"
		    COMMAND "${PROTOBUF_PROTOC_EXECUTABLE}"
		            "${SRC_FILE}"
		            --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
		            --proto_path "${CMAKE_CURRENT_SOURCE_DIR}"
		    COMMENT "Compiling CoreML Protobuf for ${SRC_FILE_WE}"
		)
		SET(PROTO_TARGET_NAME "CoreML_${SRC_FILE_WE}")
		ADD_CUSTOM_TARGET("${PROTO_TARGET_NAME}" DEPENDS "${COMPILED_SRC_H}" "${COMPILED_SRC_CPP}")
		LIST(APPEND COREML_SOURCES ${COMPILED_SRC_CPP})
	ENDFOREACH()
	add_library(coreml STATIC ${COREML_SOURCES})
	target_link_libraries(shogun-coreml coreml ${Protobuf_LITE_LIBRARIES})
ELSE()
	MESSAGE(FATAL_ERROR "Protobuf is required for CoreML")
ENDIF()

IF (ENABLE_TESTING)
	include(external/GoogleTestNMock)
	ExternalProject_Get_Property(GoogleMock source_dir)
	LINK_DIRECTORIES(${THIRD_PARTY_DIR}/libs/gmock)

	enable_testing()
	add_library(coreml_unittest_main ${CMAKE_CURRENT_SOURCE_DIR}/unittest_main.cc)
	target_link_libraries(coreml_unittest_main gmock gtest shogun::shogun)
	add_dependencies(coreml_unittest_main GoogleMock shogun::shogun)
	target_include_directories(coreml_unittest_main
		PRIVATE
		${source_dir}/googlemock/include
		${source_dir}/googletest/include)
	set(SHOGUN_UNITTEST_LINK_LIBS coreml_unittest_main shogun::shogun shogun-coreml gmock gtest)

	ADD_SHOGUN_UNITTEST(TARGET SVMConverter_test LABELS coreml-unit)
	ADD_SHOGUN_UNITTEST(TARGET KernelConverter_test LABELS coreml-unit)
ENDIF()