#include "timestamp_interpolator_service/timestamp_interpolator_service.h"

#include <lms/math/interpolation.h>
#include <timestamp_interpolator_service/timestamp_interpolator_service.h>

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

TimestampInterpolatorService::Timestamp TimestampInterpolatorService::canonical(
        const TimestampInterpolatorService::Clock& clock, const TimestampInterpolatorService::Timestamp timestamp)
{
    auto it = offsets.find(clock);
    if(it == offsets.end()) {
        // No canonical offset information added yet
        ClockOffset offset;
        offset.offset = lms::Time::ZERO;
        offset.local = timestamp;
        offsets[clock] = offset;
        it = offsets.find(clock);
    } else {
        // Found offset
        if(timestamp < it->second.local/2) {
            // Overflow has occured
            it->second.offset += (it->second.local - timestamp + lms::Time::fromMicros(1));
            // Set new local/relative timestamp
            it->second.local = timestamp;
        } else if(timestamp > it->second.local) {
            // Adjust highest local/relative timestamp yet observed
            it->second.local = timestamp;
        }
    }

    // Return offset-adjusted timestamp
    return it->second.offset + timestamp;

}
} // namespace timestamp_interpolator_service
