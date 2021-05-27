#include "vehicle.h"

// Lê e parsea o CSV do arquivo e armazena em uma struct
VehicleData* readVehicleCsv(FILE* csv){
	if(csv == NULL) {
		printf("Falha no processamentodo arquivo.\n");
		return NULL;
	}

	VehicleData* vData = malloc(sizeof(VehicleData));

	// Initialize and read header from csv file
	vData->header.isStable = '0';
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
		if (cReg->prefix[0] != '*') {
			cReg->isPresent = true;
			vData->header.regQty++;
		} else {
			cReg->isPresent = false;
			vData->header.regRemovedQty++;
		}
		
		fscanf(csv,"%[^,],",cReg->data);
		if(strncmp(cReg->data,"NULO",4) == 0){
			memcpy(cReg->data,"@@@@@@@@@@",10);
		}

		// Since the invalid value is a string NULO, setting the field to -1
		// effectively makes it the "default" value when the invalid is read
		cReg->seatQty = -1;
		fscanf(csv,"%d,",&cReg->seatQty);

		cReg->lineCode = -1;
		fscanf(csv,"%d,",&cReg->lineCode);

		fscanf(csv,"%[^,],",cReg->model);
		cReg->modelSize = strlen(cReg->model);
		if(strncmp(cReg->model,"NULO",4) == 0){
			cReg->model[0] = '\0';
			cReg->modelSize = 0;
		}

		fscanf(csv,"%[^\n]\n",cReg->category);
		cReg->categorySize = strlen(cReg->category);
		if(strncmp(cReg->category,"NULO",4) == 0){
			cReg->category[0] = '\0';
			cReg->categorySize = 0;
		}

		cReg->regSize = 23 + cReg->modelSize + cReg->categorySize;
		vData->header.regByteOff += cReg->regSize;
	}

	// Shrink registers array back to appropriate size
	vData->regQty = rpos;
	vData->registers = realloc(vData->registers,(vData->regQty) * sizeof(VehicleReg));
	return vData;
}

VehicleData* readVehicleBinary(FILE* bin){
	if(bin == NULL){
		printf("Falha no processamento do arquivo\n");
		return NULL;
	} 

	VehicleData* vData = malloc(sizeof(VehicleData));

	// Read header data - mark the file as unstable until end of read
	char header[175];
	fread(header,sizeof(char),175,bin);

	vData->header.isStable = '0';	// Start of operation - mark file as unstable
	memcpy(&vData->header.regByteOff,&header[1],sizeof(long));
	memcpy(&vData->header.regQty,&header[9],sizeof(int));
	memcpy(&vData->header.regRemovedQty,&header[13],sizeof(int));
	memcpy(&vData->header.descPrefix,&header[17],18*sizeof(char));
	memcpy(&vData->header.descData,&header[35],35*sizeof(char));
	memcpy(&vData->header.descSeats,&header[70],42*sizeof(char));
	memcpy(&vData->header.descLine,&header[112],26*sizeof(char));
	memcpy(&vData->header.descModel,&header[138],17*sizeof(char));
	memcpy(&vData->header.descCategory,&header[155],20*sizeof(char));

	vData->regQty = 1;
	vData->registers = malloc(vData->regQty * sizeof(VehicleReg));
	int rpos = 0;
	char regBuffer[32];
	while (fread(regBuffer,sizeof(char),32,bin) != 0) {
		if (rpos+1 == vData->regQty){
			vData->regQty *= 2;
			vData->registers = realloc(vData->registers,vData->regQty * sizeof(VehicleReg));
		}

		VehicleReg* cReg = &vData->registers[rpos++];

		memcpy(&cReg->isPresent,&regBuffer[0],sizeof(char));
		memcpy(&cReg->regSize,&regBuffer[1],sizeof(int));
		memcpy(cReg->prefix,&regBuffer[5],5*sizeof(char));
		memcpy(cReg->data,&regBuffer[10],10*sizeof(char));
		memcpy(&cReg->seatQty,&regBuffer[20],sizeof(int));
		memcpy(&cReg->lineCode,&regBuffer[24],sizeof(int));

		memcpy(&cReg->modelSize,&regBuffer[28],sizeof(int));
		if(cReg->modelSize != 0){
			fread(cReg->model,sizeof(char),cReg->modelSize,bin);
		}
		
		fread(&(cReg->categorySize),sizeof(int),1,bin);
		if(cReg->categorySize != 0){
			fread(cReg->category,sizeof(char),cReg->categorySize,bin);
		}
	}
	// Shrink registers to appropriate size
	vData->registers = realloc(vData->registers,(vData->regQty) * sizeof(VehicleReg));

	// Rewind and mark file as stable
	vData->header.isStable = true;
	vData->regQty = rpos;
	
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
// TODO: maybe replace header write with intermediate buffer then fwrite the buffer?
void writeVehicleBinary(VehicleData* vData,FILE* binDest){
	// Write header data - mark the file as unstable until end of write
	vData->header.isStable = false;
	fwrite(&vData->header.isStable,sizeof(char),1,binDest);
	fwrite(&vData->header.regByteOff,sizeof(long),1,binDest);
	fwrite(&vData->header.regQty,sizeof(int),1,binDest);
	fwrite(&vData->header.regRemovedQty,sizeof(int),1,binDest);
	fwrite(vData->header.descPrefix,sizeof(char),18,binDest);
	fwrite(vData->header.descData,sizeof(char),35,binDest);
	fwrite(vData->header.descSeats,sizeof(char),42,binDest);
	fwrite(vData->header.descLine,sizeof(char),26,binDest);
	fwrite(vData->header.descModel,sizeof(char),17,binDest);
	fwrite(vData->header.descCategory,sizeof(char),20,binDest);

	// Iterate through registers and write their data
	int regpos = 0;
	while (regpos < vData->regQty){
		VehicleReg* curReg = &vData->registers[regpos++];
		fwrite(&curReg->isPresent,sizeof(char),1,binDest);
		fwrite(&curReg->regSize,sizeof(int),1,binDest);
		fwrite(&curReg->prefix,sizeof(char),5,binDest);

		if(curReg->data[0] != '\0'){
			fwrite(curReg->data,sizeof(char),10,binDest);
		} else {
			fputs("\0@@@@@@@@@",binDest);
		}
		
		fwrite(&curReg->seatQty,sizeof(int),1,binDest);
		fwrite(&curReg->lineCode,sizeof(int),1,binDest);
		
		fwrite(&curReg->modelSize,sizeof(int),1,binDest);
		if (curReg->modelSize != 0){
			fwrite(curReg->model,sizeof(char),curReg->modelSize,binDest);
		}
		
		fwrite(&curReg->categorySize,sizeof(int),1,binDest);
		if (curReg->categorySize != 0){
			fwrite(curReg->category,sizeof(char),curReg->categorySize,binDest);
		}
	}

	// Rewind and mark file as stable
	rewind(binDest);
	vData->header.isStable = true;
	fwrite(&vData->header.isStable,sizeof(char),1,binDest);
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
