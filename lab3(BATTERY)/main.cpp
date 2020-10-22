#pragma comment(lib, "PowrProf.lib")
#include <Windows.h>
#include <iostream>
#include <powrprof.h>
#include <thread>
#include <conio.h>

using namespace std;

char choice;
int flag = 0;

void getSystemPowerInfo() {

	while (!flag) {
		cout << "Press 0 to turn computer into sleep mode" << endl;
		cout << "Press 1 to turn computer intp hibernate mode" << endl;
		cout << "Press 2 to exit" << endl << endl;

		SYSTEM_POWER_STATUS powerStatus;
		char ac[2][8] = { "offline", "online" };
		char saver[2][8] = { "off", "on" };
		GetSystemPowerStatus(&powerStatus);

		if (GetSystemPowerStatus(&powerStatus)) {
			cout << "Battery percentage: " << (int)powerStatus.BatteryLifePercent << endl;

			if (powerStatus.BatteryLifeTime != -1) {

				cout << "Battery life time: " << powerStatus.BatteryLifeTime / 60 / 60 << " h " << powerStatus.BatteryLifeTime / 60 % 60 << " min" << endl;
			}

			cout << "Battery save mode " << saver[powerStatus.SystemStatusFlag] << endl;
			cout << "AC line status: " << ac[powerStatus.ACLineStatus] << endl << endl;
			Sleep(1000);
			system("CLS");
		}
		else {
			exit(1);
		}
		
	}
	
}


int main() {

	thread thread(getSystemPowerInfo);

	while (choice != '2') {

		choice = _getch();

		switch (choice) {

		case('0'):
			if (!SetSuspendState(FALSE, FALSE, FALSE)) {
				exit(1);
			}
			break;

		case('1'):
			if (!SetSuspendState(TRUE, FALSE, FALSE)) {
				exit(1);
			}
			break;
		}
		
	}

	flag = 1;
	thread.join();
	return 0;
}


