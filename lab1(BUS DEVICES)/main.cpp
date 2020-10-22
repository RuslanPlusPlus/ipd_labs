#pragma comment (lib, "setupapi.lib")
#include <iostream>
#include <windows.h>
#include <string>
#include <SetupAPI.h>

using namespace std;

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	
	HDEVINFO list = SetupDiGetClassDevs(NULL, 
		"PCI", 
		NULL, 
		DIGCF_ALLCLASSES | DIGCF_PRESENT); 
	

	if (list == INVALID_HANDLE_VALUE) {
		return GetLastError();
	}

	SP_DEVINFO_DATA data; 
	data.cbSize = sizeof(SP_DEVINFO_DATA);

	BYTE buffer[256];
	BYTE name_buffer[256];
	
	for (int i = 0; SetupDiEnumDeviceInfo(list, i, &data); i++) {

		if (SetupDiGetDeviceRegistryProperty(list, &data, SPDRP_HARDWAREID, NULL, buffer, 256, NULL) && 
			SetupDiGetDeviceRegistryProperty(list, &data, SPDRP_DEVICEDESC, NULL, name_buffer, 256, NULL)) {
			string device_name((const char *)name_buffer);
			string devices_id((const char *)buffer);
			string DeviceId = devices_id.substr(17, 4);
			string VendorId = devices_id.substr(8, 4);
			cout << device_name << endl << DeviceId << "|" << VendorId << endl << "_______________________________________" << endl;
		}
	}

	SetupDiDestroyDeviceInfoList(list);

	cout << "############################# DESTROY #############################" << endl;
	if (!SetupDiEnumDeviceInfo(list, 0, &data)) {
		cout << "Cool!!!" << endl;
	}

	system("pause");
	return 0;
}