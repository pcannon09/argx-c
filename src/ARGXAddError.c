#include <stdlib.h>
#include <string.h>

#include "../inc/ARGXAddError.h"
#include "../inc/types.h"

static char *stringDuplicate(const char *str)
{
    if (!str) return NULL;
    size_t len = strlen(str) + 1;
    char *dup = malloc(len);
    if (dup)
	{
        memcpy(dup, str, len);
    }
    return dup;
}

ArgxcError argxcCreateErrorSimple(const char *msg, const char *help)
{
    ArgxcError error = {0};

    error.type = stringDuplicate("error");
    error.error = stringDuplicate(msg);
    error.help = stringDuplicate(help);
    error.code = 1; // Default error code

    return error;
}

const char *argxcGetErrorMessage(const ArgxcError *error)
{
    return error ? error->error : NULL;
}

const char *argxcGetErrorHelp(const ArgxcError *error)
{
    return error ? error->help : NULL;
}

int argxcGetErrorCode(const ArgxcError *error)
{
    return error ? error->code : 0;
}

ArgxcErrorPair argxcGetErrorPair(const ArgxcError *error)
{
    ArgxcErrorPair pair = {NULL, NULL};

    if (error)
	{
        pair.error = error->error;
        pair.help = error->help;
    }

    return pair;
}

