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
void executeProgram(char* name, int flag);
void terminate();
void deleteFile(char* fileName);
void writeFile(char* name, char* buffer, int sector);
void handleTimerInterrupt(int segment, int sp);
void killProcess(int id);
void stopWait(int id);
void quit();
void changeBackGroundColor(char* color);

struct entry {
	int sp;
	int active;
	int waiting;
	int waitingOn;
};

int currentProcess = 0;
struct entry processes[8];


int main()
{ 
	int i;
	for(i  = 0; i < 8; i++){
		processes[i].active = 0;
		processes[i].sp = 0xff00;
		processes[i].waiting = 0;
		processes[i].waitingOn = 0;
	}
	makeInterrupt21();
	makeTimerInterrupt();

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
			if(i != 0){
				line[i]='\0';
				i--;
				interrupt(0x10, 0xe*256+input, 0, 0, 0);
				interrupt(0x10, 0xe*256+' ', 0, 0, 0);
				interrupt(0x10, 0xe*256+input, 0, 0, 0);
			}
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

void executeProgram(char* name, int flag){
	char buffer[13312];
	int i;
	int j;
	int segment;
	readFile(name, buffer);	

	setKernelDataSegment();
	for(i = 0; i < 8; i++){
		if(processes[i].active == 0){
			processes[i].active = 1;
			processes[i].sp = 0xff00;
			currentProcess = i;
			break;
		}
	}
	if(flag == 1){
		processes[0].waiting = 1;
		processes[0].waitingOn = i;
	}

	segment = (2 + i) * 0x1000;
	restoreDataSegment();
	for(j = 0; j < 13312; j++){
		putInMemory(segment, j, buffer[j]);
	}
	initializeProgram(segment);
}

void terminate(){
	/*char location[7];
	location[0] = 's';
	location[1] = 'h';
	location[2] = 'e';
	location[3] = 'l';
	location[4] = 'l';
	location[5] = '\0';
	location[6] = '\0';

	interrupt(0x21, 4, location, 0x2000, 0);*/
	setKernelDataSegment();
	processes[currentProcess].active = 0;
	stopWait(currentProcess);
	while(1);

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

void handleTimerInterrupt(int segment, int sp){
	char ticString[6];
	int i;
	int newSegment;
	int moveTo;
	int index;
	ticString[0] = 'T';
	ticString[1] = 'i';
	ticString[2] = 'c';
	ticString[3] = '\n';
	ticString[4] = '\r';
	ticString[5] = '\0';

	newSegment = (segment/0x1000)-2;

	setKernelDataSegment();
	if(newSegment == currentProcess){
		processes[newSegment].sp = sp;
	}

	for(i=1; i<=8; i++){
		index = mod((i + currentProcess), 8);
		if(processes[index].active == 1 && processes[index].waiting == 0){
			currentProcess = index;
			segment = (2 + index) * 0x1000;
			sp = processes[index].sp;
			break;
		}
	}


	restoreDataSegment();
	returnFromTimer(segment, sp);
}

void killProcess(int id){
	setKernelDataSegment();
	processes[id].active = 0;
	stopWait(id);
	restoreDataSegment();
}

void stopWait(int id){
	int i;
	setKernelDataSegment();
	for (i = 0; i < 8; i++){
		if(processes[i].waitingOn == id){
			processes[i].waiting = 0;
		}
	}
	restoreDataSegment();
}

void quit(){
	interrupt(0x15, 0x5307, 0x0001, 0x0003, 0);
}

void changeBackGroundColor(char* color){
	int i;
	i = getColor(color);
	interrupt(0x10, 0xB*256, i, 0, 0);
}

void changeFontColor(char* buffer) {
	int i;
	i = getColor(buffer);
	interrupt(0x10, 0x7*256, 256*i, 0, 256*25+80);
}

int getColor(char* color){
	int i = 0;
	int j = 0;
	
	while (color[i] != '\0') {
    	if (i >= 2) 
			return -1;
    	j = j * 10 + (color[i++]-0x30);
 	}

	if (j > 15 || j < 0) 
		return -1;

	return j;
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
			executeProgram((char*) bx, 0);
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
			break;
		case 9:
			findDirectory((char*) bx);
			break;
		case 10:
			shellWriteFile((char*) bx);
			break;
		case 11:
			killProcess(bx);
			break;
		case 12:
			executeProgram((char*) bx, 1);
			break;
		case 13:
			quit();
			break;
		case 14:
			changeBackGroundColor((char*) bx);
			break;
		case 15:
			changeFontColor((char*) bx);
			break;
		default:
			printString("Error\n\r\0");
	}
}
