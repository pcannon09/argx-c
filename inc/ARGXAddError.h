#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

	// Error creation and management
	ArgxcError argxcCreateErrorSimple(const char *msg, const char *help);
	const char* argxcGetErrorMessage(const ArgxcError *error);
	const char* argxcGetErrorHelp(const ArgxcError *error);
	int argxcGetErrorCode(const ArgxcError *error);

	typedef struct {
    	const char *error;
    	const char *help;
	} ArgxcErrorPair;

	ArgxcErrorPair argxcGetErrorPair(const ArgxcError *error);

#ifdef __cplusplus
}
#endif
