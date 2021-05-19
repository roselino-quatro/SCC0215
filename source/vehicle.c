#include "vehicle.h"

// FIXME: initial implementation not working
VehicleData readVehicleCsv(FILE* csv){
	VehicleData* vData = malloc(sizeof(VehicleData));

	// Initialize and read header from csv file
	vData->header.isStable = false;
	vData->header.regByteOff = 166;
	vData->header.regQty = 0;
	vData->header.regRemovedQty = 0;

	fscanf(csv,"%[^,]",vData->header.descPrefix);
	fscanf(csv,"%[^,]",vData->header.descData);
	fscanf(csv,"%[^,]",vData->header.descSeats);
	fscanf(csv,"%[^,]",vData->header.descLine);
	fscanf(csv,"%[^,]",vData->header.descModel);
	fscanf(csv,"%[^,]",vData->header.descCategory);

	// Start register read loop and read registers from csv file
	// TODO: Realloc registers array inside loop

	vData->regQty = 1;
	vData->registers = malloc(vData->regQty * sizeof(VehicleReg));
	int rpos = 0;
	do{
		VehicleReg cReg = vData->registers[rpos];
		fscanf(csv,"%[^,]",vData->header.descPrefix);
		fscanf(csv,"%[^,]",cReg.prefix);
		fscanf(csv,"%[^,]",cReg.data);
		fscanf(csv,"%[^,]",cReg.seatQty);
		fscanf(csv,"%[^,]",cReg.lineCode);
		fscanf(csv,"%[^,]",cReg.model);
		fscanf(csv,"%[^,]",cReg.category);

		cReg.modelSize = strlen(cReg.model);
		cReg.categorySize = strlen(cReg.categorySize);
	} while(!feof(csv));
}