set(SOURCES
    "src/timestamp_interpolator_service.cpp"
    "src/interface.cpp"
)

set(HEADERS
    "include/timestamp_interpolator_service/timestamp_interpolator_service.h"
)

include_directories(include)
add_library(timestamp_interpolator_service SHARED ${SOURCES} ${HEADERS})
target_link_libraries(timestamp_interpolator_service PRIVATE lmscore)
