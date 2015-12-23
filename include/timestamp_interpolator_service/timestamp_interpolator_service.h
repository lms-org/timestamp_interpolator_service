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

    // TODO: drift calculation
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
protected:
    //! Container to store synchronization points
    Container syncPoints;
};

} // namespace timestamp_interpolator_service

#endif // TIMESTAMP_INTERPOLATOR_SERVICE_H
