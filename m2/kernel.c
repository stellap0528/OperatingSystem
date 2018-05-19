/*
 * Sophie Brusniak, Joseph Knierman, Stella Park
 * April 19, 2018
 * Team 35
 */

void printString(char* string);
void readString(char * line);
int div(int a, int b);
int mod(int a, int b);
void readSector(char *buffer, int sector);
void handleInterrupt21(int ax, int bx, int cx, int dx);
int main()
{
	char line[80];
	char buffer[512];
	char lineSecond[80];
	printString("Enter a line: \n\r\0");
	readString(line);
	printString(line);
	readSector(buffer, 30);
	printString(buffer);
	makeInterrupt21();
	interrupt(0x21,1,lineSecond,0,0);
	interrupt(0x21,0,lineSecond,0,0);
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
	int i = 0;
	while(i< 77){
		input = interrupt (0x16, 0, 0, 0, 0);
		if(input ==0xd){
			line[i]='\n';
			line[i+1]='\r';
			line[i+2]='\0';
			printString("\n\r\0");
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
		default:
			printString("Error\n\r\0");
	}
}
