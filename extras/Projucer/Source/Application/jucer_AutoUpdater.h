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

#include "../Utility/Helpers/jucer_VersionInfo.h"

class DownloadAndInstallThread;

class LatestVersionCheckerAndUpdater final : public DeletedAtShutdown,
                                             private Thread
{
public:
    LatestVersionCheckerAndUpdater();
    ~LatestVersionCheckerAndUpdater() override;

    void checkForNewVersion (bool isBackgroundCheck);

    //==============================================================================
    JUCE_DECLARE_SINGLETON_SINGLETHREADED_MINIMAL (LatestVersionCheckerAndUpdater)

private:
    //==============================================================================
    void run() override;
    void askUserAboutNewVersion (const String&, const String&, const VersionInfo::Asset&);
    void askUserForLocationToDownload (const VersionInfo::Asset&);
    void downloadAndInstall (const VersionInfo::Asset&, const File&);

    void showDialogWindow (const String&, const String&, const VersionInfo::Asset&);
    void addNotificationToOpenProjects (const VersionInfo::Asset&);

    //==============================================================================
    bool backgroundCheck = false;

    std::unique_ptr<DownloadAndInstallThread> installer;
    std::unique_ptr<Component> dialogWindow;
    std::unique_ptr<FileChooser> chooser;
    ScopedMessageBox messageBox;

    JUCE_DECLARE_WEAK_REFERENCEABLE (LatestVersionCheckerAndUpdater)
};
