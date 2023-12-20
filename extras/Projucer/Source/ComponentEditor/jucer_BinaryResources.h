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

class JucerDocument;

//==============================================================================
/**
    Manages a list of binary data objects that a JucerDocument wants to embed in
    the code it generates.
*/
class BinaryResources
{
public:
    //==============================================================================
    BinaryResources& operator= (const BinaryResources& other);

    void loadFromCpp (const File& cppFileLocation, const String& cpp);

    //==============================================================================
    struct BinaryResource
    {
        String name;
        String originalFilename;
        MemoryBlock data;
        std::unique_ptr<Drawable> drawable;
    };

    void clear();
    bool add (const String& name, const File& file);
    void add (const String& name, const String& originalFileName, const MemoryBlock& data);
    void remove (int index);
    bool reload (int index);
    void browseForResource (const String& title, const String& wildcard,
                            const File& fileToStartFrom, const String& resourceToReplace,
                            std::function<void (String)> callback);

    String findUniqueName (const String& rootName) const;

    int size() const noexcept                                            { return resources.size(); }
    const BinaryResource* operator[] (const int index) const noexcept    { return resources [index]; }

    const BinaryResource* getResource (const String& resourceName) const;
    const BinaryResource* getResourceForFile (const File& file) const;

    StringArray getResourceNames() const;

    const Drawable* getDrawable (const String& name) const;
    Image getImageFromCache (const String& name) const;

    template <class ElementComparator>
    void sort (ElementComparator& sorter)
    {
        resources.sort (sorter, true);
        changed();
    }

    //==============================================================================
    void setDocument (JucerDocument* const doc)                   { document = doc; }
    JucerDocument* getDocument() const noexcept                   { return document; }

    void fillInGeneratedCode (GeneratedCode& code) const;

private:
    //==============================================================================
    BinaryResource* findResource (const String& name) const noexcept;
    void changed();

    //==============================================================================
    JucerDocument* document;
    OwnedArray<BinaryResource> resources;
    std::unique_ptr<FileChooser> chooser;
    ScopedMessageBox messageBox;

    //==============================================================================
    JUCE_DECLARE_WEAK_REFERENCEABLE (BinaryResources)
};
