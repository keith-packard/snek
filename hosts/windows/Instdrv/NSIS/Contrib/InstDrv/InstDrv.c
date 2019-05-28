/*

InstDrv.dll - Installs or Removes Device Drivers

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

*/


#include <windows.h>
#include <setupapi.h>
#include <newdev.h>
#include "../exdll/exdll.h"


char    paramBuf[1024];
GUID    devClass;
char    hwIdBuf[1024];
int     initialized = 0;



void* memset(void* dst, int val, unsigned int len)
{
    while (len-- > 0)
        *((char *)dst)++ = val;

    return NULL;
}



void* memcpy(void* dst, const void* src, unsigned int len)
{
    while (len-- > 0)
        *((char *)dst)++ = *((char *)src)++;

    return NULL;
}



int HexCharToInt(char c)
{
    if ((c >= '0') && (c <= '9'))
        return c - '0';
    else if ((c >= 'a') && (c <= 'f'))
        return c - 'a' + 10;
    else if ((c >= 'A') && (c <= 'F'))
        return c - 'A' + 10;
    else
        return -1;
}



BOOLEAN HexStringToUInt(char* str, int width, void* valBuf)
{
    int i, val;


    for (i = width - 4; i >= 0; i -= 4)
    {
        val = HexCharToInt(*str++);
        if (val < 0)
            return FALSE;
        *(unsigned int *)valBuf += val << i;
    }

    return TRUE;
}



BOOLEAN StringToGUID(char* guidStr, GUID* pGuid)
{
    int i;


    memset(pGuid, 0, sizeof(GUID));

    if (*guidStr++ != '{')
        return FALSE;

    if (!HexStringToUInt(guidStr, 32, &pGuid->Data1))
        return FALSE;
    guidStr += 8;

    if (*guidStr++ != '-')
        return FALSE;

    if (!HexStringToUInt(guidStr, 16, &pGuid->Data2))
        return FALSE;
    guidStr += 4;

    if (*guidStr++ != '-')
        return FALSE;

    if (!HexStringToUInt(guidStr, 16, &pGuid->Data3))
        return FALSE;
    guidStr += 4;

    if (*guidStr++ != '-')
        return FALSE;

    for (i = 0; i < 2; i++)
    {
        if (!HexStringToUInt(guidStr, 8, &pGuid->Data4[i]))
            return FALSE;
        guidStr += 2;
    }

    if (*guidStr++ != '-')
        return FALSE;

    for (i = 2; i < 8; i++)
    {
        if (!HexStringToUInt(guidStr, 8, &pGuid->Data4[i]))
            return FALSE;
        guidStr += 2;
    }

    if (*guidStr++ != '}')
        return FALSE;

    return TRUE;
}



DWORD FindNextDevice(HDEVINFO devInfoSet, SP_DEVINFO_DATA* pDevInfoData, DWORD* pIndex)
{
    DWORD   buffersize = 0;
    LPTSTR  buffer     = NULL;
    DWORD   dataType;
    DWORD   result;


    while (1)
    {
        if (!SetupDiEnumDeviceInfo(devInfoSet, (*pIndex)++, pDevInfoData))
        {
            result = GetLastError();
            break;
        }

      GetDeviceRegistryProperty:
        if (!SetupDiGetDeviceRegistryProperty(devInfoSet, pDevInfoData, SPDRP_HARDWAREID,
                                              &dataType, (PBYTE)buffer, buffersize,
                                              &buffersize))
        {
            result = GetLastError();

            if (result == ERROR_INSUFFICIENT_BUFFER)
            {
                if (buffer != NULL)
                    LocalFree(buffer);

                buffer = (LPTSTR)LocalAlloc(LPTR, buffersize);

                if (buffer == NULL)
                    break;

                goto GetDeviceRegistryProperty;
            }
            else if (result == ERROR_INVALID_DATA)
                continue;   // ignore invalid entries
            else
                break;      // break on other errors
        }

        if (lstrcmpi(buffer, hwIdBuf) == 0)
        {
            result  = 0;
            break;
        }
    }

    if (buffer != NULL)
        LocalFree(buffer);

    return result;
}



DWORD FindFirstDevice(HWND hwndParent, const GUID* pDevClass, const LPTSTR hwId,
                      HDEVINFO* pDevInfoSet, SP_DEVINFO_DATA* pDevInfoData,
                      DWORD *pIndex, DWORD flags)
{
    DWORD   result;


    *pDevInfoSet = SetupDiGetClassDevs((GUID*)pDevClass, NULL, hwndParent, flags);
    if (*pDevInfoSet == INVALID_HANDLE_VALUE)
        return GetLastError();

    pDevInfoData->cbSize = sizeof(SP_DEVINFO_DATA);
    *pIndex = 0;

    result = FindNextDevice(*pDevInfoSet, pDevInfoData, pIndex);

    if (result != 0)
        SetupDiDestroyDeviceInfoList(*pDevInfoSet);

    return result;
}



/*
 * InstDrv::InitDriverSetup devClass drvHWID
 *
 *  devClass    - GUID of the driver's device setup class
 *  drvHWID     - Hardware ID of the supported device
 *
 * Return:
 *  result      - error message, empty on success
 */
void __declspec(dllexport) InitDriverSetup(HWND hwndParent, int string_size, char *variables, stack_t **stacktop)
{
    EXDLL_INIT();

    /* convert class GUID */
    popstring(paramBuf);

    if (!StringToGUID(paramBuf, &devClass))
    {
        popstring(paramBuf);
        pushstring("Invalid GUID!");
        return;
    }

    /* get hardware ID */
    memset(hwIdBuf, 0, sizeof(hwIdBuf));
    popstring(hwIdBuf);

    initialized = 1;
    pushstring("");
}



/*
 * InstDrv::CountDevices
 *
 * Return:
 *  result      - Number of installed devices the driver supports
 */
void __declspec(dllexport) CountDevices(HWND hwndParent, int string_size, char *variables, stack_t **stacktop)
{
    HDEVINFO            devInfoSet;
    SP_DEVINFO_DATA     devInfoData;
    int                 count = 0;
    char                countBuf[16];
    DWORD               index;
    DWORD               result;


    EXDLL_INIT();

    if (!initialized)
    {
        pushstring("Fatal error!");
        return;
    }

    result = FindFirstDevice(hwndParent, &devClass, hwIdBuf, &devInfoSet, &devInfoData,
                             &index, DIGCF_PRESENT);
    if (result != 0)
    {
        pushstring("0");
        return;
    }

    do
    {
        count++;
    } while (FindNextDevice(devInfoSet, &devInfoData, &index) == 0);

    SetupDiDestroyDeviceInfoList(devInfoSet);

    wsprintf(countBuf, "%d", count);
    pushstring(countBuf);
}



/*
 * InstDrv::CreateDevice
 *
 * Return:
 *  result      - Windows error code
 */
void __declspec(dllexport) CreateDevice(HWND hwndParent, int string_size, char *variables, stack_t **stacktop)
{
    HDEVINFO            devInfoSet;
    SP_DEVINFO_DATA     devInfoData;
    DWORD               result = 0;
    char                resultBuf[16];


    EXDLL_INIT();

    if (!initialized)
    {
        pushstring("Fatal error!");
        return;
    }

    devInfoSet = SetupDiCreateDeviceInfoList(&devClass, hwndParent);
    if (devInfoSet == INVALID_HANDLE_VALUE)
    {
        wsprintf(resultBuf, "%08X", GetLastError());
        pushstring(resultBuf);
        return;
    }

    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    if (!SetupDiCreateDeviceInfo(devInfoSet, hwIdBuf, &devClass, NULL,
                                 hwndParent, DICD_GENERATE_ID, &devInfoData))
    {
        result = GetLastError();
        goto InstallCleanup;
    }

    if (!SetupDiSetDeviceRegistryProperty(devInfoSet, &devInfoData, SPDRP_HARDWAREID,
                                          hwIdBuf, (lstrlen(hwIdBuf)+2)*sizeof(TCHAR))) 
    {
        result = GetLastError();
        goto InstallCleanup;
    }

    if (!SetupDiCallClassInstaller(DIF_REGISTERDEVICE, devInfoSet, &devInfoData))
        result = GetLastError();

  InstallCleanup:
    SetupDiDestroyDeviceInfoList(devInfoSet);

    wsprintf(resultBuf, "%08X", result);
    pushstring(resultBuf);
}



/*
 * InstDrv::InstallDriver infPath
 *
 * Return:
 *  result      - Windows error code
 *  reboot      - non-zero if reboot is required
 */
void __declspec(dllexport) InstallDriver(HWND hwndParent, int string_size, char *variables, stack_t **stacktop)
{
    char    resultBuf[16];
    BOOL    reboot;


    EXDLL_INIT();
    popstring(paramBuf);

    if (!initialized)
    {
        pushstring("Fatal error!");
        return;
    }

    if (!UpdateDriverForPlugAndPlayDevices(hwndParent, hwIdBuf, paramBuf,
                                           INSTALLFLAG_FORCE, &reboot))
    {
        wsprintf(resultBuf, "%08X", GetLastError());
        pushstring(resultBuf);
    }
    else
    {
        wsprintf(resultBuf, "%d", reboot);
        pushstring(resultBuf);
        pushstring("00000000");
    }
}



/*
 * InstDrv::DeleteOemInfFiles
 *
 * Return:
 *  result      - Windows error code
 *  oeminf      - Path of the deleted devices setup file (oemXX.inf)
 *  oempnf      - Path of the deleted devices setup file (oemXX.pnf)
 */
void __declspec(dllexport) DeleteOemInfFiles(HWND hwndParent, int string_size, char *variables, stack_t **stacktop)
{
    HDEVINFO                devInfo;
    SP_DEVINFO_DATA         devInfoData;
    SP_DRVINFO_DATA         drvInfoData;
    SP_DRVINFO_DETAIL_DATA  drvInfoDetail;
    DWORD                   index;
    DWORD                   result;
    char                    resultBuf[16];


    if (!initialized)
    {
        pushstring("Fatal error!");
        return;
    }

    result = FindFirstDevice(NULL, &devClass, hwIdBuf, &devInfo, &devInfoData, &index, 0);
    if (result != 0)
        goto Cleanup1;

    if (!SetupDiBuildDriverInfoList(devInfo, &devInfoData, SPDIT_COMPATDRIVER))
    {
        result = GetLastError();
        goto Cleanup2;
    }

    drvInfoData.cbSize = sizeof(SP_DRVINFO_DATA);
    drvInfoDetail.cbSize = sizeof(SP_DRVINFO_DETAIL_DATA);

    if (!SetupDiEnumDriverInfo(devInfo, &devInfoData, SPDIT_COMPATDRIVER, 0, &drvInfoData))
    {
        result = GetLastError();
        goto Cleanup3;
    }

    if (!SetupDiGetDriverInfoDetail(devInfo, &devInfoData, &drvInfoData,
                                    &drvInfoDetail, sizeof(drvInfoDetail), NULL))
    {
        result = GetLastError();

        if (result != ERROR_INSUFFICIENT_BUFFER)
            goto Cleanup3;

        result = 0;
    }

    pushstring(drvInfoDetail.InfFileName);
    if (!DeleteFile(drvInfoDetail.InfFileName))
        result = GetLastError();
    else
    {
        index = lstrlen(drvInfoDetail.InfFileName);
        if (index > 3)
        {
            lstrcpy(drvInfoDetail.InfFileName+index-3, "pnf");
            pushstring(drvInfoDetail.InfFileName);
            if (!DeleteFile(drvInfoDetail.InfFileName))
                result = GetLastError();
        }
    }

  Cleanup3:
    SetupDiDestroyDriverInfoList(devInfo, &devInfoData, SPDIT_COMPATDRIVER);

  Cleanup2:
    SetupDiDestroyDeviceInfoList(devInfo);

  Cleanup1:
    wsprintf(resultBuf, "%08X", result);
    pushstring(resultBuf);
}



/*
 * InstDrv::RemoveAllDevices
 *
 * Return:
 *  result      - Windows error code
 *  reboot      - non-zero if reboot is required
 */
void __declspec(dllexport) RemoveAllDevices(HWND hwndParent, int string_size, char *variables, stack_t **stacktop)
{
    HDEVINFO                devInfo;
    SP_DEVINFO_DATA         devInfoData;
    DWORD                   index;
    DWORD                   result;
    char                    resultBuf[16];
    BOOL                    reboot = FALSE;
    SP_DEVINSTALL_PARAMS    instParams;


    EXDLL_INIT();

    if (!initialized)
    {
        pushstring("Fatal error!");
        return;
    }

    result = FindFirstDevice(NULL, &devClass, hwIdBuf, &devInfo, &devInfoData, &index, 0);
    if (result != 0)
        goto Cleanup1;

    do
    {
        if (!SetupDiCallClassInstaller(DIF_REMOVE, devInfo, &devInfoData))
        {
            result = GetLastError();
            break;
        }

        instParams.cbSize = sizeof(instParams);
        if (!reboot &&
            SetupDiGetDeviceInstallParams(devInfo, &devInfoData, &instParams) &&
            ((instParams.Flags & (DI_NEEDRESTART|DI_NEEDREBOOT)) != 0))
        {
            reboot = TRUE;
        }

        result = FindNextDevice(devInfo, &devInfoData, &index);
    } while (result == 0);

    SetupDiDestroyDeviceInfoList(devInfo);

  Cleanup1:
    if ((result == 0) || (result == ERROR_NO_MORE_ITEMS))
    {
        wsprintf(resultBuf, "%d", reboot);
        pushstring(resultBuf);
        pushstring("00000000");
    }
    else
    {
        wsprintf(resultBuf, "%08X", result);
        pushstring(resultBuf);
    }
}



/*
 * InstDrv::StartSystemService serviceName
 *
 * Return:
 *  result      - Windows error code
 */
void __declspec(dllexport) StartSystemService(HWND hwndParent, int string_size, char *variables, stack_t **stacktop)
{
    SC_HANDLE       managerHndl;
    SC_HANDLE       svcHndl;
    SERVICE_STATUS  svcStatus;
    DWORD           oldCheckPoint;
    DWORD           result;
    char            resultBuf[16];


    EXDLL_INIT();
    popstring(paramBuf);

    managerHndl = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (managerHndl == NULL)
    {
        result = GetLastError();
        goto Cleanup1;
    }

    svcHndl = OpenService(managerHndl, paramBuf, SERVICE_START | SERVICE_QUERY_STATUS);
    if (svcHndl == NULL)
    {
        result = GetLastError();
        goto Cleanup2;
    }

    if (!StartService(svcHndl, 0, NULL) || !QueryServiceStatus(svcHndl, &svcStatus))
    {
        result = GetLastError();
        goto Cleanup3;
    }

    while (svcStatus.dwCurrentState == SERVICE_START_PENDING)
    {
        oldCheckPoint = svcStatus.dwCheckPoint;

        Sleep(svcStatus.dwWaitHint);

        if (!QueryServiceStatus(svcHndl, &svcStatus))
        {
            result = GetLastError();
            break;
        }

        if (oldCheckPoint >= svcStatus.dwCheckPoint)
        {
            if ((svcStatus.dwCurrentState == SERVICE_STOPPED) &&
                (svcStatus.dwWin32ExitCode != 0))
                result = svcStatus.dwWin32ExitCode;
            else
                result = ERROR_SERVICE_REQUEST_TIMEOUT;
        }
    }

    if (svcStatus.dwCurrentState == SERVICE_RUNNING)
        result = 0;

  Cleanup3:
    CloseServiceHandle(svcHndl);

  Cleanup2:
    CloseServiceHandle(managerHndl);

  Cleanup1:
    wsprintf(resultBuf, "%08X", result);
    pushstring(resultBuf);
}



/*
 * InstDrv::StopSystemService serviceName
 *
 * Return:
 *  result      - Windows error code
 */
void __declspec(dllexport) StopSystemService(HWND hwndParent, int string_size, char *variables, stack_t **stacktop)
{
    SC_HANDLE       managerHndl;
    SC_HANDLE       svcHndl;
    SERVICE_STATUS  svcStatus;
    DWORD           oldCheckPoint;
    DWORD           result;
    char            resultBuf[16];


    EXDLL_INIT();
    popstring(paramBuf);

    managerHndl = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (managerHndl == NULL)
    {
        result = GetLastError();
        goto Cleanup1;
    }

    svcHndl = OpenService(managerHndl, paramBuf, SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (svcHndl == NULL)
    {
        result = GetLastError();
        goto Cleanup2;
    }

    if (!ControlService(svcHndl, SERVICE_CONTROL_STOP, &svcStatus))
    {
        result = GetLastError();
        goto Cleanup3;
    }

    while (svcStatus.dwCurrentState == SERVICE_STOP_PENDING)
    {
        oldCheckPoint = svcStatus.dwCheckPoint;

        Sleep(svcStatus.dwWaitHint);

        if (!QueryServiceStatus(svcHndl, &svcStatus))
        {
            result = GetLastError();
            break;
        }

        if (oldCheckPoint >= svcStatus.dwCheckPoint)
        {
            result = ERROR_SERVICE_REQUEST_TIMEOUT;
            break;
        }
    }

    if (svcStatus.dwCurrentState == SERVICE_STOPPED)
        result = 0;

  Cleanup3:
    CloseServiceHandle(svcHndl);

  Cleanup2:
    CloseServiceHandle(managerHndl);

  Cleanup1:
    wsprintf(resultBuf, "%08X", result);
    pushstring(resultBuf);
}



BOOL WINAPI _DllMainCRTStartup(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}
