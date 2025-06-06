/* src/argxc.c
 * Owned and created by: pcannon09
 * Converted to C API
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "../inc/Argx.h"
#include "../inc/types.h"

// Internal structure definition
struct Argxc {
    char *id;
    char **mainArgs;
    size_t mainArgsCount;
    unsigned int mainArgc;
    ArgxcOptions *options;
    size_t optionsCount;
    size_t optionsCapacity;
};

// Static helper functions
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

static void freeStringArray(char **array, size_t count)
{
    if (!array) return;
    for (size_t i = 0; i < count; i++)
	{
        if (array[i]) free(array[i]);
    }

	if (array)
	{ free(array); array = NULL; }
}

static void freeOptionsArray(ArgxcOptions *options, size_t count)
{
    if (!options) return;

    for (size_t i = 0; i < count; i++)
	{
        if (&options[i]) argxcFreeOption(&options[i]);
    }

	if (options)
	{ free(options); options = NULL; }
}

// Constructor/Destructor implementations
Argxc *argxcCreate(const char *id, int argc, char *argv[])
{
    Argxc *argxc = malloc(sizeof(Argxc));
    if (!argxc) return NULL;

    argxc->id = stringDuplicate(id);
    argxc->mainArgc = argc;
    argxc->mainArgsCount = argc;
    argxc->mainArgs = malloc(argc * sizeof(char*));
    argxc->optionsCount = 0;
    argxc->optionsCapacity = 10;
    argxc->options = malloc(argxc->optionsCapacity * sizeof(ArgxcOptions));

    if (!argxc->mainArgs || !argxc->options)
	{
        argxcDestroy(argxc);
        return NULL;
    }

    for (int i = 0; i < argc; i++)
	{
        argxc->mainArgs[i] = stringDuplicate(argv[i]);
    }

    return argxc;
}

Argxc *argxcCreateDefault(void)
{
    Argxc *argxc = malloc(sizeof(Argxc));
    if (!argxc) return NULL;

    argxc->id = NULL;
    argxc->mainArgs = NULL;
    argxc->mainArgsCount = 0;
    argxc->mainArgc = 0;
    argxc->optionsCount = 0;
    argxc->optionsCapacity = 10;
    argxc->options = malloc(argxc->optionsCapacity * sizeof(ArgxcOptions));

    if (!argxc->options)
	{
        free(argxc); argxc = NULL;
        return NULL;
    }

    return argxc;
}

void argxcDestroy(Argxc *argxc)
{
    if (!argxc) return;

    free(argxc->id); argxc->id = NULL;
    if (argxc->mainArgs) freeStringArray(argxc->mainArgs, argxc->mainArgsCount);
    if (argxc->options) freeOptionsArray(argxc->options, argxc->optionsCount);
    free(argxc); argxc = NULL;
}

// Core functionality implementations
void argxcAddOption(Argxc *argxc, ArgxcOptions option)
{
    if (!argxc) return;

    if (argxc->optionsCount >= argxc->optionsCapacity)
	{
        argxc->optionsCapacity *= 2;
        argxc->options = realloc(argxc->options, argxc->optionsCapacity * sizeof(ArgxcOptions));
        if (!argxc->options) return;
    }

    argxc->options[argxc->optionsCount++] = option;
}

void argxcAddSubOption(ArgxcOptions *parent, ArgxcOptions subOption)
{
    if (!parent) return;

    if (parent->subParamsCount >= parent->subParamsCapacity)
	{
        size_t newCapacity = parent->subParamsCapacity == 0 ? 2 : parent->subParamsCapacity * 2;
        ArgxcOptions *newSubParams = realloc(parent->subParams, newCapacity * sizeof(ArgxcOptions));
        if (!newSubParams) return;

        parent->subParams = newSubParams;
        parent->subParamsCapacity = newCapacity;
    }

    parent->subParams[parent->subParamsCount++] = subOption;
}

int argxcFindParam(Argxc *argxc, const char *id)
{
    if (!argxc || !id) return -1;

    // First check if it's a main parameter
    for (size_t i = 0; i < argxc->optionsCount; i++)
	{
        if (argxc->options[i].id && strcmp(argxc->options[i].id, id) == 0)
		{
            // Check if this main parameter exists in arguments
            for (size_t j = 0; j < argxc->mainArgsCount; j++)
			{
                if ((argxc->options[i].param && strcmp(argxc->mainArgs[j], argxc->options[i].param) == 0) ||
                    	(argxc->options[i].sparam && strcmp(argxc->mainArgs[j], argxc->options[i].sparam) == 0))
				{
                    return (int)i;
                }
            }
        }
    }

    // Then look for sub-parameters
    for (size_t i = 0; i < argxc->optionsCount; i++)
	{
        ArgxcOptions *opt = &argxc->options[i];

        // Check if the parent option exists in the arguments
        bool parentExists = false;
        for (size_t j = 0; j < argxc->mainArgsCount; j++)
		{
            if ((opt->param && strcmp(argxc->mainArgs[j], opt->param) == 0) ||
                	(opt->sparam && strcmp(argxc->mainArgs[j], opt->sparam) == 0))
			{
                parentExists = true;
                break;
            }
        }

        if (parentExists)
		{
            // Find the index of the requested sub-parameter
            for (size_t j = 0; j < opt->subParamsCount; j++)
			{
                if (opt->subParams[j].id && strcmp(opt->subParams[j].id, id) == 0)
				{
                    return (int)j;
                }
            }
        }
    }

    return -1; // Not found
}

bool argxcParamExists(Argxc *argxc, const char *id)
{
    return argxcFindParam(argxc, id) >= 0;
}

ArgxcParam argxcGetParam(Argxc *argxc, const char *id)
{
    ArgxcParam result = {false, NULL, 0};

    if (!argxc || !id || argxc->mainArgc <= 1)
	{
        return result;
    }

    // First, check if this is a top-level option
    for (size_t i = 0; i < argxc->optionsCount; i++)
	{
        ArgxcOptions *opt = &argxc->options[i];

        if (opt->id && strcmp(opt->id, id) == 0)
		{
            // Find the position of the main option in arguments
            int mainOptionPos = -1;

            for (size_t j = 0; j < argxc->mainArgsCount; j++)
			{
                if ((opt->param && strcmp(argxc->mainArgs[j], opt->param) == 0) ||
                    	(opt->sparam && strcmp(argxc->mainArgs[j], opt->sparam) == 0))
				{
                    result.exists = true;
                    mainOptionPos = j;
                    break;
                }
            }

            if (result.exists)
			{
                if (opt->hasSubParams || opt->hasAnySubParams)
				{
                    // Allocate memory for sub-parameter existence array
                    result.subExistsCount = opt->subParamsCount;
                    result.subExists = malloc(result.subExistsCount * sizeof(bool));

                    if (result.subExists)
					{
                        // Check each sub-parameter
                        for (size_t j = 0; j < opt->subParamsCount; j++)
						{
                            bool subMatched = false;
                            ArgxcOptions *sub = &opt->subParams[j];

                            // Look for sub-parameters after the main option
                            for (size_t k = mainOptionPos + 1; k < argxc->mainArgsCount; k++)
							{
                                if ((sub->param && strcmp(argxc->mainArgs[k], sub->param) == 0) ||
                                    	(sub->sparam && strcmp(argxc->mainArgs[k], sub->sparam) == 0))
								{
                                    subMatched = true;
                                    break;
                                }
                            }

                            result.subExists[j] = subMatched;
                        }
                    }
                }

                return result;
            }
        }
    }

    // If not found as top-level, check if it's a sub-parameter
    for (size_t i = 0; i < argxc->optionsCount; i++)
	{
        ArgxcOptions *opt = &argxc->options[i];

        // Find if the parent option exists and get its position
        int parentPos = -1;

        for (size_t j = 0; j < argxc->mainArgsCount; j++)
		{
            if ((opt->param && strcmp(argxc->mainArgs[j], opt->param) == 0) ||
                	(opt->sparam && strcmp(argxc->mainArgs[j], opt->sparam) == 0))
			{
                parentPos = j;
                break;
            }
        }

        if (parentPos >= 0 && (opt->hasSubParams || opt->hasAnySubParams))
		{
            // Check if the requested sub-parameter exists after the parent
            for (size_t j = 0; j < opt->subParamsCount; j++)
			{
                ArgxcOptions *sub = &opt->subParams[j];

                if (sub->id && strcmp(sub->id, id) == 0)
				{
                    for (size_t k = parentPos + 1; k < argxc->mainArgsCount; k++)
					{
                        if ((sub->param && strcmp(argxc->mainArgs[k], sub->param) == 0) ||
                            	(sub->sparam && strcmp(argxc->mainArgs[k], sub->sparam) == 0))
						{
                            result.exists = true;
                            break;
                        }
                    }

                    if (!result.exists && (size_t)(parentPos + 1) < argxc->mainArgsCount)
					{
                        char *nextArg = argxc->mainArgs[parentPos + 1];

                        if ((sub->param && strcmp(nextArg, sub->param) == 0) ||
                            	(sub->sparam && strcmp(nextArg, sub->sparam) == 0))
						{
                            result.exists = true;
                        }
                    }

                    // Handle any sub-sub-parameters if they exist
                    if (result.exists && (sub->hasSubParams || sub->hasAnySubParams))
					{
                        result.subExistsCount = sub->subParamsCount;
                        result.subExists = malloc(result.subExistsCount * sizeof(bool));

                        if (result.subExists)
						{
                            for (size_t k = 0; k < sub->subParamsCount; k++)
							{
                                bool subsubMatched = false;
                                ArgxcOptions *subsub = &sub->subParams[k];

                                for (size_t l = 0; l < argxc->mainArgsCount; l++)
								{
                                    if ((subsub->param && strcmp(argxc->mainArgs[l], subsub->param) == 0) ||
                                        	(subsub->sparam && strcmp(argxc->mainArgs[l], subsub->sparam) == 0))
									{
                                        subsubMatched = true;
                                        break;
                                    }
                                }

                                result.subExists[k] = subsubMatched;
                            }
                        }
                    }

                    return result;
                }
            }
        }
    }

    return result;
}

bool argxcGetSubParam(Argxc *argxc, const ArgxcParam *param, const char *id)
{
    if (!argxc || !param || !id) return false;

    int index = argxcFindParam(argxc, id);
    return argxcParamExists(argxc, id) && param->subExists && index >= 0 && 
        (size_t)index < param->subExistsCount && param->subExists[index];
}

char *argxcCreateDocs(Argxc *argxc, ArgxcStyle style, const char *title, const char *mainInfo)
{
    if (!argxc) return NULL;

    size_t bufferSize = 4096;
    char *contentStr = malloc(bufferSize);
    if (!contentStr) return NULL;

    contentStr[0] = '\0';

    if (style == ARGX_STYLE_PROFESSIONAL)
	{
        for (size_t i = 0; i < argxc->optionsCount; i++)
		{
            ArgxcOptions *opt = &argxc->options[i];

            // Main option header line
            char temp[512];
            snprintf(temp, sizeof(temp), "ID: %s\n", opt->id ? opt->id : "");
            strcat(contentStr, temp);

            snprintf(temp, sizeof(temp), "[ %s | %s", 
                    opt->sparam ? opt->sparam : "", 
                    opt->param ? opt->param : "");
            strcat(contentStr, temp);

            if (opt->hasSubParams && opt->subParamsCount > 0)
			{
                strcat(contentStr, " [ ");

                for (size_t j = 0; j < opt->subParamsCount; j++)
				{
                    strcat(contentStr, opt->subParams[j].param ? opt->subParams[j].param : "");

                    if (j < opt->subParamsCount - 1)
					{
                        strcat(contentStr, " | ");
                    } else if (j <= opt->subParamsCount)
					{
                        strcat(contentStr, " ");
                    }
                }

                strcat(contentStr, "] ] ");
            } else {
                strcat(contentStr, " ] ");
            }

            strcat(contentStr, opt->info ? opt->info : "");
            strcat(contentStr, "\n");

            // Print all sub-options
            if (opt->hasSubParams && opt->subParamsCount > 0)
			{
                for (size_t j = 0; j < opt->subParamsCount; j++)
				{
                    ArgxcOptions *sub = &opt->subParams[j];

                    // Create spacing for alignment
                    size_t paramLen = opt->param ? strlen(opt->param) : 0;
                    for (size_t k = 0; k < paramLen; k++)
					{
                        strcat(contentStr, " ");
                    }

                    snprintf(temp, sizeof(temp), "  [ %s | %s ] %s\n",
                            sub->sparam ? sub->sparam : "",
                            sub->param ? sub->param : "",
                            sub->info ? sub->info : "");
                    strcat(contentStr, temp);
                }
            }
        }
    } else if (style == ARGX_STYLE_SIMPLE)
	{
        for (size_t i = 0; i < argxc->optionsCount; i++)
		{
            ArgxcOptions *opt = &argxc->options[i];

            char temp[256];
            snprintf(temp, sizeof(temp), "%s, %s - %s\n",
                    opt->sparam ? opt->sparam : "",
                    opt->param ? opt->param : "",
                    opt->info ? opt->info : "");
            strcat(contentStr, temp);

            if (opt->hasSubParams && opt->subParamsCount > 0)
			{
                for (size_t j = 0; j < opt->subParamsCount; j++)
				{
                    ArgxcOptions *sub = &opt->subParams[j];

                    snprintf(temp, sizeof(temp), "  %s, %s - %s\n",
                            sub->sparam ? sub->sparam : "",
                            sub->param ? sub->param : "",
                            sub->info ? sub->info : "");
                    strcat(contentStr, temp);
                }
            }
        }
    }

    // Combine title, mainInfo, and content
    size_t titleLen = title ? strlen(title) : 0;
    size_t mainInfoLen = mainInfo ? strlen(mainInfo) : 0;
    size_t contentLen = strlen(contentStr);
    size_t totalLen = titleLen + mainInfoLen + contentLen + 10; // Extra space for newlines

    char *result = malloc(totalLen);
    if (!result)
	{
        free(contentStr); contentStr = NULL;
        return NULL;
    }

    snprintf(result, totalLen, "%s\n%s\n%s",
            title ? title : "",
            mainInfo ? mainInfo : "",
            contentStr);

    free(contentStr); contentStr = NULL;
    return result;
}

bool argxcCompareArgs(ArgxcOptions *options, size_t optionsCount, char **argv, size_t argvCount)
{
    if (!options || !argv) return false;

    for (size_t i = 1; i < argvCount; i++)
	{
        const char *arg = argv[i];

        bool found = false;
        bool hasSubParams = false;
        bool hasAnySubParams = false;
        ArgxcOptions *matchedOption = NULL;

        // Find the matching option
        for (size_t j = 0; j < optionsCount; j++)
		{
            if ((options[j].sparam && strcmp(options[j].sparam, arg) == 0) ||
                	(options[j].param && strcmp(options[j].param, arg) == 0))
			{
                found = true;
                hasSubParams = options[j].hasSubParams;
                hasAnySubParams = options[j].hasAnySubParams;
                matchedOption = &options[j];
                break;
            }
        }

        if (!found) return false;

        if (hasSubParams || hasAnySubParams)
		{
            // Check if there's a next argument
            if (i + 1 < argvCount)
			{
                const char *nextArg = argv[i + 1];

                // Check if next argument is a sub-parameter
                bool isSubParam = false;

                if (nextArg && strlen(nextArg) > 0 && nextArg[0] != '-')
				{
                    // Validate if it's a valid sub-parameter
                    for (size_t k = 0; k < matchedOption->subParamsCount; k++)
					{
                        if ((matchedOption->subParams[k].param && strcmp(matchedOption->subParams[k].param, nextArg) == 0) ||
                            	(matchedOption->subParams[k].sparam && strcmp(matchedOption->subParams[k].sparam, nextArg) == 0))
						{
                            isSubParam = true;
                            break;
                        }
                    }

                    if (isSubParam)
					{
                        i++; // Skip the sub-parameter
                    } else if (hasSubParams || hasAnySubParams)
					{
                        return false;
                    }
                } else if (hasSubParams || hasAnySubParams)
				{
                    return false;
                }
            }
        }
    }

    return true;
}

// Getters
char **argxcGetMainArgs(Argxc *argxc, size_t *count)
{
    if (!argxc || !count) return NULL;

    *count = argxc->mainArgsCount;

    // Create a copy of the array
    char **copy = malloc(argxc->mainArgsCount * sizeof(char*));
    if (!copy) return NULL;

    for (size_t i = 0; i < argxc->mainArgsCount; i++)
	{
        copy[i] = stringDuplicate(argxc->mainArgs[i]);
    }

    return copy;
}

int argxcGetArgc(Argxc *argxc)
{
    return argxc ? argxc->mainArgc : 0;
}

ArgxcOptions *argxcGetOptions(Argxc *argxc, size_t *count)
{
    if (!argxc || !count) return NULL;

    *count = argxc->optionsCount;
    return argxc->options; // Return direct reference (be careful with modification)
}

const char *argxcGetId(Argxc *argxc)
{
    return argxc ? argxc->id : NULL;
}

// Utility functions for memory management
ArgxcOptions argxcCreateOption(const char *id, const char *param, const char *sparam, 
        const char *info, bool hasSubParams, bool hasAnySubParams)
{
    ArgxcOptions option = {0};

    option.id = stringDuplicate(id);
    option.param = stringDuplicate(param);
    option.sparam = stringDuplicate(sparam);
    option.info = stringDuplicate(info);
    option.hasSubParams = hasSubParams;
    option.hasAnySubParams = hasAnySubParams;
    option.subParams = NULL;
    option.subParamsCount = 0;

    return option;
}

void argxcFreeOption(ArgxcOptions *option)
{
    if (!option) return;

    if (option->id)
	{ free(option->id); option->id = NULL; }
    if (option->param)
	{ free(option->param); option->param = NULL; }
    if (option->sparam)
	{ free(option->sparam); option->sparam = NULL; }
    if (option->info)
	{ free(option->info); option->info = NULL; }

    if (option->subParams)
	{
        for (size_t i = 0; i < option->subParamsCount; i++)
		{
            argxcFreeOption(&option->subParams[i]);
        }

    	free(option->subParams); 
		option->subParams = NULL;
		option->subParamsCapacity = 0;
		option->subParamsCount = 0;
	}

    memset(option, 0, sizeof(ArgxcOptions));
}

void argxcFreeParam(ArgxcParam *param)
{
    if (!param) return;

    free(param->subExists); param->subExists = NULL;
    memset(param, 0, sizeof(ArgxcParam));
}

void argxcFreeStringArray(char **array, size_t count)
{
    freeStringArray(array, count);
}

// Error handling
ArgxcError argxcCreateError(const char *type, const char *error, const char *help, int code)
{
    ArgxcError err = {0};

    err.type = stringDuplicate(type);
    err.error = stringDuplicate(error);
    err.help = stringDuplicate(help);
    err.code = code;

    return err;
}

void argxcFreeError(ArgxcError *error)
{
    if (!error) return;

    free(error->type); error->type = NULL;
    free(error->error); error->error = NULL;
    free(error->help); error->help = NULL;
    memset(error, 0, sizeof(ArgxcError));
}
