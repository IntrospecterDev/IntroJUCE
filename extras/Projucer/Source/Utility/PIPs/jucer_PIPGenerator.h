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

#include "../Helpers/jucer_MiscUtilities.h"
#include "../../Project/Modules/jucer_AvailableModulesList.h"

//==============================================================================
class PIPGenerator
{
public:
    PIPGenerator (const File& pipFile, const File& outputDirectory = {},
                  const File& pathToJUCEModules = {}, const File& pathToUserModules = {});

    //==============================================================================
    bool hasValidPIP() const noexcept                   { return ! metadata[Ids::name].toString().isEmpty(); }
    File getJucerFile() const noexcept                  { return outputDirectory.getChildFile (metadata[Ids::name].toString() + ".jucer"); }
    File getPIPFile() const noexcept                    { return useLocalCopy ? outputDirectory.getChildFile ("Source").getChildFile (pipFile.getFileName()) : pipFile; }

    String getMainClassName() const noexcept            { return metadata[Ids::mainClass]; }

    File getOutputDirectory() const noexcept            { return outputDirectory; }

    //==============================================================================
    Result createJucerFile();
    Result createMainCpp();

private:
    //==============================================================================
    void addFileToTree (ValueTree& groupTree, const String& name, bool compile, const String& path);
    void createFiles (ValueTree& jucerTree);
    String getDocumentControllerClass() const;

    ValueTree createModulePathChild (const String& moduleID);
    ValueTree createBuildConfigChild (bool isDebug);
    ValueTree createExporterChild (const Identifier& exporterIdentifier);
    ValueTree createModuleChild (const String& moduleID);

    void addExporters (ValueTree& jucerTree);
    void addModules (ValueTree& jucerTree);

    Result setProjectSettings (ValueTree& jucerTree);

    void setModuleFlags (ValueTree& jucerTree);

    String getMainFileTextForType();

    //==============================================================================
    Array<File> replaceRelativeIncludesAndGetFilesToMove();
    bool copyRelativeFileToLocalSourceDirectory (const File&) const noexcept;

    StringArray getExtraPluginFormatsToBuild() const;

    String getPathForModule (const String&) const;
    File getExamplesDirectory() const;

    //==============================================================================
    File pipFile, outputDirectory, juceModulesPath, userModulesPath;
    std::unique_ptr<AvailableModulesList> availableUserModules;
    var metadata;
    bool isTemp = false, useLocalCopy = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PIPGenerator)
};
