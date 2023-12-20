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

namespace juce::build_tools
{

    StringArray getVersionSegments (StringRef p);

    int getVersionAsHexIntegerFromParts (const StringArray& versionString);
    int getVersionAsHexInteger (StringRef versionString);

    String getVersionAsHex (StringRef versionString);

} // namespace juce::build_tools
