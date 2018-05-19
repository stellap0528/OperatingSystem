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
void handleInterrupt21(int ax, int bx, int cx, int dx);
void readFile(char* fileName, char* buffer);
void executeProgram(char* name, int segment);
void terminate();
int main()
{
	char buffer[13312];  /* this is the maximum size of a file */
	makeInterrupt21();

	interrupt(0x21, 3,"messag\0", buffer, 0);  /* read the file into buffer */
	interrupt(0x21, 0, buffer, 0, 0);       /* print out the file */
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
		default:
			printString("Error\n\r\0");
	}
}
