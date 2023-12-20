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
class TextButtonHandler  : public ButtonHandler
{
public:
    TextButtonHandler()
        : ButtonHandler ("Text Button", "juce::TextButton", typeid (TextButton), 150, 24)
    {
        registerColour (juce::TextButton::buttonColourId, "background (normal)", "bgColOff");
        registerColour (juce::TextButton::buttonOnColourId, "background (on)", "bgColOn");
        registerColour (juce::TextButton::textColourOffId, "text colour (normal)", "textCol");
        registerColour (juce::TextButton::textColourOnId, "text colour (on)", "textColOn");
    }

    Component* createNewComponent (JucerDocument*) override
    {
        return new TextButton ("new button", String());
    }

    void getEditableProperties (Component* component, JucerDocument& document,
                                Array<PropertyComponent*>& props, bool multipleSelected) override
    {
        ButtonHandler::getEditableProperties (component, document, props, multipleSelected);

        if (multipleSelected)
            return;

        addColourProperties (component, document, props);
    }

    XmlElement* createXmlFor (Component* comp, const ComponentLayout* layout) override
    {
        return ButtonHandler::createXmlFor (comp, layout);
    }

    bool restoreFromXml (const XmlElement& xml, Component* comp, const ComponentLayout* layout) override
    {
        return ButtonHandler::restoreFromXml (xml, comp, layout);
    }

    void fillInCreationCode (GeneratedCode& code, Component* component, const String& memberVariableName) override
    {
        ButtonHandler::fillInCreationCode (code, component, memberVariableName);

        String s;

        s << getColourIntialisationCode (component, memberVariableName)
          << '\n';

        code.constructorCode += s;
    }
};
