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

#if ! defined(_WINDEF_) && ! defined(__INTELLISENSE__)
class HWND__; // Forward or never
typedef HWND__* HWND;
#endif

#if JUCE_DIRECT2D_METRICS

namespace direct2d
{

struct PaintStats : public ReferenceCountedObject
{
    enum
    {
        messageThreadPaintDuration,
        frameInterval,
        presentDuration,
        present1Duration,
        swapChainEventInterval,
        swapChainMessageTransitTime,
        swapChainMessageInterval,
        vblankToBeginDraw,
        createGeometryTime,
        createFilledGRTime,
        createStrokedGRTime,

        numStats
    };

    StringArray const accumulatorNames { "messageThreadPaintDuration", "frameInterval",          "presentDuration",
                                         "present1Duration",           "swapChainEventInterval", "swapChainMessageTransitTime",
                                         "swapChainMessageInterval",   "VBlank to BeginDraw",
                                         "Create geometry", "Create filled GR", "Create stroked GR" };

    int64 const  creationTime        = Time::getMillisecondCounter();
    double const millisecondsPerTick = 1000.0 / (double) Time::getHighResolutionTicksPerSecond();
    int          paintCount          = 0;
    int          presentCount        = 0;
    int          present1Count       = 0;
    int64        lastPaintStartTicks = 0;
    uint64       lockAcquireMaxTicks = 0;

    PaintStats() = default;
    ~PaintStats() = default;

    void reset()
    {
        for (auto& accumulator : accumulators)
        {
            accumulator.reset();
        }
        lastPaintStartTicks = 0;
        paintCount          = 0;
        present1Count       = 0;
        lockAcquireMaxTicks = 0;
    }

    using Ptr = ReferenceCountedObjectPtr<PaintStats>;

    StatisticsAccumulator<double>& getAccumulator (int index)
    {
        return accumulators[index];
    }

    void addValueTicks (int index, int64 ticks)
    {
        addValueMsec (index, Time::highResolutionTicksToSeconds (ticks) * 1000.0);
    }

    void addValueMsec (int index, double value)
    {
        auto& accumulator = accumulators[index];

        switch (index)
        {
        case frameInterval:
            if (accumulator.getCount() > 100)
            {
                accumulator.reset();
            }
            break;
        }
        accumulator.addValue (value);
    }

private:
    StatisticsAccumulator<double> accumulators[numStats];
};

struct ScopedElapsedTime
{
    ScopedElapsedTime (PaintStats::Ptr stats_, int accumulatorIndex_)
        : stats (stats_),
          accumulatorIndex (accumulatorIndex_)
    {
    }

    ~ScopedElapsedTime()
    {
        auto finishTicks = Time::getHighResolutionTicks();
        if (stats)
        {
            stats->addValueTicks(accumulatorIndex, finishTicks - startTicks);
        }
    }

    int64           startTicks = Time::getHighResolutionTicks();
    PaintStats::Ptr stats;
    int             accumulatorIndex;
};
} // namespace direct2d

#endif

#if JUCE_ETW_TRACELOGGING

struct ETWEventProvider
{
    ETWEventProvider();
    ~ETWEventProvider();
};

#endif

class Direct2DGraphicsContext : public LowLevelGraphicsContext
{
public:
    Direct2DGraphicsContext();
    ~Direct2DGraphicsContext() override;

    //==============================================================================
    bool isVectorDevice() const override
    {
        return false;
    }

    void setOrigin (Point<int>) override;
    void addTransform (const AffineTransform&) override;
    float getPhysicalPixelScaleFactor() override;
    bool clipToRectangle (const Rectangle<int>&) override;
    bool clipToRectangleList (const RectangleList<int>&) override;
    void excludeClipRectangle (const Rectangle<int>&) override;
    void clipToPath (const Path&, const AffineTransform&) override;
    void clipToImageAlpha (const Image&, const AffineTransform&) override;
    bool clipRegionIntersects (const Rectangle<int>&) override;
    Rectangle<int> getClipBounds() const override;
    bool isClipEmpty() const override;

    //==============================================================================
    void saveState() override;
    void restoreState() override;
    void beginTransparencyLayer (float opacity) override;
    void endTransparencyLayer() override;

    //==============================================================================
    void setFill (const FillType&) override;
    void setOpacity (float) override;
    void setInterpolationQuality (Graphics::ResamplingQuality) override;

    //==============================================================================
    void fillRect (const Rectangle<int>&, bool replaceExistingContents) override;
    void fillRect (const Rectangle<float>&) override;
    void fillRectList (const RectangleList<float>&) override;
    void fillPath (const Path&, const AffineTransform&) override;
    void drawImage (const Image& sourceImage, const AffineTransform&) override;

    //==============================================================================
    void drawLine (const Line<float>&) override;
    void setFont (const Font&) override;
    const Font& getFont() override;
    void drawGlyph (int glyphNumber, const AffineTransform&) override;
    bool drawTextLayout (const AttributedString&, const Rectangle<float>&) override;


    //==============================================================================
    //
    // These methods are not part of the standard LowLevelGraphicsContext; they
    // were added because Direct2D supports these drawing primitives
    //
    // Standard LLGC only supports drawing one glyph at a time; it's much more
    // efficient to pass an entire run of glyphs to the device context
    //
    bool drawLineWithThickness (const Line<float>&, float) override;

    bool drawEllipse (Rectangle<float> area, float lineThickness) override;
    bool fillEllipse (Rectangle<float> area) override;

    bool drawRect (const Rectangle<float>&, float) override;
    bool drawPath (const Path&, const PathStrokeType& strokeType, const AffineTransform&) override;

    bool drawRoundedRectangle (Rectangle<float> area, float cornerSize, float lineThickness) override;
    bool fillRoundedRectangle (Rectangle<float> area, float cornerSize) override;

    bool supportsGlyphRun() override
    {
        return true;
    }
    void drawGlyphRun (Array<PositionedGlyph> const& glyphs,
                       int                           startIndex,
                       int                           numGlyphs,
                       const AffineTransform&        transform,
                       Rectangle<float>              underlineArea) override;

    //==============================================================================
    bool startFrame();
    void endFrame();

    void setPhysicalPixelScaleFactor(float scale_);

    virtual Image createSnapshot(Rectangle<int> /*areaDIPs*/)
    {
        return {};
    }

    virtual Image createSnapshot()
    {
        return {};
    }

#if JUCE_DIRECT2D_METRICS
    direct2d::PaintStats::Ptr paintStats = new direct2d::PaintStats;
#endif

    //==============================================================================
    //
    // Min & max frame sizes; same as Direct3D texture size limits
    //
    static int constexpr minFrameSize = 1;
    static int constexpr maxFrameSize = 16384;


    //==============================================================================
protected:
    struct SavedState;
    SavedState* currentState = nullptr;

    struct Pimpl;
    virtual Pimpl* getPimpl() const noexcept = 0;

    virtual void clearTargetBuffer() = 0;
    void drawGlyphCommon (int numGlyphs, Font const& font, const AffineTransform& transform, Rectangle<float> underlineArea);
    void updateDeviceContextTransform();
    void updateDeviceContextTransform (AffineTransform chainedTransform);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Direct2DGraphicsContext)
};

} // namespace juce
