function Component() {
  installer.installationFinished.connect(this, Component.prototype.installationFinishedPageIsShown);
  installer.finishButtonClicked.connect(this, Component.prototype.installationFinished);
}

Component.prototype.getPathToApp = function() {

    var exePath;
    var appName = installer.value("Title");

    switch(systemInfo.kernelType) {
        case "winnt":
            exePath = installer.value("TargetDir") + "/" + appName + ".exe";
            break;
        case "darwin":
            exePath = installer.value("TargetDir") + "/"+ appName + ".app";
            break;
    }

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

          //LaunchBox
          var isLaunchBoxChecked = component.userInterface( "EndInstallerForm" ).LaunchBox.checked;
          if (isLaunchBoxChecked) {
            switch(systemInfo.kernelType) {
                case "winnt":
                    QDesktopServices.openUrl("file:///" + Component.prototype.getPathToApp());
                    break;
                case "darwin":
                    var argsList = ["-a", Component.prototype.getPathToApp()];
                    installer.execute("open", argsList);
                    break;
            }
          }

        }
    } catch(e) {
        console.log(e);
    }
}