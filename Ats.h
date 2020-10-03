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
DistanceIni ini;

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

double getPatternB7N(double);
bool isPatternAP(double);
int getDigitOfNumber(int, int, int);
void dispTrackCircuit(int*, double);

void dispStationSection(int*, double, int);
void dispStationSection(int*, double, int, int);
void updateStationIndex();