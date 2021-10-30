function Component() {
    installer.installationFinished.connect(this, Component.prototype.installationFinishedPageIsShown);
    installer.finishButtonClicked.connect(this, Component.prototype.installationFinished);
}

Component.prototype.getPathToApp = function() {
    var appName = installer.value("Title");
    var exePath = installer.value("TargetDir") + "/bin/" + appName + ".exe";
    return exePath;
}

Component.prototype.createOperations = function() {
    component.createOperations();

    //create a shortcut on windows
    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", Component.prototype.getPathToApp(), "@DesktopDir@/@Title@.lnk");
    }
}

Component.prototype.installationFinishedPageIsShown = function() {
    try {
        if (installer.isInstaller() && installer.status == QInstaller.Success) {
            installer.addWizardPageItem( component, "EndInstallerForm", QInstaller.InstallationFinished );
        }
    } catch(e) {
        console.log(e);
    }
}

Component.prototype.installationFinished = function() {
    try {
        if (installer.isInstaller() && installer.status == QInstaller.Success) {
            var isLaunchBoxChecked = component.userInterface("EndInstallerForm").LaunchBox.checked;
            if (isLaunchBoxChecked) {
                QDesktopServices.openUrl("file:///" + Component.prototype.getPathToApp());
            }
        }
    } catch(e) {
        console.log(e);
    }
}