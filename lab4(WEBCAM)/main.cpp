#include <Windows.h>
#include <setupapi.h>
#include <iostream>
#include <wdmguid.h>
#include <devguid.h>
#include <ctime>
#include <sstream>
#include <conio.h>

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

using namespace std;
using namespace cv;

#define FRAME_FREQUENCY 10
#define FRAME_TIME 1000 / FRAME_FREQUENCY
static string SAVE_PATH = "WEBCAM/";

#pragma comment(lib, "setupapi.lib")

string makeName()
{
	time_t now = time(0);
	tm* ltm = new tm;
	localtime_s(ltm, &now);
	stringstream sstream;
	sstream << ltm->tm_year + 1900 << '_'
		<< ltm->tm_mon + 1 << '_'
		<< ltm->tm_mday << '_'
		<< ltm->tm_hour << '_'
		<< ltm->tm_min << '_'
		<< ltm->tm_sec;
	return sstream.str();
}

void displayCameraInfo()
{
	SP_DEVINFO_DATA DeviceInfoData = { 0 };
	HDEVINFO DeviceInfoSet = SetupDiGetClassDevsA(&GUID_DEVCLASS_CAMERA, "USB", NULL, DIGCF_PRESENT);
	if (DeviceInfoSet == INVALID_HANDLE_VALUE) {
		exit(1);
	}
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	SetupDiEnumDeviceInfo(DeviceInfoSet, 0, &DeviceInfoData);

	char deviceID[256];
	char deviceName[256];
	char deviceManufacturer[256];

	SetupDiGetDeviceRegistryPropertyA(DeviceInfoSet, &DeviceInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)deviceName, sizeof(deviceName), 0);
	SetupDiGetDeviceRegistryPropertyA(DeviceInfoSet, &DeviceInfoData, SPDRP_MFG, NULL, (PBYTE)deviceManufacturer, sizeof(deviceManufacturer), 0);
	SetupDiGetDeviceRegistryPropertyA(DeviceInfoSet, &DeviceInfoData, SPDRP_HARDWAREID, NULL, (PBYTE)deviceID, sizeof(deviceID), 0);
	SetupDiDestroyDeviceInfoList(DeviceInfoSet);
	cout << left << setw(20) << "Name: " << (char*)deviceName << endl
		<< setw(20) << "Manufacturer: " << (char*)deviceManufacturer << endl
		<< setw(20) << "Camera HardwareID: " << (char*)deviceID << endl << endl;
}


int main() {
	system("chcp 1251");
	system("cls");

	RegisterHotKey(NULL, 0, MOD_ALT, 0x50);  // ALT + P to take photo
	RegisterHotKey(NULL, 2, MOD_ALT, VK_ESCAPE);  // ALT + ESC to return to the normal mode
	MSG msg = { 0 };

	displayCameraInfo();

	VideoCapture webcam;
	
	if (!webcam.open(0, CAP_DSHOW)) { //DirectShow (via videoInput)
		cerr << "ERROR! Unable to open camera\n";
		return -1;
	}
	webcam.set(CAP_PROP_FRAME_WIDTH, 640);
	webcam.set(CAP_PROP_FRAME_HEIGHT, 480);
	Mat frame;

	int choise = 0;
	do
	{
		cout << "Choose action:\n 1 - Take photo\n 2 - Capture video\n 3 - Hiden mode(alt + p - take photo, alt + esc - back to the normal mode)\n 0 - Exit\n";
		cin >> choise;
		switch (choise)
		{
		case 1: {
			webcam >> frame >> frame; 
			imwrite(SAVE_PATH + "photo_" + makeName() + ".png", frame);
			break;
		}
		case 2:
		{
			long videoLenght;
			cout << "Video lenght in seconds: ";
			cin >> videoLenght;
			VideoWriter videoWriter(SAVE_PATH + "video_" + makeName() + ".avi", VideoWriter::fourcc('X', 'V', 'I', 'D'), FRAME_FREQUENCY, Size(640, 480), true);
			long long curFrame = 0; //counter
			auto prevClock = clock();
			while (curFrame < FRAME_FREQUENCY * videoLenght)
			{
				while (clock() - prevClock < FRAME_TIME); //one frame time

				prevClock = clock();
				curFrame++;
				webcam >> frame;
				videoWriter.write(frame);
			}
			break;
		}
		case 3:
		{

			ShowWindow(GetConsoleWindow(), SW_HIDE);
			bool exit = false;

			while (GetMessage(&msg, NULL, 0, 0) != 0 && exit == false) {
				if (msg.message == WM_HOTKEY) {
					switch (msg.wParam)
					{
					case 0:
					{
						webcam >> frame >> frame;
						imwrite(SAVE_PATH + "photo_" + makeName() + ".png", frame); break;
						break;
					}
					case 2:
					{
						exit = true;
						break;
					}
					default:
						break;
					}
				}
			}

			ShowWindow(GetConsoleWindow(), SW_RESTORE);
			break;
		}
		case 0:
		{
			webcam.release();
			exit(0);
		}
		default: break;
		}
	} while (choise == 1 || choise == 2 || choise == 3);

	return 0;
}

