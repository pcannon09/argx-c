#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef enum {
    	ARGX_STYLE_PROFESSIONAL,
    	ARGX_STYLE_SIMPLE
	} ArgxcStyle;

	typedef struct {
    	char *type;
    	char *error;
    	char *help;
    	int code;
	} ArgxcError;

	// Forward declaration
	struct ArgxcOptions;

	typedef struct ArgxcOptions {
    	char *id;              // ID of the option
    	char *param;           // Long parameter (e.g., --help)
    	char *sparam;          // Short parameter (e.g., -h)
    	char *info;            // Description
    	bool hasSubParams;
    	bool hasAnySubParams;
    	struct ArgxcOptions *subParams;
    	size_t subParamsCount;
    	size_t subParamsCapacity;
	} ArgxcOptions;

	typedef struct {
    	bool exists;
    	bool *subExists;
    	size_t subExistsCount;
	} ArgxcParam;

	// Opaque handle for Argxc instance
	typedef struct Argxc Argxc;

#ifdef __cplusplus
}
#endif
