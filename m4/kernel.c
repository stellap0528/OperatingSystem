/*
 * Sophie Brusniak, Joseph Knierman, Stella Park
 * April 25, 2018
 * Team 35
 */

void printString(char* string);
void readString(char * line);
int div(int a, int b);
int mod(int a, int b);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void handleInterrupt21(int ax, int bx, int cx, int dx);
void readFile(char* fileName, char* buffer);
void executeProgram(char* name, int segment);
void terminate();
void deleteFile(char* fileName);
void writeFile(char* name, char* buffer, int sector);

int main()
{
	char buffer[13312]; 
	char buffer2[13312];  /* this is the maximum size of a file */
	makeInterrupt21();
	interrupt(0x21, 3,"messag\0", buffer, 0);  /* read the file into buffer */
	interrupt(0x21, 8,"here\0", buffer, 1);
	/* interrupt(0x21, 4, "tstprg\0", 0x2000, 0);*/
	interrupt(0x21, 4, "shell\0\0", 0x2000, 0);

	while(1);
}
void printString(char * string){
	char al;
	char ah = 0xe;
	int ax;
	int i;

	for(i = 0; string[i] != 0; i++){
 		al = string[i];
	 	ax = 0xe * 256 + al;
		interrupt(0x10, 0xe*256+al, 0, 0, 0);
	}
}
void readString(char * line){
	char ah =0x0;
	char al;
	int ax;
	char input;
	char newLine[3];
	int i = 0;
	while(i< 77){
		input = interrupt (0x16, 0, 0, 0, 0);
		if(input ==0xd){
			line[i]='\0';
			/*line[i+1]='\r';
			line[i+2]='\0';*/
			newLine[0] = '\n';
			newLine[1] = '\r';
			newLine[2] = '\0';
			printString(newLine);
			break;
		}
		else if(input == 0x8){
			line[i]='\0';
			i--;
			interrupt(0x10, 0xe*256+input, 0, 0, 0);
			interrupt(0x10, 0xe*256+' ', 0, 0, 0);
			interrupt(0x10, 0xe*256+input, 0, 0, 0);
		}else{
			line[i]=input;
			interrupt(0x10, 0xe*256+input, 0, 0, 0);
			i++;
			line[i]='\0';
		}
	}
}


int mod(int a, int b){
	while(a >= b){
  		a = a - b;
	}
	return a;
}
int div(int a, int b){
	int quotient = 0;
	while( (quotient  + 1)* b  <= a)
  		quotient = quotient + 1;
	return quotient;
}
void readSector(char *buffer, int sector){
	int ah=2;
	int al=1;
	int ax = ah*256+al;
	int ch = div(sector, 36);
	int cl = mod(sector,18)+1;
	int dh = mod(div(sector,18),2);
	int dl=0;
	int cx = ch*256 + cl;
	int dx = dh*256 + dl;
	interrupt(0x13, ax, buffer,cx,dx);
}

void writeSector(char *buffer, int sector){
	int ah=3;
	int al=1;
	int ax = ah*256+al;
	int ch = div(sector, 36);
	int cl = mod(sector,18)+1;
	int dh = mod(div(sector,18),2);
	int dl=0;
	int cx = ch*256 + cl;
	int dx = dh*256 + dl;
	interrupt(0x13, ax, buffer,cx,dx);
}

void readFile(char* fileName, char* buffer){
	char directoryBuffer[512];
	char * directory;
	int i;
	int j;
	int k;

	directory = directoryBuffer;
	readSector(directory, 2);
	for(i=0; i<16; i++){
		for(j=0; j<6; j++){
			if(directory[j] != fileName[j]){
				break;
			}
			if(j == 5){
				directory += 6;
				for(k=0; k<=10; k++){
					if(directory[k] == '\0'){
						return;
					}
					else{
						readSector(buffer, (int) directory[k]);
						buffer += 512;
					}
				}
				return;
			}
		}
		directory += 32;
	}
}

void executeProgram(char* name, int segment){
	char buffer[13312];
	int i;
	readFile(name, buffer);
	for(i = 0; i < 13312; i++){
		putInMemory(segment, i, buffer[i]);
	}
	launchProgram(segment);
}

void terminate(){
	char location[7];
	location[0] = 's';
	location[1] = 'h';
	location[2] = 'e';
	location[3] = 'l';
	location[4] = 'l';
	location[5] = '\0';
	location[6] = '\0';

	interrupt(0x21, 4, location, 0x2000, 0);
}

void deleteFile(char* fileName){
	char directoryBuffer[512];
	char mapBuffer[512];
	char * directory;
	int i;
	int j;
	int k;

	readSector(directoryBuffer, 2);
	readSector(mapBuffer, 1);
	directory = directoryBuffer;

	for(i=0; i<16; i++){
		for(j=0; j<6; j++){
			if(directory[j] != fileName[j]){
				break;
			}
			if(j == 5){
				directory += 6;
				for(k=0; k<=10; k++){
					if(directory[k] == '\0'){
						directoryBuffer[32*i] = '\0';
						writeSector(mapBuffer,1);
						writeSector(directoryBuffer, 2);
						return;
					}
					else{
						mapBuffer[(int)directory[k]] = '\0';
					}
				}
			}
		}
		directory += 32;
	}
}

void writeFile(char* name, char* buffer, int sector){
	char directoryBuffer[512];
	char mapBuffer[512];
	char * directory;
	int i;
	int j;
	int k;
	int found;
	found = 0;

	readSector(directoryBuffer, 2);
	readSector(mapBuffer, 1);
	directory = directoryBuffer;

	for(i=0; i<16; i++){
		if(directory[0] == '\0'){
			for(j = 0; j < 6 && (name[j]!='\0'); j++){
				directoryBuffer[(i * 32) + j] = name[j];
			}
			for(j; j<6; j++){
				directoryBuffer[(i * 32) + j] = '\0';
			}
			for(j = 0; j < 512; j++){
				if(found == sector){
					writeSector(mapBuffer, 1);
					writeSector(directoryBuffer, 2);
					return;
				}
				if(mapBuffer[j] == '\0'){
					mapBuffer[j] = 0xFF;
					for(k = 0; k < 10; k++){
						if(directoryBuffer[(i * 32) + 6 + k] == '\0'){
							directoryBuffer[(i * 32) + 6 + k] = j;
							writeSector(buffer, j);
							buffer += 512;
							found++;
							break;
						}
					}
				}
			}
			
		}
		directory += 32;
	}
}

void findDirectory(char * buffer){
	char directoryBuffer[512];
	char * directory;
	int i;
	int k;
	int count;
	count = 0;
	readSector(directoryBuffer, 2);
	directory = directoryBuffer;
	
	for(i=0; i<16; i++){
		if(directory[0] != '\0'){
			for(k = 0; k < 6; k++){
				if(directory[k]=='\0'){
					break;
				}
				buffer[count++]=directory[k];
			}
			buffer[count++]='\n';
			buffer[count++]='\r';
			
		}
		directory += 32;
	}

	buffer[count++]='\0';
}

void shellWriteFile(char * line){
	char ah =0x0;
	char al;
	int ax;
	char input;
	char newLine[3];
	int i = 0;
	int concurrent = 0;
	while(1){
		input = interrupt (0x16, 0, 0, 0, 0);
		if(input ==0xd){
			concurrent++;
			if(concurrent==2){
				line[i++]='\0';
				break;
			}
			/*line[i+1]='\r';
			line[i+2]='\0';*/

			interrupt(0x10, 0xe*256+'\n', 0, 0, 0);
			interrupt(0x10, 0xe*256+'\r', 0, 0, 0);
			line[i++]='\n';
			line[i++]='\r';
		}
		else if(input == 0x8){
			line[i]='\0';
			i--;
			interrupt(0x10, 0xe*256+input, 0, 0, 0);
			interrupt(0x10, 0xe*256+' ', 0, 0, 0);
			interrupt(0x10, 0xe*256+input, 0, 0, 0);
			concurrent=0;
		}else{
			line[i]=input;
			interrupt(0x10, 0xe*256+input, 0, 0, 0);
			i++;
			line[i]='\0';
			concurrent=0;
		}
	}
}

void handleInterrupt21(int ax, int bx, int cx, int dx){
	switch(ax){
		case 0:
			printString((char*) bx);
			break;
		case 1:
			readString((char*) bx);
			break;
		case 2:
			readSector((char*) bx, cx);
			break;
		case 3:
			readFile((char*) bx, (char*) cx);
			break;
		case 4:
			executeProgram((char*) bx, cx);
			break;
		case 5:
			terminate();
			break;
		case 6:
			writeSector((char*) bx, cx);
			break;
		case 7:
			deleteFile((char*) bx);
			terminate();
			break;
		case 8:
			writeFile((char*) bx, (char*) cx, dx);
			terminate();
			break;
		case 9:
			findDirectory((char*) bx);
			break;
		case 10:
			shellWriteFile((char*) bx);
		default:
			printString("Error\n\r\0");
	}
}
