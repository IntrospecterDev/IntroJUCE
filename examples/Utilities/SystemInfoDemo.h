/*
  ==============================================================================

   This file is part of the JUCE examples.
   Copyright (c) Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             SystemInfoDemo
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Displays system information.

 dependencies:     juce_core, juce_data_structures, juce_events, juce_graphics,
                   juce_gui_basics
 exporters:        xcode_mac, vs2022, linux_make, androidstudio, xcode_iphone

 moduleFlags:      JUCE_STRICT_REFCOUNTEDPOINTER=1

 type:             Component
 mainClass:        SystemInfoDemo

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once

#include "../Assets/DemoUtilities.h"

//==============================================================================
static String getMacAddressList()
{
    String addressList;

    for (auto& addr : MACAddress::getAllAddresses())
        addressList << addr.toString() << newLine;

    return addressList;
}

static String getFileSystemRoots()
{
    Array<File> roots;
    File::findFileSystemRoots (roots);

    StringArray rootList;
    for (auto& r : roots)
        rootList.add (r.getFullPathName());

    return rootList.joinIntoString (", ");
}

static String getIPAddressList()
{
    String addressList;

    for (auto& addr : IPAddress::getAllAddresses())
        addressList << "   " << addr.toString() << newLine;

    return addressList;
}

static const char* getDisplayOrientation()
{
    switch (Desktop::getInstance().getCurrentOrientation())
    {
        case Desktop::upright:              return "Upright";
        case Desktop::upsideDown:           return "Upside-down";
        case Desktop::rotatedClockwise:     return "Rotated Clockwise";
        case Desktop::rotatedAntiClockwise: return "Rotated Anti-clockwise";
        case Desktop::allOrientations:      return "All";
        default: jassertfalse; break;
    }

    return nullptr;
}

static String getDisplayInfo()
{
    auto& displays = Desktop::getInstance().getDisplays();

    String displayDesc;

    for (int i = 0; i < displays.displays.size(); ++i)
    {
        auto display = displays.displays.getReference (i);

        displayDesc << "Display " << (i + 1) << (display.isMain ? " (main)" : "") << ":" << newLine
                    << "  Total area: " << display.totalArea.toString() << newLine
                    << "  User area:  " << display.userArea .toString() << newLine
                    << "  DPI: "        << display.dpi   << newLine
                    << "  Scale: "      << display.scale << newLine
                    << newLine;
    }


    displayDesc << "Orientation: " << getDisplayOrientation() << newLine;

    return displayDesc;
}

static String boolString (bool b)
{
    return b ? "yes" : "no";
}

static String getAllSystemInfo()
{
    String systemInfo;

    systemInfo
      << "Here are a few system statistics..." << newLine
      << newLine
      << "Time and date:    " << Time::getCurrentTime().toString (true, true) << newLine
      << "System up-time:   " << RelativeTime::milliseconds ((int64) Time::getMillisecondCounterHiRes()).getDescription() << newLine
      << "Compilation date: " << Time::getCompilationDate().toString (true, false) << newLine
      << newLine
      << "Operating system: " << SystemStats::getOperatingSystemName() << newLine
      << "Host name:        " << SystemStats::getComputerName()        << newLine
      << "Device type:      " << SystemStats::getDeviceDescription()   << newLine
      << "Manufacturer:     " << SystemStats::getDeviceManufacturer()  << newLine
      << "Device ID:        " << SystemStats::getUniqueDeviceID()      << newLine
      << "User logon name:  " << SystemStats::getLogonName()           << newLine
      << "Full user name:   " << SystemStats::getFullUserName()        << newLine
      << "User region:      " << SystemStats::getUserRegion()          << newLine
      << "User language:    " << SystemStats::getUserLanguage()        << newLine
      << "Display language: " << SystemStats::getDisplayLanguage()     << newLine
      << newLine;

    systemInfo
      << "Number of logical CPUs:  " << SystemStats::getNumCpus() << newLine
      << "Number of physical CPUs: " << SystemStats::getNumPhysicalCpus() << newLine
      << "Memory size:             " << SystemStats::getMemorySizeInMegabytes() << " MB" << newLine
      << "CPU vendor:              " << SystemStats::getCpuVendor() << newLine
      << "CPU model:               " << SystemStats::getCpuModel()  << newLine
      << "CPU speed:               " << SystemStats::getCpuSpeedInMegahertz() << " MHz" << newLine
      << "CPU has MMX:             " << boolString (SystemStats::hasMMX()             ) << newLine
      << "CPU has FMA3:            " << boolString (SystemStats::hasFMA3()            ) << newLine
      << "CPU has FMA4:            " << boolString (SystemStats::hasFMA4()            ) << newLine
      << "CPU has SSE:             " << boolString (SystemStats::hasSSE()             ) << newLine
      << "CPU has SSE2:            " << boolString (SystemStats::hasSSE2()            ) << newLine
      << "CPU has SSE3:            " << boolString (SystemStats::hasSSE3()            ) << newLine
      << "CPU has SSSE3:           " << boolString (SystemStats::hasSSSE3()           ) << newLine
      << "CPU has SSE4.1:          " << boolString (SystemStats::hasSSE41()           ) << newLine
      << "CPU has SSE4.2:          " << boolString (SystemStats::hasSSE42()           ) << newLine
      << "CPU has 3DNOW:           " << boolString (SystemStats::has3DNow()           ) << newLine
      << "CPU has AVX:             " << boolString (SystemStats::hasAVX()             ) << newLine
      << "CPU has AVX2:            " << boolString (SystemStats::hasAVX2()            ) << newLine
      << "CPU has AVX512F:         " << boolString (SystemStats::hasAVX512F()         ) << newLine
      << "CPU has AVX512BW:        " << boolString (SystemStats::hasAVX512BW()        ) << newLine
      << "CPU has AVX512CD:        " << boolString (SystemStats::hasAVX512CD()        ) << newLine
      << "CPU has AVX512DQ:        " << boolString (SystemStats::hasAVX512DQ()        ) << newLine
      << "CPU has AVX512ER:        " << boolString (SystemStats::hasAVX512ER()        ) << newLine
      << "CPU has AVX512IFMA:      " << boolString (SystemStats::hasAVX512IFMA()      ) << newLine
      << "CPU has AVX512PF:        " << boolString (SystemStats::hasAVX512PF()        ) << newLine
      << "CPU has AVX512VBMI:      " << boolString (SystemStats::hasAVX512VBMI()      ) << newLine
      << "CPU has AVX512VL:        " << boolString (SystemStats::hasAVX512VL()        ) << newLine
      << "CPU has AVX512VPOPCNTDQ: " << boolString (SystemStats::hasAVX512VPOPCNTDQ() ) << newLine
      << "CPU has Neon:            " << boolString (SystemStats::hasNeon()            ) << newLine
      << newLine;

   #if JUCE_MAC
    systemInfo
      << "Application sandbox enabled: " << boolString (SystemStats::isAppSandboxEnabled()) << newLine
      << newLine;
   #endif

    systemInfo
      << "Current working directory:  " << File::getCurrentWorkingDirectory().getFullPathName() << newLine
      << "Current application file:   " << File::getSpecialLocation (File::currentApplicationFile).getFullPathName() << newLine
      << "Current executable file:    " << File::getSpecialLocation (File::currentExecutableFile) .getFullPathName() << newLine
      << "Invoked executable file:    " << File::getSpecialLocation (File::invokedExecutableFile) .getFullPathName() << newLine
      << newLine;

    systemInfo
      << "User home folder:               " << File::getSpecialLocation (File::userHomeDirectory)             .getFullPathName() << newLine
      << "User desktop folder:            " << File::getSpecialLocation (File::userDesktopDirectory)          .getFullPathName() << newLine
      << "User documents folder:          " << File::getSpecialLocation (File::userDocumentsDirectory)        .getFullPathName() << newLine
      << "User application data folder:   " << File::getSpecialLocation (File::userApplicationDataDirectory)  .getFullPathName() << newLine
      << "User music folder:              " << File::getSpecialLocation (File::userMusicDirectory)            .getFullPathName() << newLine
      << "User movies folder:             " << File::getSpecialLocation (File::userMoviesDirectory)           .getFullPathName() << newLine
      << "User pictures folder:           " << File::getSpecialLocation (File::userPicturesDirectory)         .getFullPathName() << newLine
      << "Common application data folder: " << File::getSpecialLocation (File::commonApplicationDataDirectory).getFullPathName() << newLine
      << "Common documents folder:        " << File::getSpecialLocation (File::commonDocumentsDirectory)      .getFullPathName() << newLine
      << "Local temp folder:              " << File::getSpecialLocation (File::tempDirectory)                 .getFullPathName() << newLine
      << newLine;

    systemInfo
      << "File System roots: "          << getFileSystemRoots() << newLine
      << "Free space in home folder: "  << File::descriptionOfSizeInBytes (File::getSpecialLocation (File::userHomeDirectory)
                                                                                .getBytesFreeOnVolume()) << newLine
      << newLine
      << getDisplayInfo() << newLine
      << "Network IP addresses: "       << newLine << getIPAddressList()  << newLine
      << "Network card MAC addresses: " << newLine << getMacAddressList() << newLine;

    DBG (systemInfo);
    return systemInfo;
}

class SystemInfoDemo final : public Component
{
public:
    SystemInfoDemo()
    {
        addAndMakeVisible (resultsBox);
        resultsBox.setReadOnly (true);
        resultsBox.setMultiLine (true);
        resultsBox.setColour (TextEditor::backgroundColourId, Colours::transparentBlack);
        resultsBox.setFont ({ Font::getDefaultMonospacedFontName(), 12.0f, Font::plain });
        resultsBox.setText (getAllSystemInfo());

        setSize (500, 500);
    }

    void paint (Graphics& g) override
    {
        g.fillAll (getUIColourIfAvailable (LookAndFeel_V4::ColourScheme::UIColour::windowBackground,
                                           Colour::greyLevel (0.93f)));
    }

    void resized() override
    {
        resultsBox.setBounds (getLocalBounds().reduced (8));
    }

private:
    TextEditor resultsBox;

    void lookAndFeelChanged() override
    {
        resultsBox.applyFontToAllText (resultsBox.getFont());
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SystemInfoDemo)
};
