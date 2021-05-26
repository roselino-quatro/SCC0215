#include "vehicle.h"
#include "line.h"
#include "utils.h"

int main(int argc, char *argv[]){
	FILE* vcsv = openFile(argv[1],"rb");
	VehicleData* vData = readVehicleCsv(vcsv);
	fclose(vcsv);
	
	FILE* bin = openFile("teste","wb");
	writeVehicleBinary(vData,bin);
	fclose(bin);

	selectVehicleWhere(vData,"JC314",matchVehiclePrefix);

	FILE* lcsv = openFile(argv[2],"rb");
	LineData* lData = readLineCsv(lcsv);
	fclose(lcsv);

	displayLine(&lData->registers[1]);
	displayLine(&lData->registers[2]);

	selectLineWhere(lData,(void*)"S",matchLineAcceptCard);
	
	FILE* bin2 = openFile("lteste","wb");
	writeLineBinary(lData,bin2);
	fclose(bin2);

	freeLineData(lData);
	freeVehicleData(vData);

	return 0;
}