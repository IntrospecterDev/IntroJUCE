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

#ifdef __INTELLISENSE__

#define JUCE_CORE_INCLUDE_COM_SMART_PTR 1
#define JUCE_WINDOWS                    1

#include <d2d1_2.h>
#include <d3d11_1.h>
#include <dcomp.h>
#include <dwrite.h>
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <windows.h>
#include "juce_ETW_windows.h"
#include "juce_Direct2DHelpers_windows.cpp"

#endif

namespace juce
{

    //==============================================================================

    struct Direct2ImageContext::ImagePimpl : public Direct2DGraphicsContext::Pimpl
    {
    private:
        ComSmartPtr<ID2D1Bitmap1> targetBitmap;
        Point<int> const          origin;

        void updatePaintAreas() override
        {
            paintAreas = getFrameSize();
        }

        JUCE_DECLARE_WEAK_REFERENCEABLE(ImagePimpl)

    public:
        ImagePimpl(Direct2ImageContext& owner_, DirectX::DXGI::Adapter::Ptr adapter_)
            : Pimpl(owner_, false /* opaque */)
        {
            adapter = adapter_;
        }

        ~ImagePimpl() override {}

        void setTargetBitmap(ID2D1Bitmap1* targetBitmap_)
        {
            targetBitmap = targetBitmap_;
        }

        void setScaleFactor(float scale_) override
        {
            Pimpl::setScaleFactor(scale_);

            if (deviceResources.deviceContext.context)
            {
                auto dpi = USER_DEFAULT_SCREEN_DPI * dpiScalingFactor;
                deviceResources.deviceContext.context->SetDpi(dpi, dpi);
            }
        }

        Rectangle<int> getFrameSize() override
        {
            if (targetBitmap)
            {
                auto size = targetBitmap->GetSize();
                return Rectangle<float>{ size.width, size.height }.getSmallestIntegerContainer();
            }

            return {};
        }

        ID2D1Image* getDeviceContextTarget() override
        {
            return targetBitmap;
        }

    };

    //==============================================================================

    Direct2ImageContext::Direct2ImageContext(DirectX::DXGI::Adapter::Ptr adapter_, bool clearImage_) :
        pimpl(new ImagePimpl{ *this, adapter_ }),
        clearImage(clearImage_)
    {
    }

    Direct2ImageContext::~Direct2ImageContext()
    {
        endFrame();
    }

    Direct2DGraphicsContext::Pimpl* Direct2ImageContext::getPimpl() const noexcept
    {
        return pimpl.get();
    }

    void Direct2ImageContext::startFrame(ID2D1Bitmap1* bitmap, float dpiScaleFactor)
    {
        pimpl->setTargetBitmap(bitmap);
        pimpl->setScaleFactor(dpiScaleFactor);

        Direct2DGraphicsContext::startFrame();
    }

    void Direct2ImageContext::clearTargetBuffer()
    {
        if (clearImage)
        {
            pimpl->getDeviceContext()->Clear(pimpl->backgroundColor);
        }
    }

} // namespace juce
