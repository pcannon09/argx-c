// tests/main.c
// Owned and created by: pcannon09

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../inc/Argx.h"
#include "../inc/macros.h"

int main(int argc, char *argv[])
{
    char *docStr = NULL;

    // Create Argxc instance
    Argxc *mainArgxc = argxcCreate("main-args", argc, argv);

    if (!mainArgxc)
	{
        printf("Failed to create Argxc instance.\n");
        return 1;
    }

    // Setup options
    ArgxcOptions helpOption = argxcCreateOption(
        	"help", "--help", "-h", "Show this help", true, false);
    ArgxcOptions versionOption = argxcCreateOption(
        	"version", "--version", "-v", "Show version message", false, false);
    ArgxcOptions styleOption = argxcCreateOption(
        	"style", "--style", "-s", "Set the style of the documentation (simple OR professional)", true, false);

    // Setup sub-options
    ArgxcOptions versionSubOption = argxcCreateOption(
        	"version", "version", "v", "Show version help", false, false);
    ArgxcOptions messageSubOption = argxcCreateOption(
        	"message", "message", "m", "Show a specific message", false, false);

    ArgxcOptions styleSimpleSubOption = argxcCreateOption(
        	"simple", "simple", NULL, "Set simple documentation", false, false);

    ArgxcOptions styleProfessionalSubOption = argxcCreateOption(
        	"professional", "professional", "pro", "Set professional documentation", false, false);

    // Add sub-options to styleOption
    argxcAddSubOption(&styleOption, styleSimpleSubOption);
    argxcAddSubOption(&styleOption, styleProfessionalSubOption);

    // Add sub-options to helpOption
    argxcAddSubOption(&helpOption, versionSubOption);
    argxcAddSubOption(&helpOption, messageSubOption);

    // Add sub-option to versionOption
    argxcAddSubOption(&versionOption, versionSubOption);

    // Add main options
    argxcAddOption(mainArgxc, helpOption);
    argxcAddOption(mainArgxc, versionOption);
    argxcAddOption(mainArgxc, styleOption);

    const char *msg = "Simple documentation on how to use the ARGX test";

    // Check if style param exists
    ArgxcParam styleParam = argxcGetParam(mainArgxc, "style");

    if (styleParam.exists)
    {
        if (argxcGetArgc(mainArgxc) > 2)
        {
			const char *secondArg = NULL;
			size_t count = 0;

            if (argc > 1) secondArg = argxcGetMainArgs(mainArgxc, &count)[2];

            if (strcmp(secondArg, "simple") == 0)
                docStr = argxcCreateDocs(mainArgxc, ARGX_STYLE_SIMPLE, "-- Docs ----", msg);
            else
                docStr = argxcCreateDocs(mainArgxc, ARGX_STYLE_PROFESSIONAL, "-- Docs ----", msg);
        }

        else
        {
            printf("Set one of those two values:\n");
            printf("* simple\n");
            printf("* professional\n");
            printf("NOTE: You can code your own documentation by overriding the Argxc::createDocs() function\n");

            argxcDestroy(mainArgxc); mainArgxc = NULL;

            return 1;
        }
    }
    else
    {
        docStr = argxcCreateDocs(mainArgxc, ARGX_STYLE_PROFESSIONAL, "-- Docs ----", msg);
    }

    // If no args, print docs and exit
    if (argxcGetArgc(mainArgxc) <= 1)
    {
        if (docStr)
		{
            printf("%s\n", docStr);
            free(docStr); docStr = NULL;
        }

		if (mainArgxc) argxcDestroy(mainArgxc);

        return 0;
    }

    // Check if help param exists
    ArgxcParam helpParam = argxcGetParam(mainArgxc, "help");
    if (helpParam.exists)
    {
        if (argxcGetSubParam(mainArgxc, &helpParam, "version"))
            printf("For more information, call the following parameter: `--version`\n");
        else if (argxcGetSubParam(mainArgxc, &helpParam, "message"))
        {
            if (argxcGetArgc(mainArgxc) > 3)
                printf("%s\n", argxcGetMainArgs(mainArgxc, NULL)[3]);
            else
                printf("Enter a message in the third parameter as a string\n");
        }

        if (docStr)
		{
            printf("%s\n", docStr);
            free(docStr); docStr = NULL;
        }
    }

    // Check if version param exists
    ArgxcParam versionParam = argxcGetParam(mainArgxc, "version");
    if (versionParam.exists)
    {
        printf("ARGX Version information:\n");
        printf("Version: %d.%d.%d %s\n", ARGX_VERSION_MAJOR, ARGX_VERSION_MINOR, ARGX_VERSION_PATCH, ARGX_VERSION_STATE);
        printf("Version Standard: %d\n", ARGX_VERSION_STD);
        printf("Development Type: %s\n", ARGX_DEV ? "DEV" : "PRODUCTION");
    }

    // Validate args
    size_t optionsCount = 0;
    ArgxcOptions *options = argxcGetOptions(mainArgxc, &optionsCount);
    size_t argvCount = 0;
    char **mainArgs = argxcGetMainArgs(mainArgxc, &argvCount);

    if (!argxcCompareArgs(options, optionsCount, mainArgs, argvCount))
    {
        printf("Argxc: Unknown option `%s`\n", argvCount > 1 ? mainArgs[1] : "<UNKNOWN>");
        if (docStr) free(docStr);
        argxcDestroy(mainArgxc);
        return 1;
    }

    if (docStr) free(docStr);
    if (mainArgxc)
	{ argxcDestroy(mainArgxc); mainArgxc = NULL; }

    return 0;
}

