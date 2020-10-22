#include <iostream>
#include <windows.h>
#include <string>
#include <fileapi.h>
#include <vector>
#include <iomanip>

using namespace std;

vector<string> busTypes = { "Unknown", "Scsi", "Atapi", "Ata", "1394",
							"Ssa", "Fibre", "Usb", "RAID", "iScsi",
							"Sas", "SATA", "Sd", "Mmc", "Virtual",
							"FileBackedVirtual", "Spaces" };

#define BUF_SIZE 256

DWORD getDeviceInfo(HANDLE driveHandle) {

	//storage property query указывает свойства диска
	STORAGE_PROPERTY_QUERY query = { StorageDeviceTrimProperty, PropertyStandardQuery };
	DWORD bytesReturned = 0;

	// obtaining drive type
	DEVICE_TRIM_DESCRIPTOR dtd;
	//check SSD with trim
	if (DeviceIoControl(driveHandle, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(STORAGE_PROPERTY_QUERY),//заставляем устройство выполнить соответствующую операцию
		&dtd, sizeof(DEVICE_TRIM_DESCRIPTOR), &bytesReturned, NULL)) {
		if (dtd.TrimEnabled) {
			cout << "Drive type: SSD" << endl;
		}
		else
			cout << "Drive type: HDD" << endl;

		//return GetLastError();
	}
	else return -1;

	// obtaining drive model, manufacturer, serial number, firmware, bus type
	query.PropertyId = StorageDeviceProperty;

	STORAGE_DESCRIPTOR_HEADER storage_header;
	ZeroMemory(&storage_header, sizeof(STORAGE_DESCRIPTOR_HEADER));

	if (!DeviceIoControl(driveHandle, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(STORAGE_PROPERTY_QUERY),
		&storage_header, sizeof(STORAGE_DESCRIPTOR_HEADER), &bytesReturned, NULL)) {
		return -1;
	}

	char *outBuffer = new char[storage_header.Size];

	if (DeviceIoControl(driveHandle, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(STORAGE_PROPERTY_QUERY),
		outBuffer, storage_header.Size, &bytesReturned, NULL)) {
		STORAGE_DEVICE_DESCRIPTOR *sdd = (STORAGE_DEVICE_DESCRIPTOR*)outBuffer;
		if (sdd->ProductIdOffset != 0) { cout << "Model: " << outBuffer + sdd->ProductIdOffset << endl; }
		if (sdd->VendorIdOffset != 0) { cout << "VendorId: " << outBuffer + sdd->VendorIdOffset << endl; }
		if (sdd->ProductRevisionOffset != 0) { cout << "Firmware: " << outBuffer + sdd->ProductRevisionOffset << endl; }
		if (sdd->SerialNumberOffset != 0) { cout << "Serial number: " << outBuffer + sdd->SerialNumberOffset << endl; }
		cout << "Bus type: " << busTypes.at(sdd->BusType) << endl;
	}
	else return -1;

	// drive transfer mode
	query.PropertyId = StorageAdapterProperty;

	STORAGE_ADAPTER_DESCRIPTOR sad;

	if (DeviceIoControl(driveHandle, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(STORAGE_PROPERTY_QUERY),
		&sad, sizeof(STORAGE_ADAPTER_DESCRIPTOR), &bytesReturned, NULL)) {
		cout << "Current transfer mode: ";
		if (sad.AdapterUsesPio) {
			cout << "PIO";
		}
		else cout << "DMA";
		cout << endl;
		//return GetLastError();
	}
	else return -1;

	delete[]outBuffer;

	return 0;
}


DWORD getMemoryInfo() {

	ULARGE_INTEGER freeBytesAvailableToCaller;
	ULARGE_INTEGER totalBytes;
	ULARGE_INTEGER totalFreeBytes;

	char *buffer = new char[BUF_SIZE];
	DWORD strSize = 0;
	strSize = GetLogicalDriveStrings(BUF_SIZE, buffer);

	for (int i = 0; i < strSize; i += 4) {
		char *littleBuffer = new char[5];
		int pos = i;
		for (int j = 0; j < 4; j++, pos++) {
			littleBuffer[j] = buffer[pos];
		}
		if (GetDiskFreeSpaceEx(littleBuffer, &freeBytesAvailableToCaller, &totalBytes, &totalFreeBytes)) {
			cout << "\nVolume: " << littleBuffer << endl << "Total: " << totalBytes.QuadPart / (1024 * 1024 * 1024) << "GB\n"
				<< "Used: " << (totalBytes.QuadPart - totalFreeBytes.QuadPart) / (1024 * 1024 * 1024) << "GB\n"
				<< "Free: " << totalFreeBytes.QuadPart / (1024 * 1024 * 1024) << "GB\n";
		}
		else
			return -1;
		delete[]littleBuffer;
	}
	delete[]buffer;

	return 0;
}

int main() {

	HANDLE driveHandle;

	string driveName = "//./PhysicalDrive";

	for (int i = 0; ; i++) {
		//open the drive
		if ((driveHandle = CreateFile((driveName + to_string(i)).c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE) {
			cout << "Drive name: PhysicalDrive" << to_string(i) << endl;
			if (getDeviceInfo(driveHandle) == -1) { break; }
			if (getMemoryInfo() == -1) { break; }
		}
		else {
			system("pause");
			return 0;
		}
	}
	system("pause");
	return 0;
}