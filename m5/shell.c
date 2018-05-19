/*
 * Sophie Brusniak, Joseph Knierman, Stella Park
 * April 25, 2018
 * Team 35
 */
void printHelper(char *buffer);
int commands(char* commandLine);
int main()
{
	char shellPrompt[7];
	char errorResponse[16];
	char userInput[80];

	enableInterrupts();

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

int commands(char* commandLine){
	char message[8];
	char newLine[3];
	char printBuffer[13312];
	char emptyLine[3];
	int i;
	

	message[0] = 'h';
	message[1] = 'e';
	message[2] = 'l';
	message[3] = 'p';
	message[4] = 'm';
	message[5] = 'e';
	message[6] = 's';
	message[7] = '\0';


	emptyLine[0]= '\r';
	emptyLine[1] = '\n';
	emptyLine[2] = '\0';

	if(commandLine[0] =='t'){
		if(commandLine[1] == 'y'){
			if(commandLine[2] == 'p'){
				if(commandLine[3] == 'e'){
					interrupt(0x21, 3, commandLine + 5, printBuffer, 0);
					interrupt(0x21, 0, printBuffer, 0, 0); 
					return 1;
				}
			}
		}
	}

	if(commandLine[0] =='q'){
		if(commandLine[1] == 'u'){
			if(commandLine[2] == 'i'){
				if(commandLine[3] == 't'){
					interrupt(0x21, 13, 0, 0, 0);
					return 1;
				}
			}
		}
	}
	
	if(commandLine[0] == 'e'){
		if(commandLine[1] == 'x'){
			if(commandLine[2] == 'e'){
				if(commandLine[3] == 'c'){
					if(commandLine[4] == 'u'){
						if(commandLine[5] == 't'){
							if(commandLine[6] == 'e'){
								interrupt(0x21, 4, commandLine + 8, 0x2000, 0);
								return 1;
							}
						}
					}
				}
			}
		}
	}

	if(commandLine[0] == 'd'){
		if(commandLine[1] == 'e'){
			if(commandLine[2] == 'l'){
				if(commandLine[3] == 'e'){
					if(commandLine[4] == 't'){
						if(commandLine[5] == 'e'){
							interrupt(0x21, 7, commandLine + 7, 0x2000, 0);
							interrupt(0x21, 0, commandLine, 0, 0);
							return 1;
						}
					}
				}
			}
		}
	}

	if(commandLine[0] == 'c'){
		if(commandLine[1] == 'o'){
			if(commandLine[2] == 'p'){
				if(commandLine[3] == 'y'){
					interrupt(0x21, 3, commandLine + 5, printBuffer, 0);
					interrupt(0x21, 8, commandLine + 12, printBuffer, 3);
					return 1;
				}
			}
		}
	}

	if(commandLine[0] == 'k'){
		if(commandLine[1] == 'i'){
			if(commandLine[2] == 'l'){
				if(commandLine[3] == 'l'){
					interrupt(0x21, 11, (int)(commandLine[5] - 0x30), 0, 0);
					return 1;
				}
			}
		}
	}

	if(commandLine[0] == 'd'){
		if(commandLine[1] == 'i'){
			if(commandLine[2] == 'r'){
				interrupt(0x21, 9, printBuffer, 0, 0);
				interrupt(0x21, 0, printBuffer, 0, 0);
				return 1;
			}
		}
	}

	if(commandLine[0] == 'c'){
		if(commandLine[1] == 'r'){
			if(commandLine[2] == 'e'){
				if(commandLine[3] == 'a'){
					if(commandLine[4] == 't'){
						if(commandLine[5] == 'e'){
							if(commandLine[6] == ' '){
								if(commandLine[7] == 'f'){
									if(commandLine[8] == 'i'){
										if(commandLine[9] == 'l'){
											if(commandLine[10] == 'e'){
													interrupt(0x21, 10, printBuffer, 0, 0);
													interrupt(0x21, 8, commandLine + 12, printBuffer, 10);
													return 1;
												
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if(commandLine[0] == 'e'){
		if(commandLine[1] == 'x'){
			if(commandLine[2] == 'e'){
				if(commandLine[3] == 'c'){
					if(commandLine[4] == 'f'){
						if(commandLine[5] == 'o'){
							if(commandLine[6] == 'r'){
								if(commandLine[7] == 'e'){
									if(commandLine[8] == 'g'){
										if(commandLine[9] == 'r'){
											if(commandLine[10] == 'o'){
												if(commandLine[11] == 'u'){
													if(commandLine[12] == 'n'){
														if(commandLine[13] == 'd'){
															interrupt(0x21, 12, commandLine + 15, 0, 0);
															return 1;

														}
													}
												}												
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if(commandLine[0] == 'h'){
		if(commandLine[1] == 'e'){
			if(commandLine[2] == 'l'){
				if(commandLine[3] == 'p'){
					interrupt(0x21, 3, message, printBuffer, 0);
					printHelper(printBuffer);
					return 1;
				}
			}
		}
	}

	if(commandLine[0] == 'b'){
		if(commandLine[1] == 'g'){
			if(commandLine[2] == 'c'){
				if(commandLine[3] == 'o'){
					if(commandLine[4] == 'l'){
						if(commandLine[5] == 'o'){
							if(commandLine[6] == 'r'){
								interrupt(0x21, 14, commandLine + 8, 0, 0);
								return 1;
							}
						}
					}
				}
			}
		}
	}

	if(commandLine[0] == 't'){
		if(commandLine[1] == 'x'){
			if(commandLine[2] == 't'){
				if(commandLine[3] == 'c'){
					if(commandLine[4] == 'o'){
						if(commandLine[5] == 'l'){
							if(commandLine[6] == 'o'){
								if(commandLine[7] == 'r'){
									interrupt(0x21, 15, commandLine + 9, 0, 0);
									return 1;
								}
							}
						}
					}
				}
			}
		}
	}
	if(commandLine[0] == 'c'){
		if(commandLine[1] == 'l'){
			if(commandLine[2] == 'e'){
				if(commandLine[3] == 'a'){
					if(commandLine[4] == 'r'){
						for (i = 0; i < 24; i++) {
        					interrupt(0x21, 0, emptyLine, 0, 0);
      					}
      					interrupt(0x10, 256*2+0, 0, 0, 0);
						return 1;
					}
				}
			}
		}
	}
	return 0;
}

void printHelper(char *buffer){
int i = 0;
  while (buffer[i] != '\0') {

    if (buffer[i] == '\n') {
      interrupt(0x10, 0xe*256+'\r', 0, 0, 0);
    }
      interrupt(0x10, 0xe*256+buffer[i++], 0, 0, 0);
    
  }
  return;
}