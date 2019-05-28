InstDrv.dll version 0.2 - Installs or Removes Device Drivers
------------------------------------------------------------


The plugin helps you to create NSIS scripts for installing device drivers or
removing them again. It can count installed device instances, create new ones
or delete all supported device. InstDrv works on Windows 2000 or later.



InstDrv::InitDriverSetup devClass drvHWID
Return: result

To start processing a driver, first call this function. devClass is the GUID
of the device class the driver supports, drvHWID is the device hardware ID. If
you don't know what these terms mean, you may want to take a look at the
Windows DDK. This function returns an empty string on success, otherwise an
error message.

InitDriverSetup has to be called every time after the plugin dll has been
(re-)loaded, or if you want to switch to a different driver.



InstDrv::CountDevices
Return: number

This call returns the number of installed and supported devices of the driver.



InstDrv::CreateDevice
Return: result

To create a new deviced node which the driver has to support, use this
function. You may even call it multiple times for more than one instance. The
return value is the Windows error code (in hex). Use CreateDevice before
installing or updating the driver itself.



InstDrv::InstallDriver infPath
Return: result
        reboot

InstallDriver installs or updates a device driver as specified in the .inf
setup script. It returns a Windows error code (in hex) and, on success, a flag
signalling if a system reboot is required.



InstDrv::DeleteOemInfFiles
Return: result
        oeminf
        oempnf

DeleteOemInfFiles tries to clean up the Windows inf directory by deleting the
oemXX.inf and oemXX.pnf files associated with the drivers. It returns a
Windows error code (in hex) and, on success, the names of the deleted files.
This functions requires that at least one device instance is still present.
So, call it before you remove the devices itself. You should also call it
before updating a driver. This avoids that the inf directory gets slowly
messed up with useless old setup scripts (which does NOT really accelerate
Windows). The error code which comes up when no device is installed is
"00000103".



InstDrv::RemoveAllDevices
Return: result
        reboot

This functions deletes all devices instances the driver supported. It returns
a Windows error code (in hex) and, on success, a flag signalling if the system
needs to be rebooted. You additionally have to remove the driver binaries from
the system paths.



InstDrv::StartSystemService serviceName
Return: result

Call this function to start the provided system service. The function blocks
until the service is started or the system reported a timeout. The return value
is the Windows error code (in hex).



InstDrv::StopSystemService serviceName
Return: result

This function tries to stop the provided system service. It blocks until the
service has been shut down or the system reported a timeout. The return value
is the Windows error code (in hex).



Example.nsi

The example script installs or removes the virtual COM port driver of IrCOMM2k
(2.0.0-alpha8, see www.ircomm2k.de/english). The driver and its setup script
are only included for demonstration purposes, they do not work without the
rest of IrCOMM2k (but they also do not cause any harm).



Building the Source Code

To build the plugin from the source code, some include files and libraries
which come with the Windows DDK are required.



History

 0.2    - fixed bug when calling InitDriverSetup the second time
        - added StartSystemService and StopSystemService

 0.1    - first release



License

Copyright © 2003 Jan Kiszka (Jan.Kiszka@web.de)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute
it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; 
   you must not claim that you wrote the original software.
   If you use this software in a product, an acknowledgment in the
   product documentation would be appreciated but is not required.
2. Altered versions must be plainly marked as such,
   and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any distribution.
