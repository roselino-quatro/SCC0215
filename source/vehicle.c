#include "vehicle.h"

// Lê e parsea o CSV do arquivo e armazena em uma struct
VehicleData* readVehicleCsv(FILE* csv){
	if(csv == NULL) {
		printf("Falha no processamentodo arquivo.\n");
		return NULL;
	}

	VehicleData* vData = malloc(sizeof(VehicleData));

	// Initialize and read header from csv file
	vData->header.isStable = true;
	vData->header.regByteOff = 166;
	vData->header.regQty = 0;
	vData->header.regRemovedQty = 0;

	fscanf(csv,"%[^,],",vData->header.descPrefix);
	fscanf(csv,"%[^,],",vData->header.descData);
	fscanf(csv,"%[^,],",vData->header.descSeats);
	fscanf(csv,"%[^,],",vData->header.descLine);
	fscanf(csv,"%[^,],",vData->header.descModel);
	fscanf(csv,"%[^\n]\n",vData->header.descCategory);

	// While there are registers in the csv, allocate and process
	vData->regQty = 1;
	vData->registers = malloc(vData->regQty * sizeof(VehicleReg));
	int rpos = 0;
	while (fscanf(csv,"%[^,],",vData->registers[rpos].prefix) != EOF) {
		if (rpos+1 == vData->regQty){
			vData->regQty *= 2;
			vData->registers = realloc(vData->registers,vData->regQty * sizeof(VehicleReg));
		}

		VehicleReg* cReg = &vData->registers[rpos++];
		cReg->isPresent = (cReg->prefix[0] != '*')? true : false;
		
		fscanf(csv,"%[^,],",cReg->data);
		fscanf(csv,"%d,",&cReg->seatQty);
		fscanf(csv,"%d,",&cReg->lineCode);
		fscanf(csv,"%[^,],",cReg->model);
		cReg->modelSize = strlen(cReg->model);
		fscanf(csv,"%[^\n]\n",cReg->category);
		cReg->categorySize = strlen(cReg->category);
		cReg->regSize = 23 + cReg->modelSize + cReg->categorySize;
	}

	// Shrink registers array back to appropriate size
	vData->regQty = rpos;
	vData->registers = realloc(vData->registers,(vData->regQty) * sizeof(VehicleReg));
	return vData;
}

// Destructor que libera memoria alocada de uma struct VehicleData
bool freeVehicleData(VehicleData* vData) {
	if (vData == NULL) return false;

	free(vData->registers);
	free(vData);

	return true;
}

// Transfere os dados de uma VehicleData para um arquivo binario seguindo as regras passadas nas especificaçẽos
void writeVehicleBinary(VehicleData* vData,FILE* binDest){
	// Write header data - mark the file as unstable until end of write
	vData->header.isStable = false;
	fwrite(&vData->header.isStable,1,1,binDest);
	fwrite(&vData->header.regByteOff,8,1,binDest);
	fwrite(&vData->header.regQty,4,1,binDest);
	fwrite(&vData->header.regRemovedQty,4,1,binDest);
	fwrite(vData->header.descPrefix,1,18,binDest);
	fwrite(vData->header.descData,1,35,binDest);
	fwrite(vData->header.descSeats,1,42,binDest);
	fwrite(vData->header.descLine,1,26,binDest);
	fwrite(vData->header.descModel,1,17,binDest);
	fwrite(vData->header.descCategory,1,20,binDest);

	// Iterate through registers and write their data
	int regpos = 0;
	while (regpos < vData->regQty){
		VehicleReg* curReg = &vData->registers[regpos++];
		fwrite(&curReg->isPresent,1,1,binDest);
		fwrite(&curReg->regSize,4,1,binDest);
		fwrite(curReg->prefix,1,5,binDest);
		fwrite(curReg->data,1,10,binDest);
		fwrite(&curReg->seatQty,4,1,binDest);
		fwrite(&curReg->lineCode,4,1,binDest);
		fwrite(&curReg->modelSize,4,1,binDest);
		fwrite(curReg->model,1,curReg->modelSize,binDest);
		fwrite(&curReg->categorySize,4,1,binDest);
		fwrite(curReg->category,1,curReg->categorySize,binDest);
	}

	// Rewind and mark file as stable
	rewind(binDest);
	vData->header.isStable = true;
	fwrite(&vData->header.isStable,1,1,binDest);
}

// Imprime informações do registro de veiculo
void displayVehicle(VehicleReg* vReg) {
	printf("Prefixo do veiculo: %s\n",vReg->prefix);
	printf("Modelo do veiculo: %s\n",vReg->model);
	printf("Categoria do veiculo: %s\n",vReg->category);

	char* months[12] = {"janeiro","fevereiro","marco","abril","maio","junho","julho","agosto","setembro","outubro","novembro","dezembro"};
	
	int day = atoi(&vReg->data[8]);
	char* month = months[atoi(&vReg->data[5])-1];
	int year = atoi(&vReg->data[0]);

	printf("Data de entrada do veiculo na frota: %d de %s de %d\n",day,month,year);
	printf("Quantidade de lugares sentados disponiveis: %d\n\n",vReg->seatQty);
}


// Familia de funções para a selectVehicleWhere
bool matchVehiclePrefix(VehicleReg* vReg,void* prefix) {
	if (vReg->prefix[0] == '\0') return false;
	return (strncmp(vReg->prefix,(char*)prefix,5) == 0)? true : false;
};

bool matchVehicleData(VehicleReg* vReg,void* data) {
	if (vReg->data[0] == '\0') return false;
	return (strncmp(vReg->data,(char*)data,10) == 0)? true : false;
};

bool matchVehicleSeatQty(VehicleReg* vReg,void* seatQty) {
	if (vReg->seatQty == -1) return false;
	return (vReg->seatQty == *(int*)seatQty)? true : false;
};

bool matchVehicleModel(VehicleReg* vReg,void* model) {
	if (vReg->model[0] == '\0') return false;
	return (strcmp(vReg->model,(char*)model) == 0)? true : false;
};

bool matchVehicleCategory(VehicleReg* vReg,void* category) {
	if (vReg->category[0] == '\0') return false;
	return (strcmp(vReg->category,(char*)category) == 0)? true : false;
};

// Imprime os matchs de uma comparação dentro de uma struct Data
void selectVehicleWhere(VehicleData* vData,void* key,bool (*match)(VehicleReg*,void*)) {
	if(vData == NULL) {
		printf("Falha no processamento do arquivo.\n");
		return;
	}

	bool anyMatched = false;
	int regPos = 0;
	while (regPos < vData->regQty) {
		VehicleReg* curReg = &vData->registers[regPos++];
		if (!curReg->isPresent) continue;

		if (match(curReg,key)) {
			displayVehicle(curReg);
			anyMatched = true;
		}
	}

	if (!anyMatched) printf("Registro inexistente\n");
}

// Imprime todos os registros não removidos de uma struct
void selectVehicle(VehicleData* vData) {
	if(vData == NULL) {
		printf("Falha no processamento do arquivo.\n");
		return;
	}

	bool anyMatched = false;
	int regPos = 0;
	while (regPos < vData->regQty) {
		VehicleReg* curReg = &vData->registers[regPos++];
		if (curReg->isPresent) {
			displayVehicle(curReg);
			anyMatched = true;
		}
	}

	if (!anyMatched) printf("Registro inexistente\n");
}