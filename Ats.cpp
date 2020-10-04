#include "stdafx.h"
#include "atsplugin.h"
#include "readIni.h"
#include "Ats.h"
#include <cmath>

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved){
	char filePath[_MAX_PATH + 1] = _T("");						//ファイルパス格納
	char* posIni;												//検索文字列へのポインタ
	bool loadCheck;												//INIファイルのロードに成功したかどうか
	::GetModuleFileName((HMODULE)hModule, filePath, _MAX_PATH);	//Ats.dllのファイルパスを取得
	posIni = strstr(filePath, ".dll");							//パスから.dllの位置を検索
	memmove(posIni, ".ini", 4);									//.dllを.iniに置換
	loadCheck = ini.load(filePath);								//INIファイルをロードして結果を取得

	DEC_EBN = ini.BRAKE.EBN;
	DEC_B8N = ini.BRAKE.B8N;
	DEC_B7N = ini.BRAKE.B7N;
	DEC_B4N = ini.BRAKE.B4N;

	KAITSU_SECTION  = ini.PANEL.index0;
	TEISHI_SECTION  = ini.PANEL.index1;
	STATION_SECTION = ini.PANEL.index2;
	DIGITAL_ATC     = ini.PANEL.index3;
	PATTERN_TEIGEN  = ini.PANEL.index4;
	EM_DRIVE        = ini.PANEL.index5;
	ATC_BRAKE       = ini.PANEL.index6;
	ATC_EB          = ini.PANEL.index7;
	TEITSU_BOUSHI   = ini.PANEL.index8;
	ATC_POWER       = ini.PANEL.index9;
	ATC_FREE        = ini.PANEL.index10;
	PATTERN_AP      = ini.PANEL.index11;
	EBN_PATTERN_SPEED = ini.PANEL.index12;
	B8N_PATTERN_SPEED = ini.PANEL.index13;
	B7N_PATTERN_SPEED = ini.PANEL.index14;
	B4N_PATTERN_SPEED = ini.PANEL.index15;
	BRAKE_PATTTERN    = ini.PANEL.index16;
	BRAKE_PATTTERN_5  = ini.PANEL.index17;
	OPEN_DISTANCE     = ini.PANEL.index18;
	TRACK_NAME        = ini.PANEL.index19;

	DATC_BEACON               = ini.BEACON.type0;
	SET_STATION_SECTION_START = ini.BEACON.type1;
	SET_STATION_SECTION_END   = ini.BEACON.type2;
	SET_TRACKNAME             = ini.BEACON.type3;
	STOP_DISTANCE             = ini.BEACON.type4;
	
	DATC_BRAKE = (ini.OPTION.brake > 0) ? true : false;

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

	updateStationIndex();
	

	
	// セクションの開始からの開通距離
	passableDistance = trackCircuit[signalIdx-1];	
	double run = location - beaconLocation;
	double remDis = passableDistance - run;

	bool isEmgBrake = false;
	bool isBrake8N = false;
	bool isBrake7N = false;
	bool isBrake4N = false;
	// 駅構内か？(停車前)
	bool inStationSection = false;
	// 低減パターン？
	bool TEIGEN = false;
	double speedLimit = getPatternBxN(remDis, DEC_B7N);
	// パターンに触れたか？
	bool isPattern = false;

	if (DATC_BRAKE) {
		isPattern = isPatternAP(remDis, DEC_B7N);

		if (isBrakePattern(remDis, DEC_EBN)) {
			output.Brake = emgBrake;
			output.Power = 0;
			isEmgBrake = true;
		} else if (isBrakePattern(remDis, DEC_B8N) && inStationSection) {
			output.Brake = svcBrake - 1;
			output.Power = 0;
			isBrake8N = true;
		} else if (isBrakePattern(remDis, DEC_B7N)) {
			output.Brake = svcBrake - 1;
			output.Power = 0;
			isBrake7N = true;
		} else if (isBrakePattern(remDis, DEC_B4N) && TEIGEN) {
			output.Brake = 4;
			output.Power = 0;
			isBrake4N = true;
		}
	}
	// panel[0] = getDigitOfNumber(speed, 3, 10); 
	// panel[1] = getDigitOfNumber(speed, 2, 10);
	// panel[2] = getDigitOfNumber(speed, 1,  0);
	// panel[3] = getDigitOfNumber(speedLimit, 3, 10);
	// panel[4] = getDigitOfNumber(speedLimit, 2, 10);
	// panel[5] = getDigitOfNumber(speedLimit, 1,  0);

	// panel[10] = output.Brake;
	// panel[11] = output.Power;

	// panel[34] = getDigitOfNumber(remDis,  4, 30);
	// panel[35] = getDigitOfNumber(remDis,  3, 30);
	// panel[36] = getDigitOfNumber(remDis,  2, 30);
	// panel[37] = getDigitOfNumber(remDis,  1,  0);

	// panel[200] = getDigitOfNumber(stationIdx, 2, 30);
	// panel[201] = getDigitOfNumber(stationIdx, 1,  0);

	if (DIGITAL_ATC >= 0)      panel[DIGITAL_ATC]      = 1;
	if (PATTERN_TEIGEN >= 0)   panel[PATTERN_TEIGEN]   = 0;
	if (EM_DRIVE >= 0)         panel[EM_DRIVE]         = 0;
	if (ATC_BRAKE >= 0)        panel[ATC_BRAKE]        = isBrake7N;
	if (ATC_EB >= 0)           panel[ATC_EB]           = isEmgBrake;
	if (TEITSU_BOUSHI >= 0)    panel[TEITSU_BOUSHI]    = isBrake8N;
	if (ATC_POWER >= 0)        panel[ATC_POWER]        = 1;
	if (ATC_FREE >= 0)         panel[ATC_FREE]         = 0;
	if (PATTERN_AP >= 0)       panel[PATTERN_AP]       = isPattern;
	if (BRAKE_PATTTERN >= 0)   panel[BRAKE_PATTTERN]   = (int)(speedLimit/5)*5;
	if (BRAKE_PATTTERN_5 >= 0) panel[BRAKE_PATTTERN_5] = 1;
	if (OPEN_DISTANCE >= 0)    panel[OPEN_DISTANCE]    = 1;
	if (TRACK_NAME >= 0)       panel[TRACK_NAME]       = 1;
	
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

	if (type == DATC_BEACON) {
		if (sendData < 0) {
			passableDistance = 0;
			passableEnd = 0;
			beaconLocation = -1.0 * (double)sendData;
			loopNum++;
			loopNum %= 100;
			return;
		}
		trackCircuit[sendData - 1] = sectionDistance;
	} else if (type == SET_STATION_SECTION_START) {
		n = sendData / 1000000;
		if (n > 0 && n < 100) {
			staSection[n - 1].id = n;
			staSection[n - 1].start = sendData % 1000000;
		}
	} else if (type == SET_STATION_SECTION_END) {
		n = sendData / 1000000;
		if (n > 0 && n < 100) {
			staSection[n - 1].id = n;
			staSection[n - 1].end = sendData % 1000000;
		}
	} else if (type == SET_TRACKNAME) {

	} else if (type == STOP_DISTANCE) {

	}
}
double getPatternBxN(double remDis, double dec) {
	double margin = 5.0;
	double pattern =
		sqrt(7.2 * dec * (remDis - margin));
	return pattern;
}
// --------------------------------------------------------
bool isBrakePattern(double remDis, double dec) {
	return (getPatternBxN(remDis, dec) > speed) ? false : true;
}
// --------------------------------------------------------
bool isPatternAP(double remDis, double dec) {
	double postion = speed / 3600.0 * 2.5 * 1000.0;
	return (getPatternBxN(remDis - postion, dec) >= speed) ? false : true;
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
			panel[KAITSU_SECTION + i] = 32;
		return;
	}
	for (int i = 0; i < 10; ++i) {
		if (remDis >= (double)(i+1)*100.0) {
			panel[KAITSU_SECTION + i] = 32;
			panel[TEISHI_SECTION + i] = 32;
			continue;
		}
		double rDis = remDis - (double)(i*100);
		panel[KAITSU_SECTION + i] =
			(int)(rDis / 100.0 * 32.0);
		int idx = panel[KAITSU_SECTION + i];

		panel[TEISHI_SECTION + i] = idx;
		
		for (int j = i + 1; j < 10; ++j) {
			panel[KAITSU_SECTION + j] = 0;
			panel[TEISHI_SECTION + j] = 32;
			if (idx > 0) {
				panel[TEISHI_SECTION + j] = 31 + idx;
				idx = 0;
			}
		}
		return;
	}
}
void updateStationIndex() {
	int id = staSection[stationIdx].id;
	if (id < 0) return;
	int eDis = staSection[stationIdx].end;
	if ((double)eDis < location && stationIdx < 98)
		stationIdx++;
}
void dispStationSectionOut(int* panel, double remDis, int i) {
	int idx = (int)((remDis - 100.0 * i) / 100.0 * 32.0);
	panel[STATION_SECTION + i] = 32 + idx;
	for (int j = i ; j < 10; ++j) panel[STATION_SECTION + j] = 32;
}
// --------------------------------------------------------
void dispStationSection(int* panel, double remDis, int staNum, int loopCnt) {
	if (loopCnt == 0)
		for (int i = 0; i < 10; ++i) panel[STATION_SECTION + i] = 32;

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

		if (remSDis > 100) {
			// 駅閉塞100m圏外
			panel[STATION_SECTION + i] = 32;
			continue; 
		}

		if (remDis - 100.0 * i < 100) {
			double d = remDis - 100.0 * i;
			if (d >= 0) {
				// panel[200] = getDigitOfNumber(abs(d), 3, 30);
				// panel[201] = getDigitOfNumber(abs(d), 2, 30);
				// panel[202] = getDigitOfNumber(abs(d), 1, 0);
				// panel[203] = i;

				// if (d <= remDis)
				idx = (int)(d / 100.0 * 32.0);
				
				// panel[STATION_SECTION + i] = 32 + idx;

				//if (i == 0) {
				//	if (remSDis < 0) {
				//		panel[STATION_SECTION + i] = 32 + idx;
				//	} else {
				//		panel[STATION_SECTION + i] = idx;
				//	}
				//}
				
				if ((int)remSDis >= (int)d) {
					// まだ駅閉塞に達していないとき
					panel[STATION_SECTION + i] = 31;
				} else {
					// いま駅閉塞にいるとき
					panel[STATION_SECTION + i] = 32 + idx;
				}

				for (int j = i + 1; j < 10; ++j) 
					panel[STATION_SECTION + j] = 32;
			} else {
				int cnt = 0;
				while (d < 0) {
					d += 100;
					cnt++;
				}
				// panel[200] = getDigitOfNumber(abs(d), 3, 30);
				// panel[201] = getDigitOfNumber(abs(d), 2, 30);
				// panel[202] = getDigitOfNumber(abs(d), 1, 0);
				// panel[203] = 20;
				dispStationSectionOut(panel, remDis, i-cnt);
			}
			break;
		}

		if (remSDis < 0) {
			if (remEDis > 100.0) {
				// 駅閉塞100m圏内
				panel[STATION_SECTION + i] = 0;
				continue;
			}
			idx = (int)(remEDis / 100.0 * 32.0);
			panel[STATION_SECTION + i] = 32 + idx;
			for (int j = i + 1; j < 10; ++j) 
				panel[STATION_SECTION + j] = 32;

			if (i + 1 < 10)
				dispStationSection(panel, remDis, id + 1, i + 1);
			break;
		} else if (remSDis >= 0) {
			// 駅閉塞開始まであと
			idx = (int)(remSDis / 100.0 * 32.0);
			panel[STATION_SECTION + i] = idx;
			continue;
		}
	}
}