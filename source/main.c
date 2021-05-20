#include "vehicle.h"
#include "line.h"

int main(int argc, char *argv[]){
	FILE* lcsv = fopen(argv[1],"rb");
	LineData* lData = readLineCsv(lcsv);
	fclose(lcsv);
	
	FILE* bin = fopen("lteste","wb");
	writeLineBinary(lData,bin);
	fclose(bin);

	return 0;
}