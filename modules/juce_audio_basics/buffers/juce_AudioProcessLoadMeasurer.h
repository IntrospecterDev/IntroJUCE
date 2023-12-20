/*
  ==============================================================================

   This file is part of the JUCE 8 technical preview.
   Copyright (c) Raw Material Software Limited

   You may use this code under the terms of the GPL v3
   (see www.gnu.org/licenses).

   For the technical preview this file cannot be licensed commercially.

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

//==============================================================================
/**
    Maintains an ongoing measurement of the proportion of time which is being
    spent inside an audio callback.

    @tags{Audio}
*/
class JUCE_API  AudioProcessLoadMeasurer
{
public:
    /** */
    AudioProcessLoadMeasurer();

    /** Destructor. */
    ~AudioProcessLoadMeasurer();

    //==============================================================================
    /** Resets the state. */
    void reset();

    /** Resets the counter, in preparation for use with the given sample rate and block size. */
    void reset (double sampleRate, int blockSize);

    /** Returns the current load as a proportion 0 to 1.0 */
    double getLoadAsProportion() const;

    /** Returns the current load as a percentage 0 to 100.0 */
    double getLoadAsPercentage() const;

    /** Returns the number of over- (or under-) runs recorded since the state was reset. */
    int getXRunCount() const;

    //==============================================================================
    /** This class measures the time between its construction and destruction and
        adds it to an AudioProcessLoadMeasurer.

        e.g.
        @code
        {
            AudioProcessLoadMeasurer::ScopedTimer timer (myProcessLoadMeasurer);
            myCallback->doTheCallback();
        }
        @endcode

        @tags{Audio}
    */
    struct JUCE_API  ScopedTimer
    {
        ScopedTimer (AudioProcessLoadMeasurer&);
        ScopedTimer (AudioProcessLoadMeasurer&, int numSamplesInBlock);
        ~ScopedTimer();

    private:
        AudioProcessLoadMeasurer& owner;
        double startTime;
        int samplesInBlock;

        JUCE_DECLARE_NON_COPYABLE (ScopedTimer)
    };

    /** Can be called manually to add the time of a callback to the stats.
        Normally you probably would never call this - it's simpler and more robust to
        use a ScopedTimer to measure the time using an RAII pattern.
    */
    void registerBlockRenderTime (double millisecondsTaken);

    /** Can be called manually to add the time of a callback to the stats.
        Normally you probably would never call this - it's simpler and more robust to
        use a ScopedTimer to measure the time using an RAII pattern.
    */
    void registerRenderTime (double millisecondsTaken, int numSamples);

private:
    void registerRenderTimeLocked (double, int);

    SpinLock mutex;
    int samplesPerBlock = 0;
    double msPerSample = 0;
    std::atomic<double> cpuUsageProportion { 0 };
    std::atomic<int> xruns { 0 };
};


} // namespace juce
