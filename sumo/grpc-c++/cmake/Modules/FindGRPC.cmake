find_package(PkgConfig)
pkg_check_modules(PC_GRPC grpc++)
set(GRPC_DEFINITIONS ${PC_GRPC_CFLAGS_OTHER})

find_path(GRPC_INCLUDE_DIR grpc++/grpc++.h
        HINTS ${PC_GRPC_INCLUDEDIR} ${PC_GRPC_INCLUDE_DIRS}
        PATHS_SUFIXES grpc++)

find_library(GRPC_LIBRARY NAMES grpc++ libgrpc++
        HINTS ${PC_GRPC_LIBDIR} ${PC_GRPC_LIBRARY_DIRS})

set(GRPC_LIBRARIES ${GRPC_LIBRARY})
set(GRPC_INCLUD_DIRS ${GRPC_INCLUDE_DIR})

message(STATUS  ${GRPC_LIBRARY})
message(STATUS  ${GRPC_INCLUDE_DIR})
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GRPC DEFAULT_MSG
        GRPC_LIBRARY GRPC_INCLUDE_DIR)
mark_as_advanced(GRPC_INCLUDE_DIR GRPC_LIBRARY)