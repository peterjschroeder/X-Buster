#include "q_shared.h"


/*
 * Function to skip over the leading spaces of a string.
 */
void skip_spaces(char **string)
{
    for (; **string && isspace(**string); (*string)++);
}

/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes. Doesn't ignore fill words */
char *any_one_arg(char *argument, char *first_arg)
{
    skip_spaces(&argument);

    while (*argument && !isspace(*argument))
    {
        *(first_arg++) = *argument;
        argument++;
    }

    *first_arg = '\0';

    return (argument);
}

/*
 * Return first space-delimited token in arg1; remainder of string in arg2.
 *
 * NOTE: Requires sizeof(arg2) >= sizeof(string)
 */
void half_chop(char *string, char *arg1, char *arg2)
{
    char *temp;

    temp = any_one_arg(string, arg1);
    skip_spaces(&temp);
    strcpy(arg2, temp);
}

/*
 * Return true if an argument is completely numeric.
 */
qboolean is_number( char *arg )
{
    if ( *arg == '\0' )
        return qfalse;

    if ( *arg == '+' || *arg == '-' )
        arg++;

    for ( ; *arg != '\0'; arg++ )
    {
        if ( !isdigit( *arg ) )
            return qfalse;
    }

    return qtrue;
}
