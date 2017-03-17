
/*
@func
	remove_special_char - remove special char in string and 
		adjust valid chars to front.
@params
	array - the unsigned int array
	num - the array size
@return 
	the remove special char numbers
*/

unsigned remove_special_char(char *str, char ch)
{
	unsigned cnt = 0;
	
	while(*str) {
		if (ch == *str) {
			*str = '\0';   /*use '\0' instead ch, think about str's end is ch*/
			++cnt;
		}
		else {
			*(str - cnt) = *str;
		}
	}

	return cnt;
}