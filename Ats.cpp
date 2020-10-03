#include "stdafx.h"
#include "atsplugin.h"
#include "DistanceIni.h"
#include "Ats.h"
#include <cmath>

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved){
	char filePath[_MAX_PATH + 1] = _T("");						//ファイルパス格納
	char* posIni;												//検索文字列へのポインタ
	bool loadCheck;												//INIファイルのロードに成功したかどうか
	::GetModuleFileName((HMODULE)hModule, filePath, _MAX_PATH);	//Ats.dllのファイルパスを取得
	posIni = strstr(filePath, ".dll");							//パスから.dllの位置を検索
	memmove(posIni, ".ini", 4);									//.dllを.iniに置換
	loadCheck = ini.load(filePath);							//INIファイルをロードして結果を取得
	panelIdx[0] = ini.PANEL.index1;
	panelIdx[1] = ini.PANEL.index2;
	panelIdx[2] = ini.PANEL.index3;
	panelIdx[3] = ini.PANEL.index4;
	panelIdx[4] = ini.PANEL.index5;
	mode = ini.MODE.distance;
	return TRUE;
}
ATS_API int WINAPI GetPluginVersion() { return ATS_VERSION; }
ATS_API void WINAPI SetVehicleSpec(ATS_VEHICLESPEC vehicleSpec) {
	svcBrake = vehicleSpec.BrakeNotches;
	emgBrake = svcBrake + 1;
}
ATS_API void WINAPI Initialize(int brake) { 
	speed = 0;
	beaconLocation = 0;
	signalIdx = 0;
	stationIdx = 0;
	for (int i = 0; i < 20; ++i) trackCircuit[i] = 0.0;
	for (int i = 0; i < 99; ++i) { 
		staSection[i].id = -1; 
		staSection[i].start = INT_MAX;
		staSection[i].end = INT_MAX;
	}
}
ATS_API ATS_HANDLES WINAPI Elapse(ATS_VEHICLESTATE vehicleState, int *panel, int *sound){
	deltaT = vehicleState.Time - time;
	time = vehicleState.Time;
	speed = vehicleState.Speed;
	location = vehicleState.Location;

	if (pilotlamp) { output.Reverser = reverser; }
	else { output.Reverser = 0; }
	output.Brake = brakeNotch;
	output.Power = powerNotch;
	output.ConstantSpeed = ATS_CONSTANTSPEED_CONTINUE;

	passableDistance = trackCircuit[signalIdx-1];
	
	updateStationIndex();
	
	double run = location - beaconLocation;
	double remDis = passableDistance - run;
	
	double speedLimit = getPatternB7N(remDis);

	panel[0] = getDigitOfNumber(speed, 3, 10); 
	panel[1] = getDigitOfNumber(speed, 2, 10);
	panel[2] = getDigitOfNumber(speed, 1,  0);
	panel[3] = getDigitOfNumber(speedLimit, 3, 10);
	panel[4] = getDigitOfNumber(speedLimit, 2, 10);
	panel[5] = getDigitOfNumber(speedLimit, 1,  0);

	panel[10] = output.Brake;
	panel[11] = output.Power;

	panel[34] = getDigitOfNumber(remDis,  4, 30);
	panel[35] = getDigitOfNumber(remDis,  3, 30);
	panel[36] = getDigitOfNumber(remDis,  2, 30);
	panel[37] = getDigitOfNumber(remDis,  1,  0);

	// panel[200] = getDigitOfNumber(stationIdx, 2, 30);
	// panel[201] = getDigitOfNumber(stationIdx, 1,  0);

	panel[204] = isPatternAP(remDis);

	dispTrackCircuit(panel, remDis);
	dispStationSection(panel, remDis, stationIdx, 0);
    return output;
}
ATS_API void WINAPI Load() {}
ATS_API void WINAPI Dispose() {}
ATS_API void WINAPI SetPower(int notch) { powerNotch = notch; }
ATS_API void WINAPI SetBrake(int notch) { brakeNotch = notch; }
ATS_API void WINAPI SetReverser(int pos) { reverser = pos; }
ATS_API void WINAPI KeyDown(int atsKeyCode) {}
ATS_API void WINAPI KeyUp(int hornType) {}
ATS_API void WINAPI HornBlow(int atsHornBlowIndex) {}
ATS_API void WINAPI DoorOpen() { pilotlamp = false; }
ATS_API void WINAPI DoorClose() { pilotlamp = true; }
ATS_API void WINAPI SetSignal(int signal) { signalIdx = signal; }
// --------------------------------------------------------
ATS_API void WINAPI SetBeaconData(ATS_BEACONDATA beaconData) {
	int sendData = beaconData.Optional;
	int type = beaconData.Type;
	double sectionDistance = beaconData.Distance;
	int signal = beaconData.Signal;
	int n = 0;
	switch (type) {
		case 0:
			if (sendData < 0) { 
				passableDistance = 0; 
				passableEnd = 0;
				beaconLocation = -1.0*(double)sendData;
				loopNum++;
				loopNum %= 100;
				return;
			}
			trackCircuit[sendData-1] = sectionDistance;
			break;
		case 200:
			n = sendData / 1000000;
			if ( n > 0 && n < 100) {
				staSection[n - 1].id = n;
				staSection[n - 1].start = sendData % 1000000;
			}
			break;
		case 201:
			n = sendData / 1000000;
			if (n > 0 && n < 100) {
				staSection[n - 1].id = n;
				staSection[n - 1].end = sendData % 1000000;
			}
			break;
	};
}
// --------------------------------------------------------
double getPatternB7N(double remDis) {
	double decelerate = 3.0;
	double pattern =
		sqrt(7.2 * decelerate * (remDis - 0.0));
	return pattern;
}
// --------------------------------------------------------
bool isPatternAP(double remDis) {
	double postion = speed / 3600.0 * 2.5 * 1000.0;
	return (getPatternB7N(remDis - postion) > speed) ? false : true;
}
// --------------------------------------------------------
// 指定桁の数字を返す(十の位以上は0のと30を返す)
int getDigitOfNumber(int num, int digit, int rt) {
	int n = 0;
	for (int i = 0; i < 10; ++i) {
		if (num / pow(10, i) < 1) {
			n = i;
			break;
		}
	}
	if (digit > n) return rt;
	int result = (int)(num / pow(10, digit - 1)) % 10;
	if (digit >= n && result == 0) return rt;
	return result;
}
// --------------------------------------------------------
void dispTrackCircuit(int* panel, double remDis) {
	if (remDis >= 1000) {
		for (int i = 0; i < 10; ++i)
			panel[210 + i] = 32;
		return;
	}
	for (int i = 0; i < 10; ++i) {
		if (remDis >= (double)(i+1)*100.0) {
			panel[210 + i] = 32;
			panel[220 + i] = 32;
			continue;
		}
		double rDis = remDis - (double)(i*100);
		panel[210 + i] =
			(int)(rDis / 100.0 * 32.0);
		int idx = panel[210 + i];

		panel[220 + i] = idx;
		
		for (int j = i + 1; j < 10; ++j) {
			panel[210 + j] = 0;
			panel[220 + j] = 32;
			if (idx > 0) {
				panel[220 + j] = 31 + idx;
				idx = 0;
			}
		}
		return;
	}
}

void dispStationSection(int* panel, double remDis, int staNum) {
	//updateStationIndex();
	//int id = staSection[staNum].id;
	//if (id < 0) return;

	//int start = staSection[staNum].start;
	//int end = staSection[staNum].end;
	//int lenght = end - start;
	//double sDis = (double)start - location;	// 駅構内開始までの距離
	//double eDis = (double)end - location;	// 駅構内終了までの距離
	//bool enable = false;
	//int idx = 0;

	//panel[6] = getDigitOfNumber(sDis, 3, 10);
	//panel[7] = getDigitOfNumber(sDis, 2, 10);
	//panel[8] = getDigitOfNumber(sDis, 1,  0);

	//panel[28] = getDigitOfNumber(eDis, 3, 10);
	//panel[29] = getDigitOfNumber(eDis, 2, 10);
	//panel[30] = getDigitOfNumber(eDis, 1,  0);

	//for (int i = 0; i < 10; ++i) panel[230 + i] = 32;
	//
	//if (start > location + 1000) return;

	//for (int i = 0; i < 10; ++i) {
	//	// 今見ているブロック(100m単位)
	//	int dispDis = (100 * (i + 1) + location);
	//	// 駅閉塞開始まで
	//	double remSDis = sDis - (double)100*i;
	//	// 駅閉塞終了まで
	//	double remEDis = eDis - (double)100*i;

	//	// if (remSDis > dispDis) {
	//	if (remSDis > 100) {
	//		// 駅閉塞100m圏外
	//		panel[230 + i] = 32;
	//		continue; 
	//	}

	//	if (remSDis < 0) {
	//		if (remEDis > 100.0) {
	//			// 駅閉塞100m圏内
	//			panel[230 + i] = 0;
	//			continue;
	//		}
	//		idx = (int)(remEDis / 100.0 * 32.0);
	//		panel[230 + i] = 32 + idx;
	//		for (int j = i + 1; j < 10; ++j) 
	//			panel[230 + j] = 32;

	//		panel[202] = getDigitOfNumber(idx, 2, 30);
	//		panel[203] = getDigitOfNumber(idx, 1,  0);
	//		break;
	//	} else if (remSDis >= 0) {
	//		// 駅閉塞開始まであと
	//		idx = (int)(remSDis / 100.0 * 32.0);
	//		panel[230 + i] = idx;
	//		continue;
	//	}
	//}
	//// dispStationSection(panel, remDis, staNum+1);
}
void updateStationIndex() {
	int id = staSection[stationIdx].id;
	if (id < 0) return;
	int eDis = staSection[stationIdx].end;
	if ((double)eDis < location && stationIdx < 98) {
		// stationIdx = staSection[stationIdx + 1].id;
		stationIdx++;
	}
		
}
// --------------------------------------------------------
void dispStationSection(int* panel, double remDis, int staNum, int loopCnt) {
	if (loopCnt == 0)
		for (int i = 0; i < 10; ++i) panel[230 + i] = 32;

	int id = staNum;
	if (id < 0) return;

	int start = staSection[id].start;
	int end = staSection[id].end;
	int lenght = end - start;
	double sDis = (double)start - location;	// 駅構内開始までの距離
	double eDis = (double)end - location;	// 駅構内終了までの距離
	bool enable = false;
	int idx = 0;

	if (start > location + 1000) return;
	
	for (int i = loopCnt; i < 10; ++i) {
		// 駅閉塞開始まで
		double remSDis = sDis - (double)100*i;
		// 駅閉塞終了まで
		double remEDis = eDis - (double)100*i;

		if (remDis - 100.0 * i <= 0) {
			for (int j = i; j < 10; ++j) 
				panel[230 + j] = 32;
			break;
		}

		if (remSDis > 100) {
			// 駅閉塞100m圏外
			panel[230 + i] = 32;
			continue; 
		}

		if (remSDis < 0) {
			if (remEDis > 100.0) {
				// 駅閉塞100m圏内
				panel[230 + i] = 0;
				continue;
			}
			idx = (int)(remEDis / 100.0 * 32.0);
			panel[230 + i] = 32 + idx;
			for (int j = i + 1; j < 10; ++j) 
				panel[230 + j] = 32;

			if (i + 1 < 10)
				dispStationSection(panel, remDis, id + 1, i + 1);
			break;
		} else if (remSDis >= 0) {
			// 駅閉塞開始まであと
			idx = (int)(remSDis / 100.0 * 32.0);
			panel[230 + i] = idx;
			continue;
		}
	}
}