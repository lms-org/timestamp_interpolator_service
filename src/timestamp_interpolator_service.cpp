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
    bool switched;
    auto key = makeClockPair(from, to, switched);

    const auto it = syncPoints.find(key);
    if(it == syncPoints.end())
    {
        // No sync point found
        return timestamp;
    }

    const auto& vec = it->second;
    if(vec.size() == 0)
    {
        // No sync points
        return timestamp;
    }

    Timestamp fromStart, fromEnd, toStart, toEnd;
    std::tie(fromStart, toStart) = vec.front();
    std::tie(fromEnd, toEnd) = vec.back();

    if(switched)
    {
        // Swap from and to fields
        std::swap(fromStart, toStart);
        std::swap(fromEnd, toEnd);
    }

    // Compute interpolation
    Timestamp::TimeType result;
    if(lms::math::linearInterpolation<Timestamp::TimeType>(fromStart.micros(), toStart.micros(),
                                                           fromEnd.micros(), toEnd.micros(),
                                                           timestamp.micros(), result))
    {
        return Timestamp::fromMicros(result);
    }
    else
    {
        // Start and End timestamp of source are identical -> no real interp possible
        // => Just adjust clock offset
        return toEnd + ( timestamp - fromEnd );
    }
}
} // namespace timestamp_interpolator_service
