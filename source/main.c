#include "vehicle.h"
#include "line.h"

int main(int argc, char *argv[]){
	FILE* vcsv = fopen(argv[1],"rb");
	VehicleData* vData = readVehicleCsv(vcsv);
	fclose(vcsv);
	
	FILE* bin = fopen("teste","wb");
	writeVehicleBinary(vData,bin);
	fclose(bin);

	selectVehicleWhere(vData,"JC314",matchVehiclePrefix);

	FILE* lcsv = fopen(argv[2],"rb");
	LineData* lData = readLineCsv(lcsv);
	fclose(lcsv);

	displayLine(&lData->registers[1]);
	displayLine(&lData->registers[2]);
	
	FILE* bin2 = fopen("lteste","wb");
	writeLineBinary(lData,bin2);
	fclose(bin2);

	return 0;
}