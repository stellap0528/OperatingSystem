int main()
{
	char al = 'Q';
	char ah = 0xe;
	int ax = ah * 256 + al;
	interrupt(0x10, ax, 0, 0, 0);

	while(1) {}
}