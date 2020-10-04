int emgBrake;			
int svcBrake;			
int brakeNotch;		
int powerNotch;		
int reverser;			
bool pilotlamp;		
int time;				
float speed;			
int deltaT;			
double location;
double beaconLocation;
double passableDistance;
double passableEnd;
ATS_HANDLES output;	
readIni ini;

double DEC_EBN;
double DEC_B8N;
double DEC_B7N;
double DEC_B4N;

int KAITSU_SECTION;
int STATION_SECTION;
int TEISHI_SECTION;
int DIGITAL_ATC;
int PATTERN_TEIGEN;
int EM_DRIVE;
int ATC_BRAKE;
int ATC_EB;
int TEITSU_BOUSHI;
int ATC_POWER;
int ATC_FREE;
int PATTERN_AP;
int EBN_PATTERN_SPEED;
int B8N_PATTERN_SPEED;
int B7N_PATTERN_SPEED;
int B4N_PATTERN_SPEED;
int BRAKE_PATTTERN;
int BRAKE_PATTTERN_5;
int OPEN_DISTANCE;
int TRACK_NAME;

int DATC_BEACON;
int SET_STATION_SECTION_START;
int SET_STATION_SECTION_END;
int SET_TRACKNAME;
int STOP_DISTANCE;

bool DATC_BRAKE;
struct stationSection {
	int id;
	int start;
	int end;
};

stationSection staSection[99];

int panelIdx[5] = {0,1,2,3,4};
int mode;
int signalIdx;
int stationIdx;
double trackCircuit[20];
int loopNum;

double getPatternBxN(double, double);
bool isPatternAP(double, double);
bool isBrakePattern(double, double);
int getDigitOfNumber(int, int, int);
void dispTrackCircuit(int*, double);

void dispStationSection(int*, double, int, int);
void updateStationIndex();
void dispStationSectionOut(int*, double, int);