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

#include "jucer_ComponentOverlayComponent.h"
#include "../jucer_JucerDocument.h"
#include "jucer_SnapGridPainter.h"

//==============================================================================
class ComponentLayoutEditor  : public Component,
                               public FileDragAndDropTarget,
                               public DragAndDropTarget,
                               public LassoSource<Component*>,
                               private ChangeListener
{
public:
    //==============================================================================
    ComponentLayoutEditor (JucerDocument&, ComponentLayout&);
    ~ComponentLayoutEditor() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void visibilityChanged() override;

    void mouseDown (const MouseEvent&) override;
    void mouseDrag (const MouseEvent&) override;
    void mouseUp (const MouseEvent&) override;
    bool keyPressed (const KeyPress&) override;

    bool isInterestedInFileDrag (const StringArray& files) override;
    void filesDropped (const StringArray& filenames, int x, int y) override;

    bool isInterestedInDragSource (const SourceDetails& dragSourceDetails) override;
    void itemDropped (const SourceDetails& dragSourceDetails) override;


    ComponentLayout& getLayout() const noexcept                 { return layout; }

    void findLassoItemsInArea (Array <Component*>& results, const Rectangle<int>& area) override;

    SelectedItemSet<Component*>& getLassoSelection() override;

    //==============================================================================
    void refreshAllComponents();
    void updateOverlayPositions();

    ComponentOverlayComponent* getOverlayCompFor (Component*) const;

    Rectangle<int> getComponentArea() const;
    Image createComponentLayerSnapshot() const;

private:
    void changeListenerCallback (ChangeBroadcaster*) override;

    JucerDocument& document;
    ComponentLayout& layout;
    Component* subCompHolder;

    LassoComponent<Component*> lassoComp;
    SnapGridPainter grid;
    bool firstResize;
};
