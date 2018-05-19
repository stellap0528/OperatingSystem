/*
 * Sophie Brusniak, Joseph Knierman, Stella Park
 * April 25, 2018
 * Team 35
 */

int commands(char* commanLine);
int main()
{
	char shellPrompt[7];
	char errorResponse[16];
	char userInput[80];

	shellPrompt[0] = 'S';
	shellPrompt[1] = 'H';
	shellPrompt[2] = 'E';
	shellPrompt[3] = 'L';
	shellPrompt[4] = 'L';
	shellPrompt[5] = '>';
	shellPrompt[6] = ' ';
	shellPrompt[7] = '\0';

	errorResponse[0] = '\n';
	errorResponse[1] = '\r';
	errorResponse[2] = 'B';
	errorResponse[3] = 'A';
	errorResponse[4] = 'D';
	errorResponse[5] = ' ';
	errorResponse[6] = 'C';
	errorResponse[7] = 'O';
	errorResponse[8] = 'M';
	errorResponse[9] = 'M';
	errorResponse[10] = 'A';
	errorResponse[11] = 'N';
	errorResponse[12] = 'D';
	errorResponse[13] = '\n';
	errorResponse[14] = '\r';
	errorResponse[15] = '\0';

	while (1) {
		interrupt(0x21, 0, shellPrompt, 0, 0); 
		interrupt(0x21, 1, userInput, 0, 0); 
		if(commands(userInput) == 0){
			interrupt(0x21, 0, errorResponse, 0, 0);
		}
	}
}

int commands(char* commanLine){
	char printBuffer[13312];

	if(commanLine[0] =='t'){
		if(commanLine[1] == 'y'){
			if(commanLine[2] == 'p'){
				if(commanLine[3] == 'e'){
					interrupt(0x21, 3, commanLine + 5, printBuffer, 0);
					interrupt(0x21, 0, printBuffer, 0, 0); 
					return 1;
				}
			}
		}
	}
	if(commanLine[0] == 'e'){
		if(commanLine[1] == 'x'){
			if(commanLine[2] == 'e'){
				if(commanLine[3] == 'c'){
					if(commanLine[4] == 'u'){
						if(commanLine[5] == 't'){
							if(commanLine[6] == 'e'){
								interrupt(0x21, 4, commanLine + 8, 0x2000, 0);
								return 1;
							}
						}
					}
				}
			}
		}
	}
	return 0;

}