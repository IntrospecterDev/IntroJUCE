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

#pragma once


//==============================================================================
class UTF8Component final : public Component
{
public:
    UTF8Component()
        : desc (String(),
                "Type any string into the box, and it'll be shown below as a portable UTF-8 literal, "
                "ready to cut-and-paste into your source-code...")
    {
        desc.setJustificationType (Justification::centred);
        addAndMakeVisible (desc);

        userText.setMultiLine (true, true);
        userText.setReturnKeyStartsNewLine (true);
        addAndMakeVisible (userText);
        userText.onTextChange = [this] { update(); };
        userText.onEscapeKey  = [this] { getTopLevelComponent()->exitModalState (0); };

        resultText.setFont (getAppSettings().appearance.getCodeFont().withHeight (13.0f));
        resultText.setMultiLine (true, true);
        resultText.setReadOnly (true);
        resultText.setSelectAllWhenFocused (true);
        addAndMakeVisible (resultText);

        userText.setText (getLastText());
    }

    void update()
    {
        getLastText() = userText.getText();
        resultText.setText (CodeHelpers::stringLiteral (getLastText(), 100), false);
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced (8);
        desc.setBounds (r.removeFromTop (44));
        r.removeFromTop (8);
        userText.setBounds (r.removeFromTop (r.getHeight() / 2));
        r.removeFromTop (8);
        resultText.setBounds (r);
    }

    void lookAndFeelChanged() override
    {
        userText.applyFontToAllText (userText.getFont());
        resultText.applyFontToAllText (resultText.getFont());
    }

private:
    Label desc;
    TextEditor userText, resultText;

    String& getLastText()
    {
        static String t;
        return t;
    }
};
