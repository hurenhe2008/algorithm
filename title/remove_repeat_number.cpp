
/*
@func
	remove_repeat_number - remove all repeat numbers and 
		adjust valid numbers to front.
@params
	array - the unsigned int array
	num - the array size
@return 
	the valid numbers of array
*/

unsigned remove_repeat_number(unsigned array[], unsigned num)
{
    unsigned cnt = 0;

    for (unsigned i = 0; i < num - 1; ++i) {
        if (0 == array[i]) {   /*special symbol*/
            continue;
        }

        for (unsigned j = i + 1; j < num; ++j) {
            if (array[j] == array[i]) {
                array[j] = 0;  /*record sepecial symbol*/
            }        
        }
    }
    
    for (unsigned i = 0; i < num; ++i) {
        if (0 == array[i]) {
            ++cnt;
        }
        else {
            array[i - cnt] = array[i];
        }
    }

    return num - cnt;  /*return left valid numbers*/
}