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
public:
    //! Timestamp type
    typedef lms::Time Timestamp;
    //! Clock identifier type
    typedef std::string Clock;

protected:
    //! Pair of clock identifiers as key for synchronization points container
    typedef std::pair<Clock, Clock> ClockPair;
    //! Pair of timestamps from two clocks
    typedef std::pair<Timestamp, Timestamp> TimestampPair;
    //! Start and End pair of clock sync points
    typedef std::vector<TimestampPair> SyncPointList;
    typedef std::map<ClockPair, SyncPointList> Container;

    typedef struct {
        Timestamp offset;
        Timestamp local;
    } ClockOffset;

    typedef std::map<Clock, ClockOffset> OffsetContaner;

public:
    /**
     * @brief Create synchronization point for two clocks
     *
     * @param aClock Identifier of the first clock
     * @param bClock Identifier of the second clock
     * @param aTime Synchronization timestamp of the first clock
     * @param bTime Synchronization timestamp of the second clock
     */
    void sync(const Clock& aClock, const Clock& bClock, const Timestamp& aTime, const Timestamp& bTime );

    /**
     * @brief Interpolate timestamp from one clock to another clock
     *
     * @note If no sync points are available for the clock combination, the input timestamp is returned
     *
     * @param from The clock identifier to convert the timestmap from
     * @param to The clock identifier to convert the timestamp to
     * @param timestamp the clock identifier of the source clock to convert
     * @return Interpolated timestamp of the target clock
     */
    Timestamp interpolate(const Clock& from, const Clock& to, const Timestamp& timestamp) const;

    /**
     * @brief Calculate clock drift of a time base w.r.t a reference clock
     *
     * @param reference The reference clock
     * @param clock The other clock
     * @return The drift value (0 = no drift, > 0 clock is faster than ref, < 0 clock is slower than ref)
     */
    template<typename T = double>
    T drift(const Clock& reference, const Clock& clock) const
    {
        Timestamp refStart, refEnd, clockStart, clockEnd;
        if(!getSyncPoints(reference, clock, refStart, clockStart, refEnd, clockEnd))
        {
            // No sync points found
            return T(0);
        }

        auto clockDiff = clockEnd - clockStart;
        auto refDiff = refEnd - refStart;

        auto rate = clockDiff.toFloat<std::micro, T>() / refDiff.toFloat<std::micro, T>();

        return ( rate - T(1) );
    }

    /**
     * @brief Calculate canonical timestamp for a overflowing timebase clock
     *
     * @param clock The clock of the timebase
     * @param timestamp The potentially overflowing timestamp from timebase
     * @return The canonical overflow-compensated timestamp
     */
    Timestamp canonical(const Clock& clock, const Timestamp timestamp);

protected:

    /**
     * @brief Clock pair creation
     *
     * This implementation ensures that clock pairs (as map keys) are unique
     */
    inline static ClockPair makeClockPair(const Clock& a, const Clock& b, bool& switched)
    {
        switched = (a > b);
        if(!switched)
        {
            return std::make_pair(a, b);
        }
        else
        {
            return std::make_pair(b, a);
        }
    }

    /**
     * @brief Get start and end synchronization points
     * @return true on success, false if no sync points could be found
     */
    bool getSyncPoints(const Clock& a, const Clock& b,
                       Timestamp& aStart, Timestamp& bStart,
                       Timestamp& aEnd, Timestamp& bEnd) const
    {
        bool switched;
        auto key = makeClockPair(a, b, switched);

        const auto it = syncPoints.find(key);
        if(it == syncPoints.end())
        {
            // No sync point found
            return false;
        }

        const auto& vec = it->second;
        if(vec.size() == 0)
        {
            // No sync points
            return false;
        }

        std::tie(aStart, bStart) = vec.front();
        std::tie(aEnd, bEnd) = vec.back();

        if(switched)
        {
            // Swap from and to fields
            std::swap(aStart, bStart);
            std::swap(aEnd, bEnd);
        }

        return true;
    }
protected:
    //! Container to store synchronization points
    Container syncPoints;
    //! Canonical timestamp offsets
    OffsetContaner offsets;
};

} // namespace timestamp_interpolator_service

#endif // TIMESTAMP_INTERPOLATOR_SERVICE_H
