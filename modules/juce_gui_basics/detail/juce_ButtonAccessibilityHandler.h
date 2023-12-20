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

namespace juce::detail
{

//==============================================================================
class ButtonAccessibilityHandler  : public AccessibilityHandler
{
public:
    ButtonAccessibilityHandler (Button& buttonToWrap, AccessibilityRole roleIn)
        : AccessibilityHandler (buttonToWrap,
                                isRadioButton (buttonToWrap) ? AccessibilityRole::radioButton : roleIn,
                                getAccessibilityActions (buttonToWrap),
                                getAccessibilityInterfaces (buttonToWrap)),
          button (buttonToWrap)
    {}


    AccessibleState getCurrentState() const override
    {
        auto state = AccessibilityHandler::getCurrentState();

        if (button.isToggleable())
        {
            state = state.withCheckable();

            if (button.getToggleState())
                state = state.withChecked();
        }

        return state;
    }


    String getTitle() const override
    {
        auto title = AccessibilityHandler::getTitle();

        if (title.isEmpty())
            return button.getButtonText();

        return title;
    }

    String getHelp() const override
    {
        return button.getTooltip();
    }


private:
    class ButtonValueInterface  : public AccessibilityTextValueInterface
    {
    public:
        explicit ButtonValueInterface (Button& buttonToWrap)
            : button (buttonToWrap)
        {
        }

        bool isReadOnly() const override                 { return true; }
        String getCurrentValueAsString() const override  { return button.getToggleState() ? "On" : "Off"; }
        void setValueAsString (const String&) override   {}

    private:
        Button& button;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButtonValueInterface)
    };

    static bool isRadioButton (const Button& button) noexcept
    {
        return button.getRadioGroupId() != 0;
    }

    static AccessibilityActions getAccessibilityActions (Button& button)
    {
        auto actions = AccessibilityActions().addAction (AccessibilityActionType::press,
                                                         [&button] { button.triggerClick(); });

        if (button.isToggleable())
            actions = actions.addAction (AccessibilityActionType::toggle,
                                         [&button] { button.setToggleState (! button.getToggleState(), sendNotification); });

        return actions;
    }

    static Interfaces getAccessibilityInterfaces (Button& button)
    {
        if (button.isToggleable())
            return { std::make_unique<ButtonValueInterface> (button) };

        return {};
    }

    Button& button;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButtonAccessibilityHandler)
};

} // namespace juce::detail
