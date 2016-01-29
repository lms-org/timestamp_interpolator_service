#include "timestamp_interpolator_service/timestamp_interpolator_service.h"

#include <lms/math/interpolation.h>

namespace timestamp_interpolator_service {

bool TimestampInterpolatorService::init() {
    return true;
}

void TimestampInterpolatorService::destroy() {
}

void TimestampInterpolatorService::sync(const TimestampInterpolatorService::Clock &aClock,
                                        const TimestampInterpolatorService::Clock &bClock,
                                        const TimestampInterpolatorService::Timestamp &aTime,
                                        const TimestampInterpolatorService::Timestamp &bTime)
{
    bool switched;
    auto key = makeClockPair(aClock, bClock, switched);
    auto sync = switched ? std::make_pair(bTime, aTime) : std::make_pair(aTime, bTime);
    auto& vec = syncPoints[key];

    if(vec.size() > 1)
    {
        // Remove last element
        vec.pop_back();
    }

    // Insert new sync point at the end
    vec.push_back(sync);
}

TimestampInterpolatorService::Timestamp TimestampInterpolatorService::interpolate(
        const TimestampInterpolatorService::Clock &from, const TimestampInterpolatorService::Clock &to,
        const TimestampInterpolatorService::Timestamp &timestamp) const
{
    Timestamp fromStart, fromEnd, toStart, toEnd;
    if(!getSyncPoints(from, to, fromStart, toStart, fromEnd, toEnd))
    {
        return timestamp;
    }

    // Compute interpolation
    return lms::Time::fromMicros(lms::math::linearInterpolation<Timestamp::TimeType>(fromStart.micros(), toStart.micros(),
                                                           fromEnd.micros(), toEnd.micros(),
                                                           timestamp.micros()));
}
} // namespace timestamp_interpolator_service
