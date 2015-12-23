# Timestamp Interpolator Service

This services interpolates between timestamps of different timebases (clocks).
Clock synchronization is done by saving the timestamps of two clocks being synced at certain synchronization points.
When converting from one time base to another, the time will be interpolated linearly between the first and the last saved synchronization points.

## Config

```xml
<framework>
    <service>
        <name>TIMESTAMP_INTERPOLATOR</name>
        <realName>timestamp_interpolator_service</realName>
    </service>
</framework>
```

## Usage

### Get service handle
```cpp
{
    auto service = getService<timestamp_interpolator_service::TimestampInterpolatorService>("TIMESTAMP_INTERPOLATOR");
    if(service.isValid())
    {
        // DO STUFF WITH SERVICE HERE
    }
}
```

### Create new synchronization point

```cpp
    // Create new sync point between SYSTEM clock (LMS internal) and some other MY_CLOCK timebase
    service->sync("SYSTEM", "MY_CLOCK", lms::Time::now(), lms::Time::fromMicros(my_clock_timestamp));
```

### Create new synchronization point

```cpp
    // Compute current time in MY_CLOCK timebase
    auto my_clock_now = service->interpolate("SYSTEM", "MY_CLOCK", lms::Time::now()));
```

### Calculate clock drift rate

```cpp
    // Compute drift of MY_CLOCK w.r.t reference clock SYSTEM
    double drift = service->drift("SYSTEM", "MY_CLOCK"));
```
