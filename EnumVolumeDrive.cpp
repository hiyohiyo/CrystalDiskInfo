
#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <atlbase.h>
#include <atlstr.h>
#include <atlcoll.h>
#include <cfgmgr32.h>
#include <setupapi.h>
#pragma comment(lib, "setupapi.lib")


/////////////////////////////////////////////////////////////////////////////

struct DiskDriveInfo
{
    CString		DriveLetter;	// ドライブレター
	CString		DevicePath;		// デバイスパス
	CString		VolumeName;		// ボリューム名
	CString		ParentDevId;	// 親階層デバイスのデバイスID
};


/////////////////////////////////////////////////////////////////////////////

// ディスクドライブ情報
CAtlArray< DiskDriveInfo >	gDriveInfos;


/////////////////////////////////////////////////////////////////////////////

BOOL GetVolumeNameFromDriveLetter()
{
	TCHAR szDriveLetter[] = _T("A:\\");
	TCHAR szVolumeName[MAX_PATH];
	BOOL bRet;

	for ( int i = 0; i < 25; i++ )
	{
		szDriveLetter[0] = _T('A') + i;

		// ドライブレターからボリューム名を取得
		bRet = GetVolumeNameForVolumeMountPoint(
						szDriveLetter,
						szVolumeName,
						sizeof( szVolumeName ) / sizeof( TCHAR ) );

		// 取得成功
		if ( bRet )
		{
			DiskDriveInfo info;

			info.DriveLetter = szDriveLetter;
			info.VolumeName  = szVolumeName;

			gDriveInfos.Add( info );
		}
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////

BOOL GetVolumeNameFromDiskDevicePath()
{
	IID									iidVolumeClass;
	HDEVINFO							hDevInfo;
	SP_DEVICE_INTERFACE_DATA			sDevIfData;
	SP_DEVINFO_DATA						sDevInfoData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA	pDevIfDetail;
	ULONG								ulLength;
	TCHAR								szDevicePath[MAX_PATH];
	TCHAR								szVolumeName[MAX_PATH];
	BOOL								bRet;

	// Volume Device Class
	IIDFromString( L"{53f5630d-b6bf-11d0-94f2-00a0c91efb8b}", &iidVolumeClass);

	hDevInfo = SetupDiGetClassDevs(
						&iidVolumeClass,
						NULL,
						NULL,
						DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );

	if ( hDevInfo == INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}

	sDevIfData.cbSize = sizeof( SP_DEVICE_INTERFACE_DATA );

	// ディスクデバイスを列挙し、デバイスパスからボリューム名を取得する。
	for (ULONG nIndex = 0; ; nIndex++)
	{
		bRet = SetupDiEnumDeviceInterfaces(
							hDevInfo,
							0,
							&iidVolumeClass,
							nIndex,
							&sDevIfData );

		if ( !bRet )
		{
			break;	// 列挙終了
		}

		ZeroMemory( &sDevInfoData, sizeof( SP_DEVINFO_DATA ) );

		sDevInfoData.cbSize = sizeof( SP_DEVINFO_DATA );

		// デバイス詳細情報の長さを取得
		SetupDiGetDeviceInterfaceDetail(
							hDevInfo,
							&sDevIfData,
							NULL,
							0,
							&ulLength,
							&sDevInfoData );

		// デバイス詳細情報用メモリ確保
		pDevIfDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc( ulLength );

		assert( pDevIfDetail != NULL );

		pDevIfDetail->cbSize = sizeof( SP_DEVICE_INTERFACE_DETAIL_DATA );

		// デバイス詳細情報取得
		bRet = SetupDiGetDeviceInterfaceDetail(
							hDevInfo,
							&sDevIfData,
							pDevIfDetail,
							ulLength,
							&ulLength,
							&sDevInfoData );

		// デバイス詳細情報取得成功
		if ( bRet )
		{
			// デバイスパス（末尾に'\'を付ける）
			_tcscpy( szDevicePath, pDevIfDetail->DevicePath );
			_tcscat( szDevicePath, _T("\\") );

			// デバイスパスからボリューム名を取得
			bRet = GetVolumeNameForVolumeMountPoint(
								szDevicePath,
								szVolumeName,
								sizeof( szVolumeName ) / sizeof( TCHAR ) );

			// ボリューム名取得成功
			if ( bRet )
			{
				// ★ドライブレターから取得したボリューム名と比較し
				//   ドライブレターと関連付ける。
				for ( size_t i = 0; i < gDriveInfos.GetCount(); i++ )
				{
					DiskDriveInfo& rInfo = gDriveInfos[i];

					// ボリューム名が一致
					if ( rInfo.VolumeName.CompareNoCase( szVolumeName ) == 0 )
					{
						rInfo.DevicePath = szDevicePath;

						// ★ボリュームデバイスの親階層(ディスクデバイス?)の
						// 　デバイスIDを取得する。
						{
							DEVINST hDevInst = sDevInfoData.DevInst;
							DEVINST hParentDev;
							ULONG ulParentDevIdLen;
							LPTSTR pParentDevId;
							CONFIGRET cRet;

							cRet = CM_Get_Parent( &hParentDev, hDevInst, 0 );

							if ( cRet == CR_SUCCESS )
							{
								CM_Get_Device_ID_Size( &ulParentDevIdLen, hParentDev, 0 );
								++ulParentDevIdLen; // ヌル文字分

								pParentDevId = (LPTSTR)malloc( ulParentDevIdLen * sizeof( TCHAR ) );

								assert( pParentDevId != NULL );

								ZeroMemory( pParentDevId, ulParentDevIdLen * sizeof( TCHAR ) );

								cRet = CM_Get_Device_ID( hParentDev, pParentDevId, ulParentDevIdLen, 0 );

								if ( cRet == CR_SUCCESS )
								{
									rInfo.ParentDevId = pParentDevId;
								}

								free( pParentDevId );
							}
						}
					}
				}
			}
		}

		free( pDevIfDetail );
	}

	SetupDiDestroyDeviceInfoList( hDevInfo );

	return TRUE;
}

void ShowData()
{
	CString cstr, clip;

	for ( size_t i = 0; i < gDriveInfos.GetCount(); i++ )
	{
		DiskDriveInfo& rInfo = gDriveInfos[i];

		cstr.Format(_T("Drive Letter : %s\nDevice Path  : %s\nVolume Name  : %s\nParent DevId : %s\n\n"), (LPCTSTR)rInfo.DriveLetter, (LPCTSTR)rInfo.DevicePath, (LPCTSTR)rInfo.VolumeName, (LPCTSTR)rInfo.ParentDevId);
		clip += cstr;
	}
	CStdioFile file;
	file.Open(_T("test.txt"), CFile::modeCreate | CFile::modeWrite | CFile::typeText );
	file.WriteString(clip);
	file.Close();
}
