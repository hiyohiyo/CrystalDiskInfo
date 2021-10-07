/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskInfo.h"
#include "DiskInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CDiskInfoDlg::Refresh(DWORD flagForceUpdate)
{
	CWaitCursor wait;
	BOOL flagUpdate = FALSE;
	DWORD smartUpdate[CAtaSmart::MAX_DISK] = { 0 };

	for (int i = 0; i < m_Ata.vars.GetCount(); i++)
	{
		if (flagForceUpdate || m_bAutoRefreshTarget[i])
		{
			switch (m_Ata.UpdateSmartInfo(i))
			{
			case CAtaSmart::SMART_STATUS_MAJOR_CHANGE:
				flagUpdate = TRUE;
				smartUpdate[i] = CAtaSmart::SMART_STATUS_MAJOR_CHANGE;
				SaveSmartInfo(i);
				break;
			case CAtaSmart::SMART_STATUS_MINOR_CHANGE:
				smartUpdate[i] = CAtaSmart::SMART_STATUS_MINOR_CHANGE;
				SaveSmartInfo(i);
				break;
			default:
				break;
			}
		}
	}
	AlertSound(0, AS_PLAY_SOUND);
	if (flagForceUpdate || smartUpdate[m_SelectDisk] == CAtaSmart::SMART_STATUS_MAJOR_CHANGE)
	{
		ChangeDisk(m_SelectDisk);
		UpdateListCtrl(m_SelectDisk);
	}
	else if (smartUpdate[m_SelectDisk] == CAtaSmart::SMART_STATUS_MINOR_CHANGE)
	{
		UpdateListCtrl(m_SelectDisk);
	}

	if (flagForceUpdate || flagUpdate)
	{
		InitDriveList();
		UpdateToolTip();

		if (m_bResident)
		{
			AlarmOverheat();
			UpdateTrayTemperatureIcon(TRUE);
		}
		UpdateShareInfo();
	}
}

void CDiskInfoDlg::UpdateShareInfo()
{
	if (!m_bGadget || m_Ata.vars.GetCount() == 0)
	{
		return;
	}

	HKEY hKey, hSubKey;
	CString key, cstr;
	DWORD result;
	DWORD value;
	TCHAR str[256];

	SHDeleteKey(HKEY_CURRENT_USER, REGISTRY_PATH);
	result = RegCreateKeyEx(HKEY_CURRENT_USER, REGISTRY_PATH, 0, _T(""),
		REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);

	if (result != ERROR_SUCCESS)
	{
		return;
	}

	value = 1;
	RegSetValueEx(hKey, _T("Version"), 0, REG_DWORD, (CONST BYTE*)&value, sizeof(DWORD));
	value = GetTickCount();
	RegSetValueEx(hKey, _T("LastUpdate"), 0, REG_DWORD, (CONST BYTE*)&value, sizeof(DWORD));
	value = (DWORD)m_Ata.vars.GetCount();
	RegSetValueEx(hKey, _T("DiskCount"), 0, REG_DWORD, (CONST BYTE*)&value, sizeof(DWORD));

	for (int i = 0; i < m_Ata.vars.GetCount(); i++)
	{
		result = RegCreateKeyEx(hKey, m_Ata.vars[i].ModelSerial, 0, NULL,
			REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &hSubKey, NULL);
		if (result != ERROR_SUCCESS)
		{
			continue;
		}

		key.Format(_T("Disk%d"), i);
		wsprintf(str, m_Ata.vars[i].ModelSerial);
		RegSetValueEx(hKey, key, 0, REG_SZ,
			(CONST BYTE*)&str, (DWORD)(_tcslen(str) + 1) * sizeof(TCHAR));


		wsprintf(str, m_Ata.vars[i].DriveMap);
		RegSetValueEx(hSubKey, _T("DriveLetter"), 0, REG_SZ,
			(CONST BYTE*)&str, (DWORD)(_tcslen(str) + 1) * sizeof(TCHAR));
		wsprintf(str, m_Ata.vars[i].Model);
		RegSetValueEx(hSubKey, _T("Model"), 0, REG_SZ,
			(CONST BYTE*)&str, (DWORD)(_tcslen(str) + 1) * sizeof(TCHAR));

		if (m_Ata.vars[i].TotalDiskSize >= 1000)
		{
			_stprintf_s(str, 256, _T("%.1f GB"), m_Ata.vars[i].TotalDiskSize / 1000.0);
		}
		else
		{
			_stprintf_s(str, 256, _T("%d MB"), m_Ata.vars[i].TotalDiskSize);
		}
		RegSetValueEx(hSubKey, _T("DiskSize"), 0, REG_SZ,
			(CONST BYTE*)&str, (DWORD)(_tcslen(str) + 1) * sizeof(TCHAR));

		if (m_bFahrenheit)
		{
			if (m_Ata.vars[i].Temperature > -300)
			{
				DWORD f = m_Ata.vars[i].Temperature * 9 / 5 + 32;
				if (f > 100)
				{
					_stprintf_s(str, 256, _T("%d °F"), f);
				}
				else
				{
					_stprintf_s(str, 256, _T("%d  °F"), f);
				}
			}
			else
			{
				_stprintf_s(str, 256, _T("-- °F"));
			}
		}
		else
		{
			if (m_Ata.vars[i].Temperature > -300)
			{
				_stprintf_s(str, 256, _T("%d °C"), m_Ata.vars[i].Temperature);
			}
			else
			{
				_stprintf_s(str, 256, _T("-- °C"));
			}
		}
		RegSetValueEx(hSubKey, _T("Temperature"), 0, REG_SZ,
			(CONST BYTE*)&str, (DWORD)(_tcslen(str) + 1) * sizeof(TCHAR));

		cstr = GetTemperatureClass(m_Ata.vars[i].Temperature, m_Ata.vars[i].AlarmTemperature);
		cstr.Replace(L"Green", L"");
		_stprintf_s(str, 256, _T("%s"), (LPCTSTR)cstr);
		RegSetValueEx(hSubKey, _T("TemperatureClass"), 0, REG_SZ,
			(CONST BYTE*)&str, (DWORD)(_tcslen(str) + 1) * sizeof(TCHAR));

		value = m_Ata.vars[i].DiskStatus;
		RegSetValueEx(hSubKey, _T("DiskStatus"), 0, REG_DWORD, (CONST BYTE*)&value, sizeof(DWORD));

		RegCloseKey(hSubKey);
	}
	RegCloseKey(hKey);
}

void CDiskInfoDlg::DeleteShareInfo()
{
	SHDeleteKey(HKEY_CURRENT_USER, REGISTRY_PATH);
}

typedef int(WINAPI* FuncGetSystemMetricsForDpi) (int nIndex, UINT dpi);
typedef UINT(WINAPI* FuncGetDpiForWindow) (HWND hWnd);

void CDiskInfoDlg::RebuildListHeader(DWORD i, BOOL forceUpdate)
{
	static DWORD preVendorId = -1;
	int width = 0;

	static FuncGetSystemMetricsForDpi pGetSystemMetricsForDpi = (FuncGetSystemMetricsForDpi)GetProcAddress(GetModuleHandle(_T("User32.dll")), "GetSystemMetricsForDpi");
	static FuncGetDpiForWindow pGetDpiForWindow = (FuncGetDpiForWindow)GetProcAddress(GetModuleHandle(_T("User32.dll")), "GetDpiForWindow");

	if (pGetSystemMetricsForDpi != NULL)
	{
		width = (int)((656 * m_ZoomRatio - (pGetSystemMetricsForDpi(SM_CXVSCROLL, pGetDpiForWindow(m_hWnd)))) - 4);
	}
	else
	{
		width = (int)(656 * m_ZoomRatio - GetSystemMetrics(SM_CXVSCROLL) - 4);
	}

	if (m_Ata.vars.GetCount() == 0)
	{
		m_List.InsertColumn(0, _T(""), LVCFMT_CENTER, 0, 0);
		m_List.EnableHeaderOwnerDraw(TRUE);
		return;
	}

	m_List.DeleteAllItems();
	if (preVendorId == m_Ata.vars[i].DiskVendorId && !forceUpdate)
	{
		return;
	}

	while (m_List.DeleteColumn(0)) {}
	if (m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_NVME)
	{
		m_List.InsertColumn(0, _T(""), LVCFMT_CENTER, 25, 0);
		m_List.InsertColumn(1, i18n(_T("Dialog"), _T("LIST_ID"), m_bSmartEnglish), LVCFMT_CENTER, (int)(32 * m_ZoomRatio), 0);
		m_List.InsertColumn(3, i18n(_T("Dialog"), _T("LIST_CURRENT"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(0 * m_ZoomRatio), 0);
		m_List.InsertColumn(4, i18n(_T("Dialog"), _T("LIST_WORST"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(0 * m_ZoomRatio), 0);
		m_List.InsertColumn(5, i18n(_T("Dialog"), _T("LIST_THRESHOLD"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(0 * m_ZoomRatio), 0);
		m_List.InsertColumn(6, i18n(_T("Dialog"), _T("LIST_RAW_VALUES"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(140 * m_ZoomRatio), 0);
		m_List.InsertColumn(2, i18n(_T("Dialog"), _T("LIST_ATTRIBUTE_NAME"), m_bSmartEnglish), LVCFMT_LEFT, (int)(width - 172 * m_ZoomRatio - 25), 0);
		preVendorId = m_Ata.SSD_VENDOR_NVME;
	}
	else if (m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_SANDFORCE)
	{
		m_List.InsertColumn(0, _T(""), LVCFMT_CENTER, 25, 0);
		m_List.InsertColumn(1, i18n(_T("Dialog"), _T("LIST_ID"), m_bSmartEnglish), LVCFMT_CENTER, (int)(32 * m_ZoomRatio), 0);
		m_List.InsertColumn(3, i18n(_T("Dialog"), _T("LIST_CURRENT"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(72 * m_ZoomRatio), 0);
		m_List.InsertColumn(4, i18n(_T("Dialog"), _T("LIST_WORST"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(72 * m_ZoomRatio), 0);
		m_List.InsertColumn(5, i18n(_T("Dialog"), _T("LIST_THRESHOLD"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(72 * m_ZoomRatio), 0);
		m_List.InsertColumn(6, i18n(_T("Dialog"), _T("LIST_RAW_VALUES"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(136 * m_ZoomRatio), 0);
		m_List.InsertColumn(2, i18n(_T("Dialog"), _T("LIST_ATTRIBUTE_NAME"), m_bSmartEnglish), LVCFMT_LEFT, (int)(width - 384 * m_ZoomRatio - 25), 0);
		preVendorId = m_Ata.SSD_VENDOR_SANDFORCE;
	}
	// JMicron 60x
	else if (m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_JMICRON && m_Ata.vars[i].IsRawValues8)
	{
		m_List.InsertColumn(0, _T(""), LVCFMT_CENTER, 25, 0);
		m_List.InsertColumn(1, i18n(_T("Dialog"), _T("LIST_ID"), m_bSmartEnglish), LVCFMT_CENTER, (int)(32 * m_ZoomRatio), 0);
		m_List.InsertColumn(3, i18n(_T("Dialog"), _T("LIST_CURRENT"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(72 * m_ZoomRatio), 0);
		m_List.InsertColumn(4, i18n(_T("Dialog"), _T("LIST_WORST"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(0 * m_ZoomRatio), 0);
		m_List.InsertColumn(5, i18n(_T("Dialog"), _T("LIST_THRESHOLD"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(0 * m_ZoomRatio), 0);
		m_List.InsertColumn(6, i18n(_T("Dialog"), _T("LIST_RAW_VALUES"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(140 * m_ZoomRatio), 0);
		m_List.InsertColumn(2, i18n(_T("Dialog"), _T("LIST_ATTRIBUTE_NAME"), m_bSmartEnglish), LVCFMT_LEFT, (int)(width - 244 * m_ZoomRatio - 25), 0);
		preVendorId = m_Ata.SSD_VENDOR_JMICRON;
	}
	else if (m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_INDILINX)
	{
		m_List.InsertColumn(0, _T(""), LVCFMT_CENTER, 25, 0);
		m_List.InsertColumn(1, i18n(_T("Dialog"), _T("LIST_ID"), m_bSmartEnglish), LVCFMT_CENTER, (int)(32 * m_ZoomRatio), 0);
		m_List.InsertColumn(3, i18n(_T("Dialog"), _T("LIST_CURRENT"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(0 * m_ZoomRatio), 0);
		m_List.InsertColumn(4, i18n(_T("Dialog"), _T("LIST_WORST"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(0 * m_ZoomRatio), 0);
		m_List.InsertColumn(5, i18n(_T("Dialog"), _T("LIST_THRESHOLD"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(0 * m_ZoomRatio), 0);
		m_List.InsertColumn(6, i18n(_T("Dialog"), _T("LIST_RAW_VALUES"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(140 * m_ZoomRatio), 0);
		m_List.InsertColumn(2, i18n(_T("Dialog"), _T("LIST_ATTRIBUTE_NAME"), m_bSmartEnglish), LVCFMT_LEFT, (int)(width - 172 * m_ZoomRatio - 25), 0);
		preVendorId = m_Ata.SSD_VENDOR_INDILINX;
	}
	else
	{
		m_List.InsertColumn(0, _T(""), LVCFMT_CENTER, 25, 0);
		m_List.InsertColumn(1, i18n(_T("Dialog"), _T("LIST_ID"), m_bSmartEnglish), LVCFMT_CENTER, (int)(32 * m_ZoomRatio), 0);
		m_List.InsertColumn(3, i18n(_T("Dialog"), _T("LIST_CURRENT"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(72 * m_ZoomRatio), 0);
		m_List.InsertColumn(4, i18n(_T("Dialog"), _T("LIST_WORST"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(72 * m_ZoomRatio), 0);
		m_List.InsertColumn(5, i18n(_T("Dialog"), _T("LIST_THRESHOLD"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(72 * m_ZoomRatio), 0);
		m_List.InsertColumn(6, i18n(_T("Dialog"), _T("LIST_RAW_VALUES"), m_bSmartEnglish), LVCFMT_RIGHT, (int)(108 * m_ZoomRatio), 0);
		m_List.InsertColumn(2, i18n(_T("Dialog"), _T("LIST_ATTRIBUTE_NAME"), m_bSmartEnglish), LVCFMT_LEFT, (int)(width - 356 * m_ZoomRatio - 25), 0);
		preVendorId = m_Ata.HDD_GENERAL;
	}

	m_List.EnableHeaderOwnerDraw(TRUE);
}

BOOL CDiskInfoDlg::UpdateListCtrl(DWORD i)
{
	static DWORD preSelectDisk = 0;

	if (m_Ata.vars.GetCount() == 0)
	{
		m_List.DeleteAllItems();
		return FALSE;
	}

	BOOL flag = FALSE;
	if (i == preSelectDisk && m_List.GetItemCount() > 0)
	{
		flag = TRUE;
	}
	else
	{
		m_List.SetRedraw(FALSE);
		RebuildListHeader(i);
		preSelectDisk = i;
	}

	CString cstr;
	DWORD caution = 0;
	UINT mask = LVIF_IMAGE;

	DWORD k = 0;

	TCHAR str[256];
	CString unknown;
	CString vendorSpecific;

	//	unknown = i18n(_T("Smart"), _T("UNKNOWN"), m_bSmartEnglish);
	vendorSpecific = i18n(_T("Smart"), _T("VENDOR_SPECIFIC"), m_bSmartEnglish);

	for (DWORD j = 0; j < m_Ata.vars[i].AttributeCount; j++)
	{
		if (m_Ata.vars[i].Attribute[j].Id == 0x00 || m_Ata.vars[i].Attribute[j].Id == 0xFF)
		{
			continue;
		}

		if (m_Ata.vars[i].IsNVMe)
		{
			UINT icon = ICON_GOOD + m_bGreenMode;

			if (m_Ata.vars[i].Model.Find(_T("Parallels")) == 0
			||  m_Ata.vars[i].Model.Find(_T("VMware")) == 0
			||  m_Ata.vars[i].Model.Find(_T("QEMU")) == 0
				)
			{
				icon = ICON_UNKNOWN;
			}
			else if (m_Ata.vars[i].Attribute[j].Id == 0x01 && m_Ata.vars[i].Attribute[j].RawValue[0])
			{
				icon = ICON_BAD;
			}
			else if (m_Ata.vars[i].Attribute[j].Id == 0x02 && ((MAKEWORD(m_Ata.vars[i].Attribute[1].RawValue[0], m_Ata.vars[i].Attribute[1].RawValue[1]) - 273) >= m_Ata.vars[i].AlarmTemperature))
			{
				icon = ICON_BAD;
			}
			else if(m_Ata.vars[i].Attribute[j].Id == 0x03 && (m_Ata.vars[i].Attribute[2].RawValue[0] < m_Ata.vars[i].Attribute[3].RawValue[0]))
			{
				icon = ICON_BAD;
			}
			else if (m_Ata.vars[i].Attribute[j].Id == 0x03 && (m_Ata.vars[i].Attribute[2].RawValue[0] == m_Ata.vars[i].Attribute[3].RawValue[0]))
			{
				icon = ICON_CAUTION;
			}
			else if (m_Ata.vars[i].Attribute[j].Id == 0x05 && ((100 - m_Ata.vars[i].Attribute[j].RawValue[0]) <= m_Ata.vars[i].ThresholdFF))
			{
				icon = ICON_CAUTION;
			}

			if (flag)
			{
				m_List.SetItem(k, 0, mask, _T(""), icon, 0, 0, 0, 0);
			}
			else
			{
				m_List.InsertItem(k, _T(""), icon);
			}
		}
		else if (m_Ata.vars[i].IsSmartCorrect && m_Ata.vars[i].IsThresholdCorrect && !m_Ata.vars[i].IsThresholdBug)
		{
			if (!m_Ata.vars[i].IsSsd &&
				(m_Ata.vars[i].Attribute[j].Id == 0x05 // Reallocated Sectors Count
					|| m_Ata.vars[i].Attribute[j].Id == 0xC5 // Current Pending Sector Count
					|| m_Ata.vars[i].Attribute[j].Id == 0xC6 // Off-Line Scan Uncorrectable Sector Count
					))
			{
				WORD raw = MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[0], m_Ata.vars[i].Attribute[j].RawValue[1]);
				WORD threshold;
				switch (m_Ata.vars[i].Attribute[j].Id)
				{
				case 0x05:
					threshold = m_Ata.vars[i].Threshold05;
					break;
				case 0xC5:
					threshold = m_Ata.vars[i].ThresholdC5;
					break;
				case 0xC6:
					threshold = m_Ata.vars[i].ThresholdC6;
					break;
				}
				if (threshold > 0 && raw >= threshold)
				{
					caution = 1;
				}
				else
				{
					caution = 0;
				}

				if (m_Ata.vars[i].Threshold[j].ThresholdValue != 0
					&& m_Ata.vars[i].Attribute[j].CurrentValue < m_Ata.vars[i].Threshold[j].ThresholdValue)
				{
					if (flag)
					{
						m_List.SetItem(k, 0, mask, _T(""), ICON_BAD, 0, 0, 0, 0);
					}
					else
					{
						m_List.InsertItem(k, _T(""), ICON_BAD);
					}
				}
				else if (caution > 0)
				{
					if (flag)
					{
						m_List.SetItem(k, 0, mask, _T(""), ICON_CAUTION, 0, 0, 0, 0);
					}
					else
					{
						m_List.InsertItem(k, _T(""), ICON_CAUTION);
					}
				}
				else
				{
					if (flag)
					{
						m_List.SetItem(k, 0, mask, _T(""), ICON_GOOD + m_bGreenMode, 0, 0, 0, 0);
					}
					else
					{
						m_List.InsertItem(k, _T(""), ICON_GOOD + m_bGreenMode);
					}
				}
			}
			// Temperature
			else if (m_Ata.vars[i].Attribute[j].Id == 0xC2)
			{
				if (flag)
				{
					m_List.SetItem(k, 0, mask, _T(""), ICON_GOOD + m_bGreenMode, 0, 0, 0, 0);
				}
				else
				{
					m_List.InsertItem(k, _T(""), ICON_GOOD + m_bGreenMode);
				}
			}
			// End-to-End Error
			// https://crystalmark.info/bbs/c-board.cgi?cmd=one;no=1090;id=diskinfo#1090
			// http://h20000.www2.hp.com/bc/docs/support/SupportManual/c01159621/c01159621.pdf
			else if (m_Ata.vars[i].Attribute[j].Id == 0xB8)
			{
				if (flag)
				{
					m_List.SetItem(k, 0, mask, _T(""), ICON_GOOD + m_bGreenMode, 0, 0, 0, 0);
				}
				else
				{
					m_List.InsertItem(k, _T(""), ICON_GOOD + m_bGreenMode);
				}
			}
			// Life for WDC/SanDisk
			else if (m_Ata.vars[i].Attribute[j].Id == 0xE6 && (m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_WDC || m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_SANDISK))
			{
				int life;

				if (m_Ata.vars[i].FlagLifeSanDisk0_1)
				{
					life = 100 - (m_Ata.vars[i].Attribute[j].RawValue[1] * 256 + m_Ata.vars[i].Attribute[j].RawValue[0]) / 100;
				}
				else
				{
					life = 100 - m_Ata.vars[i].Attribute[j].RawValue[1];
				}

				if (life <= 0) { life = 0; }
				if (life == 0)
				{
					if (flag)
					{
						m_List.SetItem(k, 0, mask, _T(""), ICON_BAD, 0, 0, 0, 0);
					}
					else
					{
						m_List.InsertItem(k, _T(""), ICON_BAD);
					}
				}
				else if (life <= m_Ata.vars[i].ThresholdFF)
				{
					if (flag)
					{
						m_List.SetItem(k, 0, mask, _T(""), ICON_CAUTION, 0, 0, 0, 0);
					}
					else
					{
						m_List.InsertItem(k, _T(""), ICON_CAUTION);
					}
				}
				else
				{
					if (flag)
					{
						m_List.SetItem(k, 0, mask, _T(""), ICON_GOOD + m_bGreenMode, 0, 0, 0, 0);
					}
					else
					{
						m_List.InsertItem(k, _T(""), ICON_GOOD + m_bGreenMode);
					}
				}
			}
			// Life
			else if (
				   (m_Ata.vars[i].Attribute[j].Id == 0xA9 && (m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_REALTEK || (m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_KINGSTON && m_Ata.vars[i].HostReadsWritesUnit == m_Ata.HOST_READS_WRITES_32MB)))
				|| (m_Ata.vars[i].Attribute[j].Id == 0xAD && (m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_TOSHIBA || m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_KIOXIA))
				|| (m_Ata.vars[i].Attribute[j].Id == 0xB1 && m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_SAMSUNG)
				|| (m_Ata.vars[i].Attribute[j].Id == 0xBB && m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_MTRON)
				|| (m_Ata.vars[i].Attribute[j].Id == 0xCA && (m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_MICRON || m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_MICRON_MU02 || m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_INTEL_DC))
				|| (m_Ata.vars[i].Attribute[j].Id == 0xD1 && m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_INDILINX)
				|| (m_Ata.vars[i].Attribute[j].Id == 0xE7 && (m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_SANDFORCE || m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_CORSAIR || m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_KINGSTON || m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_SKHYNIX || m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_REALTEK || m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_SANDISK || m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_SSSTC || m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_APACER || m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_JMICRON || m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_SEAGATE || m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_MAXIOTEK || m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_YMTC || m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_SCY))
				|| (m_Ata.vars[i].Attribute[j].Id == 0xE8 && m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_PLEXTOR)
				|| (m_Ata.vars[i].Attribute[j].Id == 0xE9 && (m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_INTEL || m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_OCZ || m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_OCZ_VECTOR || m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_SKHYNIX))
				|| (m_Ata.vars[i].Attribute[j].Id == 0xE9 && m_Ata.vars[i].FlagLifeSanDiskLenovo)
				)
			{
				if (m_Ata.vars[i].FlagLifeRawValue)
				{
					if (flag)
					{
						m_List.SetItem(k, 0, mask, _T(""), ICON_GOOD + m_bGreenMode, 0, 0, 0, 0);
					}
					else
					{
						m_List.InsertItem(k, _T(""), ICON_GOOD + m_bGreenMode);
					}
				}
				else if (m_Ata.vars[i].Attribute[j].CurrentValue == 0
					|| m_Ata.vars[i].Attribute[j].CurrentValue < m_Ata.vars[i].Threshold[j].ThresholdValue)
				{
					if (flag)
					{
						m_List.SetItem(k, 0, mask, _T(""), ICON_BAD, 0, 0, 0, 0);
					}
					else
					{
						m_List.InsertItem(k, _T(""), ICON_BAD);
					}
				}
				else if (m_Ata.vars[i].Attribute[j].CurrentValue <= m_Ata.vars[i].ThresholdFF)
				{
					if (flag)
					{
						m_List.SetItem(k, 0, mask, _T(""), ICON_CAUTION, 0, 0, 0, 0);
					}
					else
					{
						m_List.InsertItem(k, _T(""), ICON_CAUTION);
					}
				}
				else
				{
					if (flag)
					{
						m_List.SetItem(k, 0, mask, _T(""), ICON_GOOD + m_bGreenMode, 0, 0, 0, 0);
					}
					else
					{
						m_List.InsertItem(k, _T(""), ICON_GOOD + m_bGreenMode);
					}
				}
			}
			else if (m_Ata.vars[i].Attribute[j].Id == 0x01 // Read Error Rate for SandForce Bug
				&& m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_SANDFORCE)
			{
				if (m_Ata.vars[i].Attribute[j].CurrentValue == 0
					&& m_Ata.vars[i].Attribute[j].RawValue[0] == 0
					&& m_Ata.vars[i].Attribute[j].RawValue[1] == 0)
				{
					if (flag)
					{
						m_List.SetItem(k, 0, mask, _T(""), ICON_GOOD + m_bGreenMode, 0, 0, 0, 0);
					}
					else
					{
						m_List.InsertItem(k, _T(""), ICON_GOOD + m_bGreenMode);
					}
				}
				else if (m_Ata.vars[i].Threshold[j].ThresholdValue != 0
					&& m_Ata.vars[i].Attribute[j].CurrentValue < m_Ata.vars[i].Threshold[j].ThresholdValue)
				{
					if (flag)
					{
						m_List.SetItem(k, 0, mask, _T(""), ICON_BAD, 0, 0, 0, 0);
					}
					else
					{
						m_List.InsertItem(k, _T(""), ICON_BAD);
					}
				}
				else
				{
					if (flag)
					{
						m_List.SetItem(k, 0, mask, _T(""), ICON_GOOD + m_bGreenMode, 0, 0, 0, 0);
					}
					else
					{
						m_List.InsertItem(k, _T(""), ICON_GOOD + m_bGreenMode);
					}
				}
			}
			else if ((m_Ata.vars[i].IsSsd && !m_Ata.vars[i].IsRawValues8)
				|| ((0x01 <= m_Ata.vars[i].Attribute[j].Id && m_Ata.vars[i].Attribute[j].Id <= 0x0D)
					||	m_Ata.vars[i].Attribute[j].Id == 0x16
					|| (0xBB <= m_Ata.vars[i].Attribute[j].Id && m_Ata.vars[i].Attribute[j].Id <= 0xC1)
					|| (0xC3 <= m_Ata.vars[i].Attribute[j].Id && m_Ata.vars[i].Attribute[j].Id <= 0xD1)
					|| (0xD3 <= m_Ata.vars[i].Attribute[j].Id && m_Ata.vars[i].Attribute[j].Id <= 0xD4)
					|| (0xDC <= m_Ata.vars[i].Attribute[j].Id && m_Ata.vars[i].Attribute[j].Id <= 0xE4)
					|| (0xE6 <= m_Ata.vars[i].Attribute[j].Id && m_Ata.vars[i].Attribute[j].Id <= 0xE7)
					|| m_Ata.vars[i].Attribute[j].Id == 0xF0
					|| m_Ata.vars[i].Attribute[j].Id == 0xFA
					|| m_Ata.vars[i].Attribute[j].Id == 0xFE
					))
			{
				if (m_Ata.vars[i].Threshold[j].ThresholdValue != 0
					&& m_Ata.vars[i].Attribute[j].CurrentValue < m_Ata.vars[i].Threshold[j].ThresholdValue)
				{
					if (flag)
					{
						m_List.SetItem(k, 0, mask, _T(""), ICON_BAD, 0, 0, 0, 0);
					}
					else
					{
						m_List.InsertItem(k, _T(""), ICON_BAD);
					}
				}
				else
				{
					if (flag)
					{
						m_List.SetItem(k, 0, mask, _T(""), ICON_GOOD + m_bGreenMode, 0, 0, 0, 0);
					}
					else
					{
						m_List.InsertItem(k, _T(""), ICON_GOOD + m_bGreenMode);
					}
				}
			}
			else
			{
				if (flag)
				{
					m_List.SetItem(k, 0, mask, _T(""), ICON_GOOD + m_bGreenMode, 0, 0, 0, 0);
				}
				else
				{
					m_List.InsertItem(k, _T(""), ICON_GOOD + m_bGreenMode);
				}
			}
		}
		else
		{
			if (flag)
			{
				m_List.SetItem(k, 0, mask, _T(""), ICON_UNKNOWN, 0, 0, 0, 0);
			}
			else
			{
				m_List.InsertItem(k, _T(""), ICON_UNKNOWN);
			}
		}

		cstr.Format(_T("%02X"), m_Ata.vars[i].Attribute[j].Id);
		m_List.SetItemText(k, 1, cstr);

		BYTE id = m_Ata.vars[i].Attribute[j].Id;

		if (m_bSmartEnglish)
		{
			GetPrivateProfileString(m_Ata.vars[i].SmartKeyName, cstr, vendorSpecific, str, 256, m_DefaultLangPath);
		}
		else
		{
			GetPrivateProfileString(m_Ata.vars[i].SmartKeyName, cstr, L"", str, 256, m_DefaultLangPath);
			CString en = str;
			if (en.IsEmpty())
			{
				GetPrivateProfileString(m_Ata.vars[i].SmartKeyName, cstr, vendorSpecific, str, 256, m_CurrentLangPath);
			}
			else
			{
				GetPrivateProfileString(m_Ata.vars[i].SmartKeyName, cstr, en, str, 256, m_CurrentLangPath);
			}
		}

		m_List.SetItemText(k, 2, str);

		if (m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_SANDFORCE)
		{
			cstr.Format(_T("%d"), m_Ata.vars[i].Attribute[j].CurrentValue);
			cstr.Format(_T("%d"), m_Ata.vars[i].Attribute[j].CurrentValue);
			m_List.SetItemText(k, 3, cstr);
			cstr.Format(_T("%d"), m_Ata.vars[i].Attribute[j].WorstValue);
			m_List.SetItemText(k, 4, cstr);
			if (m_Ata.vars[i].IsThresholdCorrect)
			{
				cstr.Format(_T("%d"), m_Ata.vars[i].Threshold[j].ThresholdValue);
			}
			else
			{
				cstr = _T("---");
			}
			m_List.SetItemText(k, 5, cstr);

			switch (m_RawValues)
			{
			case 3:
				cstr.Format(_T("%d %d %d %d %d %d %d"),
					m_Ata.vars[i].Attribute[j].Reserved,
					m_Ata.vars[i].Attribute[j].RawValue[5],
					m_Ata.vars[i].Attribute[j].RawValue[4],
					m_Ata.vars[i].Attribute[j].RawValue[3],
					m_Ata.vars[i].Attribute[j].RawValue[2],
					m_Ata.vars[i].Attribute[j].RawValue[1],
					m_Ata.vars[i].Attribute[j].RawValue[0]);
				break;
			case 2:
				cstr.Format(_T("%d %d %d %d"),
					m_Ata.vars[i].Attribute[j].Reserved,
					MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[4], m_Ata.vars[i].Attribute[j].RawValue[5]),
					MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[2], m_Ata.vars[i].Attribute[j].RawValue[3]),
					MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[0], m_Ata.vars[i].Attribute[j].RawValue[1]));
				break;
			case 1:
				cstr.Format(_T("%I64u"),
					((UINT64)m_Ata.vars[i].Attribute[j].Reserved << 48) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[5] << 40) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[4] << 32) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[3] << 24) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[2] << 16) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[1] << 8) +
					(UINT64)m_Ata.vars[i].Attribute[j].RawValue[0]);
				break;
			case 0:
			default:
				cstr.Format(_T("%02X%02X%02X%02X%02X%02X%02X"),
					m_Ata.vars[i].Attribute[j].Reserved,
					m_Ata.vars[i].Attribute[j].RawValue[5],
					m_Ata.vars[i].Attribute[j].RawValue[4],
					m_Ata.vars[i].Attribute[j].RawValue[3],
					m_Ata.vars[i].Attribute[j].RawValue[2],
					m_Ata.vars[i].Attribute[j].RawValue[1],
					m_Ata.vars[i].Attribute[j].RawValue[0]);
				break;
			}
			//	m_List.SetItemText(k, 6, _T("DDDDDDDDDDDDDD"));
			m_List.SetItemText(k, 6, cstr);
		}
		else if (m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_JMICRON && m_Ata.vars[i].IsRawValues8)
		{
			cstr.Format(_T("%d"), m_Ata.vars[i].Attribute[j].CurrentValue);
			m_List.SetItemText(k, 3, cstr);
			m_List.SetItemText(k, 4, _T("---"));
			m_List.SetItemText(k, 5, _T("---"));
			switch (m_RawValues)
			{
			case 3:
				cstr.Format(_T("%d %d %d %d %d %d %d %d"),
					m_Ata.vars[i].Attribute[j].Reserved,
					m_Ata.vars[i].Attribute[j].RawValue[5],
					m_Ata.vars[i].Attribute[j].RawValue[4],
					m_Ata.vars[i].Attribute[j].RawValue[3],
					m_Ata.vars[i].Attribute[j].RawValue[2],
					m_Ata.vars[i].Attribute[j].RawValue[1],
					m_Ata.vars[i].Attribute[j].RawValue[0],
					m_Ata.vars[i].Attribute[j].WorstValue);
				break;
			case 2:
				cstr.Format(_T("%d %d %d %d"),
					MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[5], m_Ata.vars[i].Attribute[j].Reserved),
					MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[3], m_Ata.vars[i].Attribute[j].RawValue[4]),
					MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[1], m_Ata.vars[i].Attribute[j].RawValue[2]),
					MAKEWORD(m_Ata.vars[i].Attribute[j].WorstValue, m_Ata.vars[i].Attribute[j].RawValue[0]));
				break;
			case 1:
				cstr.Format(_T("%I64u"),
					((UINT64)m_Ata.vars[i].Attribute[j].Reserved << 56) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[5] << 48) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[4] << 40) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[3] << 32) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[2] << 24) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[1] << 16) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[0] << 8) +
					(UINT64)m_Ata.vars[i].Attribute[j].WorstValue);
				break;
			case 0:
			default:
				cstr.Format(_T("%02X%02X%02X%02X%02X%02X%02X%02X"),
					m_Ata.vars[i].Attribute[j].Reserved,
					m_Ata.vars[i].Attribute[j].RawValue[5],
					m_Ata.vars[i].Attribute[j].RawValue[4],
					m_Ata.vars[i].Attribute[j].RawValue[3],
					m_Ata.vars[i].Attribute[j].RawValue[2],
					m_Ata.vars[i].Attribute[j].RawValue[1],
					m_Ata.vars[i].Attribute[j].RawValue[0],
					m_Ata.vars[i].Attribute[j].WorstValue);
				break;
			}
			m_List.SetItemText(k, 6, cstr);
			//	m_List.SetItemText(k, 6, _T("DDDDDDDDDDDDDDDD"));
		}
		else if (m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_INDILINX)
		{
			m_List.SetItemText(k, 3, _T("---"));
			m_List.SetItemText(k, 4, _T("---"));
			m_List.SetItemText(k, 5, _T("---"));
			switch (m_RawValues)
			{
			case 3:
				cstr.Format(_T("%d %d %d %d %d %d %d %d"),
					m_Ata.vars[i].Attribute[j].RawValue[5],
					m_Ata.vars[i].Attribute[j].RawValue[4],
					m_Ata.vars[i].Attribute[j].RawValue[3],
					m_Ata.vars[i].Attribute[j].RawValue[2],
					m_Ata.vars[i].Attribute[j].RawValue[1],
					m_Ata.vars[i].Attribute[j].RawValue[0],
					m_Ata.vars[i].Attribute[j].WorstValue,
					m_Ata.vars[i].Attribute[j].CurrentValue);
				break;
			case 2:
				cstr.Format(_T("%d %d %d %d"),
					MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[4], m_Ata.vars[i].Attribute[j].RawValue[5]),
					MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[2], m_Ata.vars[i].Attribute[j].RawValue[3]),
					MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[0], m_Ata.vars[i].Attribute[j].RawValue[1]),
					MAKEWORD(m_Ata.vars[i].Attribute[j].CurrentValue, m_Ata.vars[i].Attribute[j].WorstValue));
				break;
			case 1:
				cstr.Format(_T("%I64u"),
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[5] << 56) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[4] << 48) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[3] << 40) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[2] << 32) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[1] << 24) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[0] << 16) +
					((UINT64)m_Ata.vars[i].Attribute[j].WorstValue << 8) +
					(UINT64)m_Ata.vars[i].Attribute[j].CurrentValue);
				break;
			case 0:
			default:
				cstr.Format(_T("%02X%02X%02X%02X%02X%02X%02X%02X"),
					m_Ata.vars[i].Attribute[j].RawValue[5],
					m_Ata.vars[i].Attribute[j].RawValue[4],
					m_Ata.vars[i].Attribute[j].RawValue[3],
					m_Ata.vars[i].Attribute[j].RawValue[2],
					m_Ata.vars[i].Attribute[j].RawValue[1],
					m_Ata.vars[i].Attribute[j].RawValue[0],
					m_Ata.vars[i].Attribute[j].WorstValue,
					m_Ata.vars[i].Attribute[j].CurrentValue);
				break;
			}
			m_List.SetItemText(k, 6, cstr);
			//	m_List.SetItemText(k, 6, _T("DDDDDDDDDDDDDDDD"));
		}
		else if (m_Ata.vars[i].DiskVendorId == m_Ata.SSD_VENDOR_NVME)
		{
			cstr = _T("---");
			m_List.SetItemText(k, 3, cstr);
			m_List.SetItemText(k, 4, cstr);
			m_List.SetItemText(k, 5, cstr);
			switch (m_RawValues)
			{
			case 3:
				cstr.Format(_T("%d %d %d %d %d %d %d"),
					m_Ata.vars[i].Attribute[j].Reserved,
					m_Ata.vars[i].Attribute[j].RawValue[5],
					m_Ata.vars[i].Attribute[j].RawValue[4],
					m_Ata.vars[i].Attribute[j].RawValue[3],
					m_Ata.vars[i].Attribute[j].RawValue[2],
					m_Ata.vars[i].Attribute[j].RawValue[1],
					m_Ata.vars[i].Attribute[j].RawValue[0]);
				break;
			case 2:
				cstr.Format(_T("%d %d %d %d"),
					m_Ata.vars[i].Attribute[j].Reserved,
					MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[4], m_Ata.vars[i].Attribute[j].RawValue[5]),
					MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[2], m_Ata.vars[i].Attribute[j].RawValue[3]),
					MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[0], m_Ata.vars[i].Attribute[j].RawValue[1]));
				break;
			case 1:
				cstr.Format(_T("%I64u"),
					((UINT64)m_Ata.vars[i].Attribute[j].Reserved << 48) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[5] << 40) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[4] << 32) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[3] << 24) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[2] << 16) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[1] << 8) +
					(UINT64)m_Ata.vars[i].Attribute[j].RawValue[0]);
				break;
			case 0:
			default:
				cstr.Format(_T("%02X%02X%02X%02X%02X%02X%02X"),
					m_Ata.vars[i].Attribute[j].Reserved,
					m_Ata.vars[i].Attribute[j].RawValue[5],
					m_Ata.vars[i].Attribute[j].RawValue[4],
					m_Ata.vars[i].Attribute[j].RawValue[3],
					m_Ata.vars[i].Attribute[j].RawValue[2],
					m_Ata.vars[i].Attribute[j].RawValue[1],
					m_Ata.vars[i].Attribute[j].RawValue[0]);
				break;
			}
			m_List.SetItemText(k, 6, cstr);
		}
		else
		{
			cstr.Format(_T("%d"), m_Ata.vars[i].Attribute[j].CurrentValue);
			m_List.SetItemText(k, 3, cstr);
			cstr.Format(_T("%d"), m_Ata.vars[i].Attribute[j].WorstValue);
			m_List.SetItemText(k, 4, cstr);
			if (m_Ata.vars[i].IsThresholdCorrect)
			{
				cstr.Format(_T("%d"), m_Ata.vars[i].Threshold[j].ThresholdValue);
			}
			else
			{
				cstr = _T("---");
			}
			m_List.SetItemText(k, 5, cstr);

			switch (m_RawValues)
			{
			case 3:
				cstr.Format(_T("%d %d %d %d %d %d"),
					m_Ata.vars[i].Attribute[j].RawValue[5],
					m_Ata.vars[i].Attribute[j].RawValue[4],
					m_Ata.vars[i].Attribute[j].RawValue[3],
					m_Ata.vars[i].Attribute[j].RawValue[2],
					m_Ata.vars[i].Attribute[j].RawValue[1],
					m_Ata.vars[i].Attribute[j].RawValue[0]);
				break;
			case 2:
				cstr.Format(_T("%d %d %d"),
					MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[4], m_Ata.vars[i].Attribute[j].RawValue[5]),
					MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[2], m_Ata.vars[i].Attribute[j].RawValue[3]),
					MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[0], m_Ata.vars[i].Attribute[j].RawValue[1]));
				break;
			case 1:
				cstr.Format(_T("%I64u"),
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[5] << 40) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[4] << 32) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[3] << 24) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[2] << 16) +
					((UINT64)m_Ata.vars[i].Attribute[j].RawValue[1] << 8) +
					(UINT64)m_Ata.vars[i].Attribute[j].RawValue[0]);
				break;
			case 0:
			default:
				cstr.Format(_T("%02X%02X%02X%02X%02X%02X"),
					m_Ata.vars[i].Attribute[j].RawValue[5],
					m_Ata.vars[i].Attribute[j].RawValue[4],
					m_Ata.vars[i].Attribute[j].RawValue[3],
					m_Ata.vars[i].Attribute[j].RawValue[2],
					m_Ata.vars[i].Attribute[j].RawValue[1],
					m_Ata.vars[i].Attribute[j].RawValue[0]);
				break;
			}
			//	m_List.SetItemText(k, 6, _T("DDDDDDDDDDDD"));
			m_List.SetItemText(k, 6, cstr);
		}

		k++;
	}

	if (!flag)
	{
		m_List.SetRedraw(TRUE);
	}

	return TRUE;
}

BOOL CDiskInfoDlg::ChangeDisk(DWORD i)
{
	BOOL flagUpdate = FALSE;

	//	m_CtrlDiskStatus.SetToolTipText(L"");
	m_CtrlDriveMap.SetToolTipText(L"");
	m_CtrlModel.SetToolTipText(L"");
	m_CtrlPowerOnHours.SetToolTipText(L"");
	m_CtrlPowerOnCount.SetToolTipText(L"");
	m_CtrlWAF.SetToolTipText(L"");
	m_CtrlBufferSize.SetToolTipText(L"");
	m_CtrlNvCacheSize.SetToolTipText(L"");
	m_CtrlRotationRate.SetToolTipText(L"");
	m_CtrlTransferMode.SetToolTipText(L"");
	m_CtrlAtaAtapi.SetToolTipText(L"");

	if (m_Ata.vars.GetCount() == 0)
	{
		m_Model = _T("");
		m_ModelCapacity = i18n(_T("Message"), _T("DISK_NOT_FOUND"));
		m_Firmware = _T("");
		m_SerialNumber = _T("");
		m_WAF = _T("");
		m_PowerOnCount = _T("");
		m_PowerOnHours = _T("");
		m_BufferSize = _T("");
		m_NvCacheSize = _T("");
		m_RotationRate = _T("");
		m_LbaSize = _T("");
		m_Capacity = _T("");
		m_TransferMode = _T("");
		m_Interface = _T("");
		m_AtaAtapi = _T("");
		m_DiskStatus = _T("");
		m_Temperature = _T("");
		m_Life = _T("");
		m_Feature = _T("");
		m_DriveMap = _T("");
		UpdateData(FALSE);
		UpdateListCtrl(i);

		m_CtrlDiskStatus.ReloadImage(IP(L"diskStatusUnknown"), 1);
		m_CtrlTemperature.ReloadImage(IP(L"temperatureUnknown"), 1);

		return FALSE;
	}

	static int preSelectedDisk = -1;
	if (preSelectedDisk != i)
	{
		flagUpdate = TRUE;
		preSelectedDisk = i;
	}

	static int preFlagFahrenheit = -1;
	if (preFlagFahrenheit != m_bFahrenheit)
	{
		flagUpdate = TRUE;
		preFlagFahrenheit = m_bFahrenheit;
	}

	static CString preLang = _T("");
	if (preLang.Compare(m_CurrentLang) != 0)
	{
		flagUpdate = TRUE;
		preLang = m_CurrentLang;
	}

	CString cstr, diskStatus, diskStatusReason, className, logicalDriveInfo;
	static CString preDiskStatus = _T("");
	static CString preDiskStatusReason = _T("");
	static CString preTemperatureStatus = _T("");
	static CString preLogicalDriveInfo = _T("");
	static BOOL preFlagHideSerialNumber = FALSE;

	diskStatus = GetDiskStatus(m_Ata.vars[i].DiskStatus);
	className = GetDiskStatusClass(m_Ata.vars[i].DiskStatus);
	diskStatusReason = GetDiskStatusReason(i);

	if (preDiskStatus.Compare(className) != 0)
	{
		preDiskStatus = className;
	}
	if (preDiskStatusReason.Compare(diskStatusReason) != 0)
	{
		preDiskStatusReason = diskStatusReason;
	}

	if (m_Ata.vars[i].Life >= 0)
	{
#ifdef SUISHO_SHIZUKU_SUPPORT
		cstr.Format(_T(" (%d %%)"), m_Ata.vars[i].Life);
#else
		cstr.Format(_T("\n%d %%"), m_Ata.vars[i].Life);
#endif
		m_DiskStatus.Format(_T("%s %s"), diskStatus, cstr);
	}
	else
	{
		m_DiskStatus.Format(_T("%s"), diskStatus);
	}
	m_CtrlDiskStatus.SetToolTipText(diskStatusReason);
	m_CtrlDiskStatus.ReloadImage(IP(className), 1);

#ifdef SUISHO_SHIZUKU_SUPPORT
	className.Replace(L"Green", L"");
	if (m_Ata.vars[i].DiskStatus == CAtaSmart::DISK_STATUS_GOOD && m_Ata.vars[i].Life == 100)
	{
		className += L"100";
	}
	 m_CtrlLife.ReloadImage(IP(L"SD" + className), 1);
#endif

	className = GetTemperatureClass(m_Ata.vars[i].Temperature, m_Ata.vars[i].AlarmTemperature);

	if (preTemperatureStatus.Compare(className) != 0)
	{
		preTemperatureStatus = className;
	}

	// static int preTemperature = -1;
	// if(preTemperature != m_Ata.vars[i].Temperature || flagUpdate)
	{
		if (m_Ata.vars[i].Temperature > -300)
		{
			if (m_bFahrenheit)
			{
				m_Temperature.Format(_T("%d °F"), m_Ata.vars[i].Temperature * 9 / 5 + 32);
			}
			else
			{
				m_Temperature.Format(_T("%d °C"), m_Ata.vars[i].Temperature);
			}
		}
		else
		{
			if (m_bFahrenheit)
			{
				m_Temperature.Format(_T("-- °F"));
			}
			else
			{
				m_Temperature.Format(_T("-- °C"));
			}
		}
	}
	m_CtrlTemperature.ReloadImage(IP(className), 1);

	logicalDriveInfo = GetLogicalDriveInfo(i);
	//	if(preLogicalDriveInfo.Compare(logicalDriveInfo) != 0)
	{
		preLogicalDriveInfo = logicalDriveInfo;
		m_CtrlDriveMap.SetToolTipText(logicalDriveInfo);
	}

	/*
	if(m_Ata.vars[i].Sector48 >= m_Ata.vars[i].Sector28)
	{
	cstr.Format(_T("%I64d"), m_Ata.vars[i].Sector48);
	//	SetElementPropertyEx(_T("LbaSize"), DISPID_IHTMLELEMENT_TITLE, cstr);
	//	m_LabelLbaSize = _T("48bit LBA");
	}
	else if(m_Ata.vars[i].Sector28 > 0)
	{
	cstr.Format(_T("%d"), m_Ata.vars[i].Sector28);
	//	SetElementPropertyEx(_T("LbaSize"), DISPID_IHTMLELEMENT_TITLE, cstr);
	//	m_LabelLbaSize = _T("28bit LBA");
	}
	else
	{
	cstr = _T("");
	//	SetElementPropertyEx(_T("LbaSize"), DISPID_IHTMLELEMENT_TITLE, cstr);
	//	m_LabelLbaSize = _T("CHS");
	}
	*/

	if (preFlagHideSerialNumber != m_bHideSerialNumber)
	{
		preFlagHideSerialNumber = m_bHideSerialNumber;
		flagUpdate = TRUE;
	}

	if (m_bHideSerialNumber)
	{
		m_SerialNumber = _T("");
		for (int j = 0; j < m_Ata.vars[i].SerialNumber.GetLength(); j++)
		{
			m_SerialNumber += _T("*");
		}
	}
	else
	{
		m_SerialNumber = m_Ata.vars[i].SerialNumber;
	}

	m_Model = m_Ata.vars[i].Model;
	m_Firmware = m_Ata.vars[i].FirmwareRev;

	static int prePowerOnCount = -1;
	if (flagUpdate || prePowerOnCount != m_Ata.vars[i].PowerOnCount)
	{
		if (m_Ata.vars[i].PowerOnCount > 0)
		{
			m_PowerOnCount.Format(_T("%d %s"), m_Ata.vars[i].PowerOnCount, i18n(_T("Dialog"), _T("POWER_ON_COUNT_UNIT")));
		}
		else
		{
			m_PowerOnCount = i18n(_T("Dialog"), _T("UNKNOWN"));
		}
		prePowerOnCount = m_Ata.vars[i].PowerOnCount;
		flagUpdate = TRUE;
	}

	CString IsMinutes;
	CString IsMinutesT;
	CString title;	static int prePowerOnHours = -1;

	const INT powerOnHours = m_Ata.vars[i].MeasuredPowerOnHours > 0 ? m_Ata.vars[i].MeasuredPowerOnHours : m_Ata.vars[i].DetectedPowerOnHours;
	const DWORD timeUnitType = m_Ata.vars[i].MeasuredPowerOnHours > 0 ? m_Ata.vars[i].MeasuredTimeUnitType : m_Ata.vars[i].DetectedTimeUnitType;

	if (powerOnHours >= 0)
	{
		//if(flagUpdate || prePowerOnHours != powerOnHours)
		{
			if (timeUnitType == CAtaSmart::POWER_ON_MINUTES && m_Ata.vars[i].IsMaxtorMinute)
			{
				IsMinutes = _T("?");
				IsMinutesT = _T(" (?)");
			}
			else
			{
				IsMinutes = _T(" ");
				IsMinutesT = _T("");
			}

			const int years = powerOnHours / (365 * 24);
			const int days = (powerOnHours - 365 * 24 * years) / 24;
			const int hours = powerOnHours % 24;
			
			if (m_NowDetectingUnitPowerOnHours)
			{
				if (years > 0)
				{
					title.Format(_T("%d %s %d %s %d %s%s\r\n%s"),
						years, i18n(_T("Dialog"), _T("POWER_ON_YEARS_UNIT")),
						days, i18n(_T("Dialog"), _T("POWER_ON_DAYS_UNIT")),
						hours, i18n(_T("Dialog"), _T("POWER_ON_HOURS_UNIT")),
						IsMinutesT, i18n(_T("Message"), _T("DETECT_UNIT_POWER_ON_HOURS")));
				}
				else
				{
					title.Format(_T("%d %s %d %s%s\r\n%s"),
						days, i18n(_T("Dialog"), _T("POWER_ON_DAYS_UNIT")),
						hours, i18n(_T("Dialog"), _T("POWER_ON_HOURS_UNIT")),
						IsMinutesT, i18n(_T("Message"), _T("DETECT_UNIT_POWER_ON_HOURS")));
				}
			}
			else
			{
				if (years > 0)
				{
					title.Format(_T("%d %s %d %s %d %s%s"),
						years, i18n(_T("Dialog"), _T("POWER_ON_YEARS_UNIT")),
						days, i18n(_T("Dialog"), _T("POWER_ON_DAYS_UNIT")),
						hours, i18n(_T("Dialog"), _T("POWER_ON_HOURS_UNIT")),
						IsMinutesT);
				}
				else
				{
					title.Format(_T("%d %s %d %s%s"),
						days, i18n(_T("Dialog"), _T("POWER_ON_DAYS_UNIT")),
						hours, i18n(_T("Dialog"), _T("POWER_ON_HOURS_UNIT")),
						IsMinutesT);
				}
			}

			m_PowerOnHours.Format(_T("%d%s%s"),
				powerOnHours, IsMinutes, i18n(_T("Dialog"), _T("POWER_ON_HOURS_UNIT")));

			m_CtrlPowerOnHours.SetToolTipText(title);

			prePowerOnHours = powerOnHours;
			flagUpdate = TRUE;
		}
	}
	else
	{
		if (flagUpdate || prePowerOnHours != 0)
		{
			m_PowerOnHours = i18n(_T("Dialog"), _T("UNKNOWN"));
			prePowerOnHours = 0;
			flagUpdate = TRUE;
			m_CtrlPowerOnHours.SetToolTipText(L"");
		}
	}

	if (m_Ata.vars[i].HostReads >= 0)
	{
		cstr = L"";

		if(m_Ata.vars[i].HostReads > 1024 * 1024)
		{
			cstr.Format(_T("%.3f PB  "), m_Ata.vars[i].HostReads / 1024.0 / 1024.0);
		}
		else if(m_Ata.vars[i].HostReads > 1024)
		{
			cstr.Format(_T("%.3f TB  "), m_Ata.vars[i].HostReads / 1024.0);
		}

		m_BufferSize.Format(_T("%d GB"), m_Ata.vars[i].HostReads);
		m_LabelBufferSize = i18n(_T("Dialog"), _T("TOTAL_HOST_READS"));
		m_CtrlBufferSize.SetToolTipText(cstr);
		m_CtrlLabelBufferSize.SetToolTipText(i18n(_T("Dialog"), _T("TOTAL_HOST_READS")));
	}
	else if (m_Ata.vars[i].BufferSize == 0xFFFF * 512)
	{
		m_BufferSize.Format(_T(">= 32 MB"));
		m_LabelBufferSize = i18n(_T("Dialog"), _T("BUFFER_SIZE"));
		m_CtrlBufferSize.SetToolTipText(L"");
		m_CtrlLabelBufferSize.SetToolTipText(i18n(_T("Dialog"), _T("BUFFER_SIZE")));
	}
	else if (m_Ata.vars[i].BufferSize > 0)
	{
		m_BufferSize.Format(_T("%d KB"), m_Ata.vars[i].BufferSize / 1024);
		m_LabelBufferSize = i18n(_T("Dialog"), _T("BUFFER_SIZE"));
		m_CtrlBufferSize.SetToolTipText(L"");
		m_CtrlLabelBufferSize.SetToolTipText(i18n(_T("Dialog"), _T("BUFFER_SIZE")));
	}
	else
	{
		m_BufferSize = _T("----");
		m_LabelBufferSize = _T("----");
		m_CtrlBufferSize.SetToolTipText(L"");
		m_CtrlLabelBufferSize.SetToolTipText(L"");
	}

	if (m_Ata.vars[i].HostWrites >= 0)
	{
		cstr = L"";

		if(m_Ata.vars[i].HostWrites > 1024 * 1024)
		{
			cstr.Format(_T("%.3f PB  "), m_Ata.vars[i].HostWrites / 1024.0 / 1024.0);
		}
		else if(m_Ata.vars[i].HostWrites > 1024)
		{
			cstr.Format(_T("%.3f TB  "), m_Ata.vars[i].HostWrites / 1024.0);
		}

		m_NvCacheSize.Format(_T("%d GB"), m_Ata.vars[i].HostWrites);
		m_LabelNvCacheSize = i18n(_T("Dialog"), _T("TOTAL_HOST_WRITES"));
		m_CtrlNvCacheSize.SetToolTipText(cstr);
		m_CtrlLabelNvCacheSize.SetToolTipText(i18n(_T("Dialog"), _T("TOTAL_HOST_WRITES")));
	}
	else if (m_Ata.vars[i].NvCacheSize > 0)
	{
		m_NvCacheSize.Format(_T("%d MB"), (DWORD)(m_Ata.vars[i].NvCacheSize / 1024 / 1024));
		m_LabelNvCacheSize = i18n(_T("Dialog"), _T("NV_CACHE_SIZE"));
		m_CtrlNvCacheSize.SetToolTipText(L"");
		m_CtrlLabelNvCacheSize.SetToolTipText(i18n(_T("Dialog"), _T("NV_CACHE_SIZE")));
	}
	else
	{
		m_NvCacheSize = _T("----");
		m_LabelNvCacheSize = _T("----");
		m_CtrlNvCacheSize.SetToolTipText(L"");
		m_CtrlLabelNvCacheSize.SetToolTipText(L"");
	}

	if (m_Ata.vars[i].NandWrites >= 0)
	{
		cstr = L"";
		/*
		if(m_Ata.vars[i].NandWrites > 1024 * 1024)
		{
		cstr.Format(_T("%.3f PB"), m_Ata.vars[i].NandWrites / 1024.0 / 1024.0);
		}
		else if(m_Ata.vars[i].NandWrites > 1024)
		{
		cstr.Format(_T("%.3f TB"), m_Ata.vars[i].NandWrites / 1024.0);
		}
		*/
		m_RotationRate.Format(_T("%d GB"), m_Ata.vars[i].NandWrites);
		/*
		if (m_Ata.vars[i].PlextorNandWritesUnit)
		{
		m_RotationRate += L" (?)";
		cstr += L" (?)";
		}
		*/

		SetLabel(m_CtrlLabelRotationRate, m_LabelRotationRate, i18n(_T("Dialog"), _T("TOTAL_NAND_WRITES")));
		m_CtrlRotationRate.SetToolTipText(cstr);

		SetLabel(m_CtrlLabelWAF, m_LabelWAF, i18n(_T("Dialog"), _T("WAF")));
	}
	else if (m_Ata.vars[i].GBytesErased >= 0)
	{
		cstr = L"";
		/*
		if(m_Ata.vars[i].GBytesErased > 1024 * 1024)
		{
		cstr.Format(_T("%.3f PB"), m_Ata.vars[i].GBytesErased / 1024.0 / 1024.0);
		}
		else if(m_Ata.vars[i].GBytesErased > 1024)
		{
		cstr.Format(_T("%.3f TB"), m_Ata.vars[i].GBytesErased / 1024.0);
		}
		*/
		m_RotationRate.Format(_T("%d GB"), m_Ata.vars[i].GBytesErased);
		m_LabelRotationRate = i18n(_T("SmartSandForce"), _T("64"));
		m_CtrlRotationRate.SetToolTipText(cstr);
		m_CtrlLabelRotationRate.SetToolTipText(i18n(_T("SmartSandForce"), _T("64")));
	}
	/*
	else if(m_Ata.vars[i].WearLevelingCount >= 0)
	{
	m_RotationRate.Format(_T("%d %s"), m_Ata.vars[i].WearLevelingCount, i18n(_T("Dialog"), _T("POWER_ON_COUNT_UNIT")));
	//	m_LabelRotationRate = i18n(_T("Dialog"), _T("WEAR_LEVELING_COUNT"));
	SetLabel(m_LabelRotationRate, _T("RotationRate"),i18n(_T("Dialog"), _T("WEAR_LEVELING_COUNT")));
	//	SetElementPropertyEx(_T("RotationRate"), DISPID_IHTMLELEMENT_CLASSNAME, _T("supported"));
	}
	*/
	else if (m_Ata.vars[i].NominalMediaRotationRate == 1) // SSD
	{
		m_RotationRate = _T("---- (SSD)");
		m_LabelRotationRate = i18n(_T("Dialog"), _T("ROTATION_RATE"));
		m_CtrlRotationRate.SetToolTipText(L"");
		m_CtrlLabelRotationRate.SetToolTipText(i18n(_T("Dialog"), _T("ROTATION_RATE")));
	}
	else if (m_Ata.vars[i].NominalMediaRotationRate > 0)
	{
		m_RotationRate.Format(_T("%d RPM"), m_Ata.vars[i].NominalMediaRotationRate);
		m_LabelRotationRate = i18n(_T("Dialog"), _T("ROTATION_RATE"));
		m_CtrlLabelRotationRate.SetToolTipText(i18n(_T("Dialog"), _T("ROTATION_RATE")));
	}
	else
	{
		m_RotationRate = _T("----");
		m_LabelRotationRate = _T("----");
		m_WAF = _T("----");
		m_CtrlRotationRate.SetToolTipText(L"");
		m_CtrlLabelRotationRate.SetToolTipText(L"");
	}
	if (m_Ata.vars[i].HostWrites > 10 && m_Ata.vars[i].NandWrites > 0)
	{
		double d_waf = static_cast<double>(m_Ata.vars[i].NandWrites) / static_cast<double>(m_Ata.vars[i].HostWrites);
		m_WAF.Format(_T("%.4f"), d_waf);
		m_CtrlLabelWAF.SetToolTipText(i18n(_T("Dialog"), _T("WAF_TOOLTIP")));
	}
	else // refuse to a new or unsupported device
	{
		SetLabel(m_CtrlLabelWAF, m_LabelWAF, L"----");
		m_WAF = _T("----");
		m_CtrlLabelWAF.SetToolTipText(L"");
	}


	if (m_Ata.vars[i].DiskSizeLba48 >= m_Ata.vars[i].DiskSizeLba28)
	{
		m_LbaSize.Format(_T("%.1f GB"), m_Ata.vars[i].DiskSizeLba48 / 1000.0);
	}
	else if (m_Ata.vars[i].DiskSizeLba28 > 0)
	{
		m_LbaSize.Format(_T("%.1f GB"), m_Ata.vars[i].DiskSizeLba28 / 1000.0);
	}
	else if (m_Ata.vars[i].DiskSizeChs > 0)
	{
		m_LbaSize.Format(_T("%.1f GB"), m_Ata.vars[i].DiskSizeChs / 1000.0);
	}
	else
	{
		m_LbaSize = _T("----");
	}

	m_DriveMap = m_Ata.vars[i].DriveMap;
	if (m_Ata.vars[i].TotalDiskSize >= 1000)
	{
		m_Capacity.Format(_T("%.1f GB"), m_Ata.vars[i].TotalDiskSize / 1000.0);
	}
	else if (m_Ata.vars[i].TotalDiskSize > 0)
	{
		m_Capacity.Format(_T("%d MB"), m_Ata.vars[i].TotalDiskSize);
	}
	else
	{
		m_Capacity = i18n(_T("Dialog"), _T("UNKNOWN"));
	}
	
	if (m_Ata.vars[i].CurrentTransferMode.IsEmpty() && m_Ata.vars[i].MaxTransferMode.IsEmpty())
	{
		m_TransferMode = L"";
	}
	else
	{
		m_TransferMode = m_Ata.vars[i].CurrentTransferMode + _T(" | ") + m_Ata.vars[i].MaxTransferMode;
	}
	m_CtrlTransferMode.SetToolTipText(i18n(_T("Dialog"), _T("CURRENT_MODE_SUPPORTED_MODE")));

	m_Interface = m_Ata.vars[i].Interface;
	if (!m_Ata.vars[i].MinorVersion.IsEmpty())
	{
		m_AtaAtapi = m_Ata.vars[i].MajorVersion + _T(" | ") + m_Ata.vars[i].MinorVersion;
	}
	else
	{
		m_AtaAtapi = m_Ata.vars[i].MajorVersion;
	}

	m_CtrlAtaAtapi.SetToolTipText(i18n(_T("Dialog"), _T("MAJOR_VERSION_MINOR_VERSION")));
	m_Feature = _T("");
	
	if (m_Ata.vars[i].IsNVMe)
	{
		cstr = L"\
S.M.A.R.T.: Self-Monitoring, Analysis and Reporting Technology\n\
TRIM: Trim function of DATA SET MANAGEMENT command\n\
VolatileWriteCache: Volatile Write Cache Present\n\
";
	}
	else
	{
		cstr = L"\
S.M.A.R.T.: Self-Monitoring, Analysis and Reporting Technology\n\
APM: Advanced Power Management\n\
AAM: Automatic Acoustic Management\n\
NCQ: Native Command Queuing\n\
TRIM: Trim function of DATA SET MANAGEMENT command\n\
DevSleep: Device Sleep\
";
	}

	m_CtrlFeature.SetToolTipText(cstr);

	if (m_Ata.vars[i].IsSmartSupported)
	{
		m_Feature += _T("S.M.A.R.T., ");
	}

	if (m_Ata.vars[i].IsApmSupported)
	{
		m_Feature += _T("APM, ");
	}

	if (m_Ata.vars[i].IsAamSupported)
	{
		m_Feature += _T("AAM, ");
	}

	if (m_Ata.vars[i].IsNcqSupported)
	{
		m_Feature += _T("NCQ, ");
	}

	if (m_Ata.vars[i].IsTrimSupported)
	{
		m_Feature += _T("TRIM, ");
	}

	if (m_Ata.vars[i].IsDeviceSleepSupported)
	{
		m_Feature += _T("DevSleep, ");
	}

	if (m_Ata.vars[i].IsVolatileWriteCachePresent)
	{
		m_Feature += _T("VolatileWriteCache, ");
	}
	if (!m_Feature.IsEmpty())
	{
		m_Feature.Delete(m_Feature.GetLength() - 2, 2);
	}

	m_ModelCapacity = m_Model + _T(" ") + m_Capacity;

	m_CtrlModel.SetToolTipText(m_ModelCapacity + L"\r\n" + m_Ata.vars[i].Enclosure);

	/*
	if(m_Ata.vars[i].Life >= 0)
	{
	m_Life.Format(L"%d %%", m_Ata.vars[i].Life);
	}
	else
	{
	m_Life = L"";
	}
	*/

	UpdateData(FALSE);

	return TRUE;
}

void CDiskInfoDlg::ChangeLang(CString LangName)
{
	if (m_SelectDisk >= (DWORD)m_Ata.vars.GetCount())
	{
		m_SelectDisk = 0;
	}

	m_CurrentLangPath.Format(_T("%s\\%s.lang"), m_LangDir, LangName);
	CString cstr;
	CMenu *menu = GetMenu();
	CMenu subMenu;
	CMenu subSubMenu;
	UINT menuState = 0;

	cstr = i18n(_T("Menu"), _T("FILE"));
	menu->ModifyMenu(0, MF_BYPOSITION | MF_STRING, 0, cstr);
	cstr = i18n(_T("Menu"), _T("EDIT"));
	menu->ModifyMenu(1, MF_BYPOSITION | MF_STRING, 1, cstr);
	cstr = i18n(_T("Menu"), _T("FUNCTION"));
	menu->ModifyMenu(2, MF_BYPOSITION | MF_STRING, 2, cstr);
	cstr = i18n(_T("Menu"), _T("THEME"));
	menu->ModifyMenu(3, MF_BYPOSITION | MF_STRING, 3, cstr);
	cstr = i18n(_T("Menu"), _T("DISK"));
	menu->ModifyMenu(4, MF_BYPOSITION | MF_STRING, 4, cstr);
	cstr = i18n(_T("Menu"), _T("HELP"));
	menu->ModifyMenu(5, MF_BYPOSITION | MF_STRING, 5, cstr);
	cstr = i18n(_T("Menu"), _T("LANGUAGE"));

	if (cstr.Find(_T("Language")) >= 0)
	{
		cstr = _T("&Language");
		menu->ModifyMenu(6, MF_BYPOSITION | MF_STRING, 6, cstr);
	}
	else
	{
		menu->ModifyMenu(6, MF_BYPOSITION | MF_STRING, 6, cstr + _T("(&Language)"));
	}

	cstr = i18n(_T("Menu"), _T("CUSTOMIZE"));
	menuState = menu->GetMenuState(ID_CUSTOMIZE, MF_BYCOMMAND);
	menu->ModifyMenu(ID_CUSTOMIZE, MF_STRING, ID_CUSTOMIZE, cstr);
	menu->EnableMenuItem(ID_CUSTOMIZE, menuState);

	//////////////////////////////////////////////////////////////////////////////////
	// File
	//////////////////////////////////////////////////////////////////////////////////
	cstr = i18n(_T("Menu"), _T("SAVE_TEXT"));
	cstr += _T("\tCtrl + T");
	menu->ModifyMenu(ID_SAVE_TEXT, MF_STRING, ID_SAVE_TEXT, cstr);
	cstr = i18n(_T("Menu"), _T("SAVE_IMAGE"));
	cstr += _T("\tCtrl + S");
	menu->ModifyMenu(ID_SAVE_IMAGE, MF_STRING, ID_SAVE_IMAGE, cstr);
	cstr = i18n(_T("Menu"), _T("EXIT"));
	cstr += _T("\tAlt + F4");
	menu->ModifyMenu(ID_EXIT, MF_STRING, ID_EXIT, cstr);

	//////////////////////////////////////////////////////////////////////////////////
	// Edit
	//////////////////////////////////////////////////////////////////////////////////
	cstr = i18n(_T("Menu"), _T("COPY"));
	cstr += _T("\tCtrl + Shift + C");
	menu->ModifyMenu(ID_COPY, MF_STRING, ID_COPY, cstr);

	subMenu.Attach(menu->GetSubMenu(1)->GetSafeHmenu());
	cstr = i18n(_T("Menu"), _T("COPY_OPTION"));
	subMenu.ModifyMenu(1, MF_BYPOSITION, 1, cstr);
	subMenu.Detach();

	cstr = i18n(_T("Menu"), _T("ASCII_VIEW"));
	menu->ModifyMenu(ID_ASCII_VIEW, MF_STRING, ID_ASCII_VIEW, cstr);

	//////////////////////////////////////////////////////////////////////////////////
	// Function
	//////////////////////////////////////////////////////////////////////////////////
	cstr = i18n(_T("Menu"), _T("HIDE_SMART_INFO"));
	menu->ModifyMenu(ID_HIDE_SMART_INFO, MF_STRING, ID_HIDE_SMART_INFO, cstr);
	cstr = i18n(_T("Menu"), _T("HIDE_SERIAL_NUMBER"));
	menu->ModifyMenu(ID_HIDE_SERIAL_NUMBER, MF_STRING, ID_HIDE_SERIAL_NUMBER, cstr);
	cstr = i18n(_T("Menu"), _T("HIDE_NO_SMART_DISK"));
	menu->ModifyMenu(ID_HIDE_NO_SMART_DISK, MF_STRING, ID_HIDE_NO_SMART_DISK, cstr);
	cstr = i18n(_T("Menu"), _T("ADVANCED_DISK_SEARCH"));
	menu->ModifyMenu(ID_ADVANCED_DISK_SEARCH, MF_STRING, ID_ADVANCED_DISK_SEARCH, cstr);
	cstr = i18n(_T("Menu"), _T("EVENT_LOG"));
	menu->ModifyMenu(ID_EVENT_LOG, MF_STRING, ID_EVENT_LOG, cstr);

	cstr = i18n(_T("Menu"), _T("ALERT_SOUND"));
	menu->ModifyMenu(ID_ALERT_SOUND, MF_STRING, ID_ALERT_SOUND, cstr);
	cstr = i18n(_T("Menu"), _T("ALARM_HISTORY"));
	menu->ModifyMenu(ID_ALARM_HISTORY, MF_STRING, ID_ALARM_HISTORY, cstr);
	cstr = i18n(_T("Menu"), _T("SOUND_SETTINGS"));
	menu->ModifyMenu(ID_SOUND_SETTINGS, MF_STRING, ID_SOUND_SETTINGS, cstr);
	cstr = i18n(_T("Menu"), _T("GADGET_SUPPORT"));
	menu->ModifyMenu(ID_GADGET_SUPPORT, MF_STRING, ID_GADGET_SUPPORT, cstr);

	cstr = i18n(_T("Menu"), _T("DISABLE"));
	menu->ModifyMenu(ID_CSMI_DISABLE, MF_STRING, ID_CSMI_DISABLE, cstr);
	cstr = i18n(_T("Menu"), _T("ENABLE_AUTO"));
	menu->ModifyMenu(ID_CSMI_ENABLE_AUTO, MF_STRING, ID_CSMI_ENABLE_AUTO, cstr);
	cstr = i18n(_T("Menu"), _T("ENABLE_RAID"));
	menu->ModifyMenu(ID_CSMI_ENABLE_RAID, MF_STRING, ID_CSMI_ENABLE_RAID, cstr);
	cstr = i18n(_T("Menu"), _T("ENABLE_ALL"));
	menu->ModifyMenu(ID_CSMI_ENABLE_ALL, MF_STRING, ID_CSMI_ENABLE_ALL, cstr);
	cstr = i18n(_T("Menu"), _T("INSTALL_GADGET"));
	menu->ModifyMenu(ID_INSTALL_GADGET, MF_STRING, ID_INSTALL_GADGET, cstr);
	if (HasSidebar())
	{
		menu->EnableMenuItem(ID_INSTALL_GADGET, MF_ENABLED);
	}
	else
	{
		menu->EnableMenuItem(ID_INSTALL_GADGET, MF_GRAYED);
	}

	if (IsDotNet4() || IsDotNet2())
	{
		cstr = i18n(_T("Menu"), _T("ALERT_MAIL"));
		menu->ModifyMenu(ID_ALERT_MAIL, MF_STRING, ID_ALERT_MAIL, cstr);
		menu->EnableMenuItem(ID_ALERT_MAIL, MF_ENABLED);
		cstr = i18n(_T("Menu"), _T("MAIL_SETTINGS"));
		menu->ModifyMenu(ID_MAIL_SETTINGS, MF_STRING, ID_MAIL_SETTINGS, cstr);
		menu->EnableMenuItem(ID_MAIL_SETTINGS, MF_ENABLED);
	}
	else
	{
		cstr = i18n(_T("Menu"), _T("ALERT_MAIL")) + _T(" [.NET 2.0-]");
		menu->ModifyMenu(ID_ALERT_MAIL, MF_STRING, ID_ALERT_MAIL, cstr);
		menu->EnableMenuItem(ID_ALERT_MAIL, MF_GRAYED);
		cstr = i18n(_T("Menu"), _T("MAIL_SETTINGS")) + _T(" [.NET 2.0-]");
		menu->ModifyMenu(ID_MAIL_SETTINGS, MF_STRING, ID_MAIL_SETTINGS, cstr);
		menu->EnableMenuItem(ID_MAIL_SETTINGS, MF_GRAYED);
	}

	cstr = i18n(_T("Menu"), _T("RESIDENT"));
	menu->ModifyMenu(ID_RESIDENT, MF_STRING, ID_RESIDENT, cstr);
	cstr = i18n(_T("Menu"), _T("HEALTH_STATUS_SETTING"));
	menu->ModifyMenu(ID_HEALTH_STATUS, MF_STRING, ID_HEALTH_STATUS, cstr);
	cstr = i18n(_T("Menu"), _T("AAM_APM_CONTROL"));
	menuState = menu->GetMenuState(ID_AAM_APM, MF_BYCOMMAND);
	menu->ModifyMenu(ID_AAM_APM, MF_STRING, ID_AAM_APM, cstr);

	cstr = i18n(_T("Alarm"), _T("ALARM_TEMPERATURE"));
	menuState = menu->GetMenuState(ID_TEMPERATURE, MF_BYCOMMAND);
	menu->ModifyMenu(ID_TEMPERATURE, MF_STRING, ID_TEMPERATURE, cstr);

	/*
	if(m_Ata.vars.GetCount() && menuState != MF_GRAYED)
	{
	menu->EnableMenuItem(ID_AAM_APM, MF_ENABLED);
	menu->EnableMenuItem(ID_HEALTH_STATUS, MF_ENABLED);
	}
	else
	{
	menu->EnableMenuItem(ID_AAM_APM, MF_GRAYED);
	menu->EnableMenuItem(ID_HEALTH_STATUS, MF_GRAYED);
	}
	*/

	cstr = i18n(_T("Menu"), _T("AUTO_AAM_APM_ADAPTION"));
	menu->ModifyMenu(ID_AUTO_AAM_APM, MF_STRING, ID_AUTO_AAM_APM, cstr);
	cstr = i18n(_T("Menu"), _T("STARTUP"));
	menu->ModifyMenu(ID_STARTUP, MF_STRING, ID_STARTUP, cstr);

#ifdef UWP
	menu->EnableMenuItem(ID_STARTUP, MF_GRAYED);
#endif

	cstr = i18n(_T("Menu"), _T("GRAPH"));
	menu->ModifyMenu(ID_GRAPH, MF_STRING, ID_GRAPH, cstr);
	cstr = i18n(_T("Menu"), _T("REFRESH"));
	cstr += _T("\tF5");
	menu->ModifyMenu(ID_REFRESH, MF_STRING, ID_REFRESH, cstr);
	cstr = i18n(_T("Menu"), _T("RESCAN"));
	cstr += _T("\tF6");
	menu->ModifyMenu(ID_RESCAN, MF_STRING, ID_RESCAN, cstr);

	subMenu.Attach(menu->GetSubMenu(2)->GetSafeHmenu());
	cstr = i18n(_T("Menu"), _T("AUTO_REFRESH"));
	subMenu.ModifyMenu(1, MF_BYPOSITION, 1, cstr);
	cstr = i18n(_T("Menu"), _T("AUTO_REFRESH_TARGET"));
	subMenu.ModifyMenu(2, MF_BYPOSITION, 2, cstr);

	cstr = i18n(_T("Menu"), _T("ALERT_FEATURES"));
	subMenu.ModifyMenu(11, MF_BYPOSITION, 11, cstr);
	cstr = i18n(_T("Menu"), _T("ADVANCED_FEATURE"));
	subMenu.ModifyMenu(12, MF_BYPOSITION, 12, cstr);
	cstr = i18n(_T("Menu"), _T("WORKAROUND"));
	subMenu.ModifyMenu(13, MF_BYPOSITION, 13, cstr);
	subMenu.Detach();

	cstr = i18n(_T("Menu"), _T("OPEN_DISK_MANAGEMENT"));
	menu->ModifyMenu(ID_OPEN_DISK_MANAGEMENT, MF_STRING, ID_OPEN_DISK_MANAGEMENT, cstr);
	cstr = i18n(_T("Menu"), _T("OPEN_DEVICE_MANAGER"));
	menu->ModifyMenu(ID_OPEN_DEVICE_MANAGER, MF_STRING, ID_OPEN_DEVICE_MANAGER, cstr);

	cstr = i18n(_T("TrayMenu"), _T("DISABLE"));
	menu->ModifyMenu(ID_AUTO_REFRESH_DISABLE, MF_STRING, ID_AUTO_REFRESH_DISABLE, cstr);
	cstr = i18n(_T("Menu"), _T("MINUTE"));
	menu->ModifyMenu(ID_AUTO_REFRESH_01_MIN, MF_STRING, ID_AUTO_REFRESH_01_MIN, _T("1 ") + cstr);
	menu->ModifyMenu(ID_AUTO_REFRESH_03_MIN, MF_STRING, ID_AUTO_REFRESH_03_MIN, _T("3 ") + cstr);
	menu->ModifyMenu(ID_AUTO_REFRESH_05_MIN, MF_STRING, ID_AUTO_REFRESH_05_MIN, _T("5 ") + cstr);
	menu->ModifyMenu(ID_AUTO_REFRESH_10_MIN, MF_STRING, ID_AUTO_REFRESH_10_MIN, _T("10 ") + cstr);
	menu->ModifyMenu(ID_AUTO_REFRESH_30_MIN, MF_STRING, ID_AUTO_REFRESH_30_MIN, _T("30 ") + cstr);
	menu->ModifyMenu(ID_AUTO_REFRESH_60_MIN, MF_STRING, ID_AUTO_REFRESH_60_MIN, _T("60 ") + cstr);
	menu->ModifyMenu(ID_AUTO_REFRESH_120_MIN, MF_STRING, ID_AUTO_REFRESH_120_MIN, _T("120 ") + cstr);
	menu->ModifyMenu(ID_AUTO_REFRESH_180_MIN, MF_STRING, ID_AUTO_REFRESH_180_MIN, _T("180 ") + cstr);
	menu->ModifyMenu(ID_AUTO_REFRESH_360_MIN, MF_STRING, ID_AUTO_REFRESH_360_MIN, _T("360 ") + cstr);
	menu->ModifyMenu(ID_AUTO_REFRESH_720_MIN, MF_STRING, ID_AUTO_REFRESH_720_MIN, _T("720 ") + cstr);
	menu->ModifyMenu(ID_AUTO_REFRESH_1440_MIN, MF_STRING, ID_AUTO_REFRESH_1440_MIN, _T("1440 ") + cstr);

	CheckRadioAutoRefresh();

	cstr = i18n(_T("Menu"), _T("SECOND"));
	menu->ModifyMenu(ID_WAIT_0_SEC, MF_STRING, ID_WAIT_0_SEC, _T("0 ") + cstr);
	menu->ModifyMenu(ID_WAIT_5_SEC, MF_STRING, ID_WAIT_5_SEC, _T("5 ") + cstr);
	menu->ModifyMenu(ID_WAIT_10_SEC, MF_STRING, ID_WAIT_10_SEC, _T("10 ") + cstr);
	menu->ModifyMenu(ID_WAIT_15_SEC, MF_STRING, ID_WAIT_15_SEC, _T("15 ") + cstr);
	menu->ModifyMenu(ID_WAIT_20_SEC, MF_STRING, ID_WAIT_20_SEC, _T("20 ") + cstr);
	menu->ModifyMenu(ID_WAIT_30_SEC, MF_STRING, ID_WAIT_30_SEC, _T("30 ") + cstr);
	menu->ModifyMenu(ID_WAIT_40_SEC, MF_STRING, ID_WAIT_40_SEC, _T("40 ") + cstr);
	menu->ModifyMenu(ID_WAIT_50_SEC, MF_STRING, ID_WAIT_50_SEC, _T("50 ") + cstr);
	menu->ModifyMenu(ID_WAIT_60_SEC, MF_STRING, ID_WAIT_60_SEC, _T("60 ") + cstr);
	menu->ModifyMenu(ID_WAIT_90_SEC, MF_STRING, ID_WAIT_90_SEC, _T("90 ") + cstr);
	menu->ModifyMenu(ID_WAIT_120_SEC, MF_STRING, ID_WAIT_120_SEC, _T("120 ") + cstr);
	menu->ModifyMenu(ID_WAIT_150_SEC, MF_STRING, ID_WAIT_150_SEC, _T("150 ") + cstr);
	menu->ModifyMenu(ID_WAIT_180_SEC, MF_STRING, ID_WAIT_180_SEC, _T("180 ") + cstr);
	menu->ModifyMenu(ID_WAIT_210_SEC, MF_STRING, ID_WAIT_210_SEC, _T("210 ") + cstr);
	menu->ModifyMenu(ID_WAIT_240_SEC, MF_STRING, ID_WAIT_240_SEC, _T("240 ") + cstr);

	CheckRadioWaitTime();

	// Advanced Features
	subMenu.Attach(menu->GetSubMenu(2)->GetSubMenu(12)->GetSafeHmenu());
	cstr = i18n(_T("Menu"), _T("TEMPERATURE_TYPE"));
	subMenu.ModifyMenu(5, MF_BYPOSITION, 5, cstr);
	cstr = i18n(_T("Menu"), _T("AUTO_DTECTION"));
	subMenu.ModifyMenu(6, MF_BYPOSITION, 6, cstr);
	cstr = i18n(_T("Dialog"), _T("LIST_RAW_VALUES"));
	subMenu.ModifyMenu(7, MF_BYPOSITION, 7, cstr);
	cstr = i18n(_T("Menu"), _T("WAIT_TIME_AT_STARTUP"));
	subMenu.ModifyMenu(8, MF_BYPOSITION, 8, cstr);
	cstr = i18n(_T("Menu"), _T("RESIDENT_STYLE"));
	subMenu.ModifyMenu(9, MF_BYPOSITION, 9, cstr);

#ifdef UWP
	subMenu.EnableMenuItem(7, MF_GRAYED);
#endif
	subMenu.Detach();

	cstr = i18n(_T("Menu"), _T("SECOND"));
	menu->ModifyMenu(ID_AUTO_DETECTION_05_SEC, MF_STRING, ID_AUTO_DETECTION_05_SEC, _T("5 ") + cstr);
	menu->ModifyMenu(ID_AUTO_DETECTION_10_SEC, MF_STRING, ID_AUTO_DETECTION_10_SEC, _T("10 ") + cstr);
	menu->ModifyMenu(ID_AUTO_DETECTION_20_SEC, MF_STRING, ID_AUTO_DETECTION_20_SEC, _T("20 ") + cstr);
	menu->ModifyMenu(ID_AUTO_DETECTION_30_SEC, MF_STRING, ID_AUTO_DETECTION_30_SEC, _T("30 ") + cstr);
	cstr = i18n(_T("TrayMenu"), _T("DISABLE"));
	menu->ModifyMenu(ID_AUTO_DETECTION_DISABLE, MF_STRING, ID_AUTO_DETECTION_DISABLE, cstr);

	CheckRadioAutoDetection();
	CheckRadioRawValues();
	CheckRadioCsmiType();

	cstr = i18n(_T("Menu"), _T("CELSIUS"));
	menu->ModifyMenu(ID_CELSIUS, MF_STRING, ID_CELSIUS, cstr);
	cstr = i18n(_T("Menu"), _T("FAHRENHEIT"));
	menu->ModifyMenu(ID_FAHRENHEIT, MF_STRING, ID_FAHRENHEIT, cstr);

	if (m_bFahrenheit)
	{
		OnFahrenheit();
	}
	else
	{
		OnCelsius();
	}

	cstr = i18n(_T("Menu"), _T("HIDE"));
	menu->ModifyMenu(ID_RESIDENT_HIDE, MF_STRING, ID_RESIDENT_HIDE, cstr);
	cstr = i18n(_T("Menu"), _T("MINIMIZE"));
	menu->ModifyMenu(ID_RESIDENT_MINIMIZE, MF_STRING, ID_RESIDENT_MINIMIZE, cstr);

	if (m_bResidentMinimize)
	{
		OnResidentMinimize();
	}
	else
	{
		OnResidentHide();
	}

	cstr = i18n(_T("Menu"), _T("HELP_WEB")) + _T("\tF1");
	menu->ModifyMenu(ID_HELP, MF_STRING, ID_HELP, cstr);
	cstr = i18n(_T("Menu"), _T("HELP_ABOUT"));
	menuState = menu->GetMenuState(ID_ABOUT, MF_BYCOMMAND);
	menu->ModifyMenu(ID_ABOUT, MF_STRING, ID_ABOUT, cstr);
	menu->EnableMenuItem(ID_ABOUT, menuState);

	cstr = i18n(_T("Menu"), _T("HELP_ABOUT_SMART"));
	menu->ModifyMenu(ID_HELP_ABOUT_SMART, MF_STRING, ID_HELP_ABOUT_SMART, cstr);
	cstr = i18n(_T("TrayMenu"), _T("ENABLE_ALL"));
	menu->ModifyMenu(ID_USB_ENABLE_ALL, MF_STRING, ID_USB_ENABLE_ALL, cstr);
	cstr = i18n(_T("TrayMenu"), _T("DISABLE_ALL"));
	menu->ModifyMenu(ID_USB_DISABLE_ALL, MF_STRING, ID_USB_DISABLE_ALL, cstr);
	cstr = i18n(_T("Menu"), _T("SMART_IN_ENGLISH"));
	menu->ModifyMenu(ID_SMART_ENGLISH, MF_STRING, ID_SMART_ENGLISH, cstr);
	cstr = i18n(_T("Menu"), _T("FONT_SETTING"));
	menu->ModifyMenu(ID_FONT_SETTING, MF_STRING, ID_FONT_SETTING, cstr);


	// Check Status

	if (m_bDumpIdentifyDevice)
	{
		menu->CheckMenuItem(ID_DUMP_IDENTIFY_DEVICE, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_DUMP_IDENTIFY_DEVICE, MF_UNCHECKED);
	}

	if (m_bDumpSmartReadData)
	{
		menu->CheckMenuItem(ID_DUMP_SMART_READ_DATA, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_DUMP_SMART_READ_DATA, MF_UNCHECKED);
	}

	if (m_bDumpSmartReadThreshold)
	{
		menu->CheckMenuItem(ID_DUMP_SMART_READ_THRESHOLD, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_DUMP_SMART_READ_THRESHOLD, MF_UNCHECKED);
	}

	if (m_bAsciiView)
	{
		menu->CheckMenuItem(ID_ASCII_VIEW, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_ASCII_VIEW, MF_UNCHECKED);
	}

	if (m_bSmartEnglish)
	{
		menu->CheckMenuItem(ID_SMART_ENGLISH, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_SMART_ENGLISH, MF_UNCHECKED);
	}

	if (m_bHideSerialNumber)
	{
		menu->CheckMenuItem(ID_HIDE_SERIAL_NUMBER, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_HIDE_SERIAL_NUMBER, MF_UNCHECKED);
	}

	if (m_bHideSmartInfo)
	{
		menu->CheckMenuItem(ID_HIDE_SMART_INFO, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_HIDE_SMART_INFO, MF_UNCHECKED);
	}

	if (m_bHideNoSmartDisk)
	{
		menu->CheckMenuItem(ID_HIDE_NO_SMART_DISK, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_HIDE_NO_SMART_DISK, MF_UNCHECKED);
	}

	if (m_bAdvancedDiskSearch)
	{
		menu->CheckMenuItem(ID_ADVANCED_DISK_SEARCH, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_ADVANCED_DISK_SEARCH, MF_UNCHECKED);
	}

	if (m_bWorkaroundHD204UI)
	{
		menu->CheckMenuItem(ID_WORKAROUND_HD204UI, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_WORKAROUND_HD204UI, MF_UNCHECKED);
	}

	if (m_bWorkaroundIE8MODE)
	{
		menu->CheckMenuItem(ID_WORKAROUND_IE8MODE, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_WORKAROUND_IE8MODE, MF_UNCHECKED);
	}

	if (m_bWorkaroundIgnoreC4)
	{
		menu->CheckMenuItem(ID_WORKAROUND_IGNORE_C4, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_WORKAROUND_IGNORE_C4, MF_UNCHECKED);
	}

	if (GetIeVersion() >= 900)
	{
		menu->EnableMenuItem(ID_WORKAROUND_IE8MODE, MF_ENABLED);
	}
	else
	{
		menu->EnableMenuItem(ID_WORKAROUND_IE8MODE, MF_GRAYED);
	}

	if (GetIeVersion() >= 600)
	{
		menu->EnableMenuItem(ID_GRAPH, MF_ENABLED);
	}
	else
	{
		menu->EnableMenuItem(ID_GRAPH, MF_GRAYED);
	}

	if (m_bEventLog)
	{
		menu->CheckMenuItem(ID_EVENT_LOG, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_EVENT_LOG, MF_UNCHECKED);
	}

	if (m_bAlertMail)
	{
		menu->CheckMenuItem(ID_ALERT_MAIL, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_ALERT_MAIL, MF_UNCHECKED);
	}

	if (m_bAtaPassThroughSmart)
	{
		menu->CheckMenuItem(ID_ATA_PASS_THROUGH_SMART, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_ATA_PASS_THROUGH_SMART, MF_UNCHECKED);
	}

	if (m_bGadget)
	{
		menu->CheckMenuItem(ID_GADGET_SUPPORT, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_GADGET_SUPPORT, MF_UNCHECKED);
	}

	if (m_bAutoAamApm)
	{
		menu->CheckMenuItem(ID_AUTO_AAM_APM, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_AUTO_AAM_APM, MF_UNCHECKED);
	}

	if (m_bResident)
	{
		menu->CheckMenuItem(ID_RESIDENT, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_RESIDENT, MF_UNCHECKED);
	}

	if (m_bStartup)
	{
		menu->CheckMenuItem(ID_STARTUP, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_STARTUP, MF_UNCHECKED);
	}

	if (m_bAlertSound)
	{
		menu->CheckMenuItem(ID_ALERT_SOUND, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_ALERT_SOUND, MF_UNCHECKED);
	}

	//	menu->EnableMenuItem(ID_ALARM_HISTORY, MF_GRAYED);
	//	menu->EnableMenuItem(ID_ALERT_SOUND, MF_GRAYED);
	//	menu->EnableMenuItem(ID_SOUND_SETTINGS, MF_GRAYED);

	//////////////////////////////////////////////////////////////////////////////////
	// Theme
	//////////////////////////////////////////////////////////////////////////////////
	subMenu.Attach(menu->GetSubMenu(3)->GetSafeHmenu());
	cstr = i18n(_T("Menu"), _T("ZOOM"));
	subMenu.ModifyMenu(0, MF_BYPOSITION, 0, cstr);
	subMenu.Detach();

	cstr = i18n(_T("Menu"), _T("AUTO"));
	menu->ModifyMenu(ID_ZOOM_AUTO, MF_STRING, ID_ZOOM_AUTO, cstr);

	cstr = i18n(_T("Menu"), _T("GREEN_MODE"));
	menu->ModifyMenu(ID_GREEN_MODE, MF_STRING, ID_GREEN_MODE, cstr);

	CheckRadioZoomType();

	if (m_bGreenMode)
	{
		menu->CheckMenuItem(ID_GREEN_MODE, MF_CHECKED);
	}
	else
	{
		menu->CheckMenuItem(ID_GREEN_MODE, MF_UNCHECKED);
	}

	//////////////////////////////////////////////////////////////////////////////////
	// Disk
	//////////////////////////////////////////////////////////////////////////////////
	subMenu.Attach(menu->GetSubMenu(MENU_DRIVE_INDEX)->GetSafeHmenu());
	while (subMenu.RemoveMenu(0, MF_BYPOSITION));

	MENUITEMINFO subMenuInfo;
	ZeroMemory(&subMenuInfo, sizeof(MENUITEMINFO));
	subMenuInfo.cbSize = sizeof(MENUITEMINFO);
	subMenuInfo.fMask = MIIM_CHECKMARKS | MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU;
	subMenuInfo.fType = MFT_RADIOCHECK;
	subMenuInfo.hbmpChecked = NULL;
	subMenuInfo.fState = MFS_UNCHECKED;
	subMenuInfo.hSubMenu = NULL;

	for (int i = 0; i < m_Ata.vars.GetCount(); i++)
	{
		CString cstr;
		if (m_Ata.vars[i].TotalDiskSize >= 1000)
		{
			cstr.Format(_T("(%d) %s %.1f GB"), i + 1, m_Ata.vars[i].Model, m_Ata.vars[i].TotalDiskSize / 1000.0);
		}
		else
		{
			cstr.Format(_T("(%d) %s %d MB"), i + 1, m_Ata.vars[i].Model, m_Ata.vars[i].TotalDiskSize);
		}
		subMenuInfo.wID = SELECT_DISK_BASE + i;
		subMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
		subMenu.InsertMenuItem(-1, &subMenuInfo);

		if (i % 8 == 7 && i + 1 != m_Ata.vars.GetCount())
		{
			subMenu.AppendMenu(MF_SEPARATOR);
		}
	}
	subMenu.CheckMenuRadioItem(SELECT_DISK_BASE, SELECT_DISK_BASE + (INT)m_Ata.vars.GetCount(),
		SELECT_DISK_BASE + m_SelectDisk, MF_BYCOMMAND);
	subMenu.Detach();

	// Auto Refresh Target
	subSubMenu.Attach(menu->GetSubMenu(2)->GetSubMenu(2)->GetSafeHmenu());
	while (subSubMenu.RemoveMenu(0, MF_BYPOSITION));

	MENUITEMINFO subSubMenuInfo;
	ZeroMemory(&subSubMenuInfo, sizeof(MENUITEMINFO));
	subSubMenuInfo.cbSize = sizeof(MENUITEMINFO);
	subSubMenuInfo.fMask = MIIM_CHECKMARKS | MIIM_TYPE | MIIM_STATE | MIIM_ID;
	subSubMenuInfo.fType = NULL;
	subSubMenuInfo.hbmpChecked = NULL;

	cstr = i18n(_T("Menu"), _T("AUTO_REFRESH_TARGET_ALL_DISK"));
	subSubMenuInfo.wID = AUTO_REFRESH_TARGET_BASE + CAtaSmart::MAX_DISK;
	subSubMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	subSubMenu.InsertMenuItem(-1, &subSubMenuInfo);

	cstr = i18n(_T("Menu"), _T("AUTO_REFRESH_UNTARGET_ALL_DISK"));
	subSubMenuInfo.wID = AUTO_REFRESH_TARGET_BASE + CAtaSmart::MAX_DISK + 1;
	subSubMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
	subSubMenu.InsertMenuItem(-1, &subSubMenuInfo);

	subSubMenu.AppendMenu(MF_SEPARATOR);
	for (int i = 0; i < m_Ata.vars.GetCount(); i++)
	{
		if (m_Ata.vars[i].TotalDiskSize >= 1000)
		{
			cstr.Format(_T("(%d) %s %.1f GB"), i + 1, m_Ata.vars[i].Model, m_Ata.vars[i].TotalDiskSize / 1000.0);
		}
		else
		{
			cstr.Format(_T("(%d) %s %d MB"), i + 1, m_Ata.vars[i].Model, m_Ata.vars[i].TotalDiskSize);
		}
		subSubMenuInfo.wID = AUTO_REFRESH_TARGET_BASE + i;
		subSubMenuInfo.dwTypeData = (LPWSTR)cstr.GetString();
		if (m_bAutoRefreshTarget[i])
		{
			subSubMenuInfo.fState = MFS_CHECKED;
		}
		else
		{
			subSubMenuInfo.fState = MFS_UNCHECKED;
		}

		subSubMenu.InsertMenuItem(-1, &subSubMenuInfo);

		if (i % 8 == 7 && i + 1 != m_Ata.vars.GetCount())
		{
			subSubMenu.AppendMenu(MF_SEPARATOR);
		}
	}
	subSubMenu.Detach();

	SetMenu(menu);
	DrawMenuBar();

	//////////////////////////////////////////////////////////////////////////////////
	// Main Window
	//////////////////////////////////////////////////////////////////////////////////
	SetLabel(m_CtrlLabelFirmware, m_LabelFirmware, i18n(_T("Dialog"), _T("FIRMWARE")));
	SetLabel(m_CtrlLabelSerialNumber, m_LabelSerialNumber, i18n(_T("Dialog"), _T("SERIAL_NUMBER")));
	SetLabel(m_CtrlLabelTemperature, m_LabelTemperature, i18n(_T("Dialog"), _T("TEMPERATURE")));
	SetLabel(m_CtrlLabelPowerOnHours, m_LabelPowerOnHours, i18n(_T("Dialog"), _T("POWER_ON_HOURS")));
	SetLabel(m_CtrlLabelPowerOnCount, m_LabelPowerOnCount, i18n(_T("Dialog"), _T("POWER_ON_COUNT")));
	SetLabel(m_CtrlLabelWAF, m_LabelWAF, i18n(_T("Dialog"), _T("WAF")));
	SetLabel(m_CtrlLabelFeature, m_LabelFeature, i18n(_T("Dialog"), _T("FEATURE")));
	SetLabel(m_CtrlLabelDriveMap, m_LabelDriveMap, i18n(_T("Dialog"), _T("DRIVE_LETTER")));
	SetLabel(m_CtrlLabelInterface, m_LabelInterface, i18n(_T("Dialog"), _T("INTERFACE")));
	SetLabel(m_CtrlLabelTransferMode, m_LabelTransferMode, i18n(_T("Dialog"), _T("TRANSFER_MODE")));
	SetLabel(m_CtrlLabelAtaAtapi, m_LabelAtaAtapi, i18n(_T("Dialog"), _T("STANDARD")));
	SetLabel(m_CtrlLabelDiskStatus, m_LabelDiskStatus, i18n(_T("Dialog"), _T("HEALTH_STATUS")));
	SetLabel(m_CtrlLabelBufferSize, m_LabelBufferSize, i18n(_T("Dialog"), _T("BUFFER_SIZE")));
	SetLabel(m_CtrlLabelNvCacheSize, m_LabelNvCacheSize, i18n(_T("Dialog"), _T("NV_CACHE_SIZE")));
	SetLabel(m_CtrlLabelRotationRate, m_LabelRotationRate, i18n(_T("Dialog"), _T("ROTATION_RATE")));

	if (m_Ata.vars.GetCount() > 0)
	{
		if (m_Ata.vars[m_SelectDisk].HostReads >= 0)
		{
			SetLabel(m_CtrlLabelBufferSize, m_LabelBufferSize, i18n(_T("Dialog"), _T("TOTAL_HOST_READS")));
		}

		if (m_Ata.vars[m_SelectDisk].HostWrites >= 0)
		{
			SetLabel(m_CtrlLabelNvCacheSize, m_LabelNvCacheSize, i18n(_T("Dialog"), _T("TOTAL_HOST_WRITES")));
		}

		if (m_Ata.vars[m_SelectDisk].NandWrites >= 0)
		{
			SetLabel(m_CtrlLabelRotationRate, m_LabelRotationRate, i18n(_T("Dialog"), _T("TOTAL_NAND_WRITES")));
		}
		else if (m_Ata.vars[m_SelectDisk].GBytesErased >= 0)
		{
			SetLabel(m_CtrlLabelRotationRate, m_LabelRotationRate, i18n(_T("SmartSandForce"), _T("64")));
		}
		/*
		else if(m_Ata.vars[m_SelectDisk].WearLevelingCount >= 0)
		{
		//	m_LabelRotationRate = i18n(_T("Dialog"), _T("WEAR_LEVELING_COUNT"));
		SetLabel(m_LabelRotationRate, _T("LabelRotationRate"), i18n(_T("Dialog"), _T("WEAR_LEVELING_COUNT")));
		}
		*/
		if (m_Ata.vars[m_SelectDisk].HostWrites >= 0 && m_Ata.vars[m_SelectDisk].NandWrites >= 0)
		{
			SetLabel(m_CtrlLabelWAF, m_LabelWAF, i18n(_T("Dialog"), _T("WAF")));
		}

	}

	UpdateData(FALSE);

	RebuildListHeader(m_SelectDisk, TRUE);
	ChangeDisk(m_SelectDisk);
	InitDriveList();
	UpdateToolTip();
	InitListCtrl();
	UpdateListCtrl(m_SelectDisk);

	if (m_bResident)
	{
		UpdateTrayTemperatureIcon(TRUE);
	}

	SetClientSize(m_SizeX, m_SizeY, m_ZoomRatio);

	WritePrivateProfileString(_T("Setting"), _T("Language"), LangName, m_Ini);
}

void CDiskInfoDlg::OnDiskStatus() { OnHealthStatus(); }
void CDiskInfoDlg::OnPreDisk() { SelectDrive(m_SelectDisk - 1); }
void CDiskInfoDlg::OnNextDisk() { SelectDrive(m_SelectDisk + 1); }

void CDiskInfoDlg::OnDisk0() { SelectDrive(0 + m_DriveMenuPage * 8); }
void CDiskInfoDlg::OnDisk1() { SelectDrive(1 + m_DriveMenuPage * 8); }
void CDiskInfoDlg::OnDisk2() { SelectDrive(2 + m_DriveMenuPage * 8); }
void CDiskInfoDlg::OnDisk3() { SelectDrive(3 + m_DriveMenuPage * 8); }
void CDiskInfoDlg::OnDisk4() { SelectDrive(4 + m_DriveMenuPage * 8); }
void CDiskInfoDlg::OnDisk5() { SelectDrive(5 + m_DriveMenuPage * 8); }
void CDiskInfoDlg::OnDisk6() { SelectDrive(6 + m_DriveMenuPage * 8); }
void CDiskInfoDlg::OnDisk7() { SelectDrive(7 + m_DriveMenuPage * 8); }

int CDiskInfoDlg::GetSelectedDrive()
{
	return m_SelectDisk;
}

void CDiskInfoDlg::SelectDrive(DWORD i)
{
	if (i >= (DWORD)m_Ata.vars.GetCount())
	{
		return;
	}

	CWaitCursor wait;
	static int preFlagFahrenheit = -1;

	switch (m_Ata.UpdateSmartInfo(i))
	{
	case CAtaSmart::SMART_STATUS_MAJOR_CHANGE:
	case CAtaSmart::SMART_STATUS_MINOR_CHANGE:
		SaveSmartInfo(i);
		AlertSound(0, AS_PLAY_SOUND);
		UpdateShareInfo();
		break;
	default:
		if (m_SelectDisk == i)
		{
			if (preFlagFahrenheit == m_bFahrenheit)
			{
				//	return ;
			}
			else
			{
				preFlagFahrenheit = m_bFahrenheit;
			}
		}
		break;
	}

	m_SelectDisk = i;
	m_DriveMenuPage = i / 8;
	ChangeDisk(i);
	UpdateListCtrl(i);
	InitDriveList();
	UpdateToolTip();

	CMenu *menu = GetMenu();
	menu->CheckMenuRadioItem(SELECT_DISK_BASE, SELECT_DISK_BASE + (INT)m_Ata.vars.GetCount(),
		SELECT_DISK_BASE + m_SelectDisk, MF_BYCOMMAND);
	SetMenu(menu);
	DrawMenuBar();
	CheckPage();

	if (m_bResident)
	{
		UpdateTrayTemperatureIcon(FALSE);
	}
}

void CDiskInfoDlg::CheckPage()
{
	CString cstr;
	if (m_Ata.vars.GetCount() > 1)
	{
		if (0 < m_SelectDisk)
		{
			m_CtrlButtonPreDisk.ShowWindow(SW_SHOW);
		}
		else
		{
			m_CtrlButtonPreDisk.ShowWindow(SW_HIDE);
		}

		if (m_SelectDisk < (DWORD)m_Ata.vars.GetCount() - 1)
		{
			m_CtrlButtonNextDisk.ShowWindow(SW_SHOW);
		}
		else
		{
			m_CtrlButtonNextDisk.ShowWindow(SW_HIDE);
		}
	}
	else
	{
		m_CtrlButtonPreDisk.ShowWindow(SW_HIDE);
		m_CtrlButtonNextDisk.ShowWindow(SW_HIDE);
	}
}

void CDiskInfoDlg::SaveSmartInfo(DWORD i)
{
	if (!m_Ata.vars[i].IsSmartCorrect)
	{
		return;
	}

	static CTime preTime[CAtaSmart::MAX_DISK] = { 0 };
	CTime time = CTime::GetTickCount();

	if (time - preTime[i] < SAVE_SMART_PERIOD)
	{
		return;
	}
	else
	{
		preTime[i] = time;
	}

	CString line;
	CString cstr;
	CStdioFile outFile;
	CString dir;
	CString disk;
	CString path;
	BOOL flagFirst = FALSE;
	TCHAR str[256];

	dir = m_SmartDir;
	CreateDirectory(dir, NULL);

	disk = m_Ata.vars[i].ModelSerial;
	dir += disk;
	CreateDirectory(dir, NULL);

	AlarmHealthStatus(i, dir, disk);


	// Computer Name
	// 
	DWORD length = 256;
	GetComputerNameW(str, &length);
	WritePrivateProfileString(L"PC", L"ComputerName", str, dir + _T("\\") + SMART_INI);

	GetPrivateProfileString(disk, _T("Date"), _T(""), str, 256, dir + _T("\\") + SMART_INI);
	cstr = str;
	if (cstr.IsEmpty())
	{
		flagFirst = TRUE;
		_stprintf_s(str, 256, _T("%s"), time.Format(_T("%Y/%m/%d %H:%M:%S")).GetBuffer());
		WritePrivateProfileString(disk + _T("FIRST"), _T("Date"), str, dir + _T("\\") + SMART_INI);

		_stprintf_s(str, 256, _T("%d"), m_Ata.vars[i].DiskStatus);
		WritePrivateProfileString(disk + _T("FIRST"), _T("HealthStatus"), str, dir + _T("\\") + SMART_INI);
	}

	// Check Threshold of Reallocated Sectors Count
	GetPrivateProfileString(disk + _T("THRESHOLD"), _T("05"), _T(""), str, 256, dir + _T("\\") + SMART_INI);
	cstr = str;
	if (cstr.IsEmpty())
	{
		flagFirst = TRUE;
	}

	_stprintf_s(str, 256, _T("%s"), time.Format(_T("%Y/%m/%d %H:%M:%S")).GetBuffer());
	WritePrivateProfileString(disk, _T("Date"), str, dir + _T("\\") + SMART_INI);

	_stprintf_s(str, 256, _T("%d"), m_Ata.vars[i].DiskStatus);
	WritePrivateProfileString(disk, _T("HealthStatus"), str, dir + _T("\\") + SMART_INI);

	if (m_Ata.vars[i].Temperature > -300)
	{
		AppendLog(dir, disk, _T("Temperature"), time, m_Ata.vars[i].Temperature, flagFirst);
	}

	if (m_Ata.vars[i].MeasuredPowerOnHours > 0 && m_NowDetectingUnitPowerOnHours == FALSE)
	{
		AppendLog(dir, disk, _T("PowerOnHours"), time, m_Ata.vars[i].MeasuredPowerOnHours, flagFirst);
	}

	if (m_Ata.vars[i].PowerOnCount >= 0)
	{
		AppendLog(dir, disk, _T("PowerOnCount"), time, m_Ata.vars[i].PowerOnCount, flagFirst);
	}

	if (m_Ata.vars[i].Life >= 0)
	{
		AppendLog(dir, disk, _T("Life"), time, m_Ata.vars[i].Life, flagFirst);
	}

	if (m_Ata.vars[i].HostWrites >= 0)
	{
		AppendLog(dir, disk, _T("HostWrites"), time, m_Ata.vars[i].HostWrites, flagFirst);
	}

	if (m_Ata.vars[i].HostReads >= 0)
	{
		AppendLog(dir, disk, _T("HostReads"), time, m_Ata.vars[i].HostReads, flagFirst);
	}

	if (m_Ata.vars[i].NandWrites >= 0)
	{
		AppendLog(dir, disk, _T("NandWrites"), time, m_Ata.vars[i].NandWrites, flagFirst);
	}

	if (m_Ata.vars[i].GBytesErased >= 0)
	{
		AppendLog(dir, disk, _T("GBytesErased"), time, m_Ata.vars[i].GBytesErased, flagFirst);
	}

	if (m_Ata.vars[i].HostWrites > 1 && m_Ata.vars[i].NandWrites > 0)
	{
		double d_waf = static_cast<double>(m_Ata.vars[i].NandWrites) / static_cast<double>(m_Ata.vars[i].HostWrites);
		AppendLog_WAF(dir, disk, _T("WAF"), time, d_waf, flagFirst);
	}

	if (m_Ata.vars[i].WearLevelingCount >= 0)
	{
		AppendLog(dir, disk, _T("WearLevelingCount"), time, m_Ata.vars[i].WearLevelingCount, flagFirst);
	}

	for (DWORD j = 0; j < m_Ata.vars[i].AttributeCount; j++)
	{
		cstr.Format(_T("%02X"), m_Ata.vars[i].Attribute[j].Id);
		AppendLog(dir, disk, cstr, time, m_Ata.vars[i].Attribute[j].CurrentValue,
			flagFirst, m_Ata.vars[i].Threshold[j].ThresholdValue);

		switch (m_Ata.vars[i].Attribute[j].Id)
		{
		case 0x05: // Reallocated Sectors Count
			AppendLog(dir, disk, _T("ReallocatedSectorsCount"), time,
				MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[0], m_Ata.vars[i].Attribute[j].RawValue[1]), flagFirst);
			break;
		case 0xC4: // Reallocation Event Count
			AppendLog(dir, disk, _T("ReallocationEventCount"), time,
				MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[0], m_Ata.vars[i].Attribute[j].RawValue[1]), flagFirst);
			break;
		case 0xC5: // Current Pending Sector Count
			AppendLog(dir, disk, _T("CurrentPendingSectorCount"), time,
				MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[0], m_Ata.vars[i].Attribute[j].RawValue[1]), flagFirst);
			break;
		case 0xC6: // Off-Line Scan Uncorrectable Sector Count
			AppendLog(dir, disk, _T("UncorrectableSectorCount"), time,
				MAKEWORD(m_Ata.vars[i].Attribute[j].RawValue[0], m_Ata.vars[i].Attribute[j].RawValue[1]), flagFirst);
			break;
		default:
			break;
		}
	}
}

BOOL CDiskInfoDlg::AppendLog(CString dir, CString disk, CString file, CTime time, int value, BOOL flagFirst, int threshold)
{
	TCHAR str[256];

	// First Time
	if (flagFirst)
	{
		wsprintf(str, _T("%d"), value);
		WritePrivateProfileString(disk + _T("FIRST"), file, str, dir + _T("\\") + SMART_INI);

		if (file.GetLength() == 2)
		{
			wsprintf(str, _T("%d"), threshold);
			WritePrivateProfileString(disk + _T("THRESHOLD"), file, str, dir + _T("\\") + SMART_INI);
		}
	}

	GetPrivateProfileString(disk, file, _T("-1"), str, 256, dir + _T("\\") + SMART_INI);
	int pre = _tstoi(str);

	if (pre != value)
	{
		// Update
		wsprintf(str, _T("%d"), value);
		WritePrivateProfileString(disk, file, str, dir + _T("\\") + SMART_INI);

		CString line;
		line.Format(_T("%s,%d\n"), time.Format(_T("%Y/%m/%d %H:%M:%S")), value);

		CStdioFile outFile;
		if (outFile.Open(dir + _T("\\") + file + _T(".csv"),
			CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::typeText))
		{
			ULONGLONG fileLength = outFile.GetLength();
			try
			{
				if (outFile.SeekToEnd() == fileLength)
				{
					outFile.WriteString(line);
				}
			}
			catch (CFileException * e)
			{
				DebugPrint(L"CFileException");
				e->Delete();
			}
			outFile.Close();
			return TRUE;
		}
	}
	return FALSE;
}
BOOL CDiskInfoDlg::AppendLog_WAF(CString dir, CString disk, CString file, CTime time, double value, BOOL flagFirst, int threshold)
{
	TCHAR str[256];

	// First Time
	if (flagFirst)
	{
		wsprintf(str, _T("%f"), value);
		WritePrivateProfileString(disk + _T("FIRST"), file, str, dir + _T("\\") + SMART_INI);

		if (file.GetLength() == 2)
		{
			wsprintf(str, _T("%f"), threshold);
			WritePrivateProfileString(disk + _T("THRESHOLD"), file, str, dir + _T("\\") + SMART_INI);
		}
	}

	GetPrivateProfileString(disk, file, _T("-1"), str, 256, dir + _T("\\") + SMART_INI);
	int pre = _tstoi(str);

	if (pre != value)
	{
		// Update
		wsprintf(str, _T("%f"), value);
		WritePrivateProfileString(disk, file, str, dir + _T("\\") + SMART_INI);

		CString line;
		line.Format(_T("%s,%f\n"), time.Format(_T("%Y/%m/%d %H:%M:%S")), value);

		CStdioFile outFile;
		if (outFile.Open(dir + _T("\\") + file + _T(".csv"),
			CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::typeText))
		{
			ULONGLONG fileLength = outFile.GetLength();
			try
			{
				if (outFile.SeekToEnd() == fileLength)
				{
					outFile.WriteString(line);
				}
			}
			catch (CFileException* e)
			{
				DebugPrint(L"CFileException");
				e->Delete();
			}
			outFile.Close();
			return TRUE;
		}
	}
	return FALSE;
}
