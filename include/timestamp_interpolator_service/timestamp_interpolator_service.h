#ifndef TIMESTAMP_INTERPOLATOR_SERVICE_H
#define TIMESTAMP_INTERPOLATOR_SERVICE_H

#include <lms/service.h>

namespace timestamp_interpolator_service {

/**
 * @brief LMS service timestamp_interpolator_service
 **/
class TimestampInterpolatorService : public lms::Service {
public:
    bool init() override;
    void destroy() override;
};

} // namespace timestamp_interpolator_service

#endif // TIMESTAMP_INTERPOLATOR_SERVICE_H
