#pragma once

#include <stdbool.h>

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
 	 * @brief Create a new Argxc instance with a specified ID and command-line arguments.
 	 *
 	 * @param id Identifier for the parser instance.
 	 * @param argc Number of command-line arguments.
 	 * @param argv Array of command-line argument strings.
 	 * @return Argxc* Pointer to the created Argxc instance.
 	 */
	Argxc* argxcCreate(const char *id, int argc, char *argv[]);

	/**
 	 * @brief Create a new Argxc instance with default values (empty ID and no arguments).
 	 *
 	 * @return Argxc* Pointer to the default Argxc instance.
 	 */
	Argxc* argxcCreateDefault(void);

	/**
 	 * @brief Destroy an Argxc instance and free all associated memory.
 	 *
 	 * @param argxc Pointer to the Argxc instance to destroy.
 	 */
	void argxcDestroy(Argxc *argxc);

	/**
 	 * @brief Add a new option to the Argxc instance.
 	 *
 	 * @param argxc Pointer to the Argxc instance.
 	 * @param option The option to add.
 	 */
	void argxcAddOption(Argxc *argxc, ArgxcOptions option);

	/**
 	 * @brief Add a sub-option to a parent option.
 	 *
 	 * @param parent Pointer to the parent option.
 	 * @param subOption Sub-option to add under the parent.
 	 */
	void argxcAddSubOption(ArgxcOptions *parent, ArgxcOptions subOption);

	/**
 	 * @brief Find the index of a parameter by ID.
 	 *
 	 * @param argxc Pointer to the Argxc instance.
 	 * @param id The identifier of the parameter to find.
 	 * @return int Index of the parameter, or -1 if not found.
 	 */
	int argxcFindParam(Argxc *argxc, const char *id);

	/**
 	 * @brief Retrieve a parameter by ID.
 	 *
 	 * @param argxc Pointer to the Argxc instance.
 	 * @param id The identifier of the parameter to retrieve.
 	 * @return ArgxcParam The parameter object.
 	 */
	ArgxcParam argxcGetParam(Argxc *argxc, const char *id);

	/**
 	 * @brief Check if a parameter with the given ID exists.
 	 *
 	 * @param argxc Pointer to the Argxc instance.
 	 * @param id The identifier of the parameter.
 	 * @return true if the parameter exists, false otherwise.
 	 */
	bool argxcParamExists(Argxc *argxc, const char *id);

	/**
 	 * @brief Retrieve a sub-parameter from a given parameter by ID.
 	 *
 	 * @param argxc Pointer to the Argxc instance.
 	 * @param param Pointer to the parent parameter.
 	 * @param id The identifier of the sub-parameter.
 	 * @return true if the sub-parameter exists, false otherwise.
 	 */
	bool argxcGetSubParam(Argxc *argxc, const ArgxcParam *param, const char *id);

	/**
 	 * @brief Compare if the given argv matches a list of ArgxcOptions.
 	 *
 	 * @param options Array of ArgxcOptions.
 	 * @param optionsCount Number of options.
 	 * @param argv Array of argument strings.
 	 * @param argvCount Number of argument strings.
 	 * @return true if argv matches the options, false otherwise.
 	 */
	bool argxcCompareArgs(ArgxcOptions *options, size_t optionsCount, char **argv, size_t argvCount);

	/**
 	 * @brief Generate documentation for the defined options.
 	 *
 	 * @param argxc Pointer to the Argxc instance.
 	 * @param style Documentation output style.
 	 * @param title Title of the documentation.
 	 * @param mainInfo Additional info to be displayed in the documentation.
 	 * @return char* Documentation string (must be freed by caller).
 	 */
	char *argxcCreateDocs(Argxc *argxc, ArgxcStyle style, const char *title, const char *mainInfo);

	/**
 	 * @brief Get the raw command-line arguments passed to Argxc.
 	 *
 	 * @param argxc Pointer to the Argxc instance.
 	 * @param count Output: number of arguments returned.
 	 * @return char** Array of argument strings.
 	 */
	char **argxcGetMainArgs(Argxc *argxc, size_t *count);

	/**
 	 * @brief Get the number of command-line arguments passed.
 	 *
 	 * @param argxc Pointer to the Argxc instance.
 	 * @return int Number of arguments.
 	 */
	int argxcGetArgc(Argxc *argxc);

	/**
 	 * @brief Get all top-level options defined for the Argxc instance.
 	 *
 	 * @param argxc Pointer to the Argxc instance.
 	 * @param count Output: number of options returned.
 	 * @return ArgxcOptions* Array of options.
 	 */
	ArgxcOptions *argxcGetOptions(Argxc *argxc, size_t *count);

	/**
 	 * @brief Get the identifier of the Argxc instance.
 	 *
 	 * @param argxc Pointer to the Argxc instance.
 	 * @return const char* The ID string.
 	 */
	const char *argxcGetId(Argxc *argxc);

	/**
 	 * @brief Create a new option.
 	 *
 	 * @param id Option identifier.
 	 * @param param Parameter name (e.g., --param).
 	 * @param sparam Short form (e.g., -p).
 	 * @param info Description of the option.
 	 * @param hasSubParams Whether the option has sub-parameters.
 	 * @param hasAnySubParams Whether the option accepts any sub-parameters.
 	 * @return ArgxcOptions The created option.
 	 */
	ArgxcOptions argxcCreateOption(const char *id, const char *param, const char *sparam, 
            const char *info, bool hasSubParams, bool hasAnySubParams);

	/**
 	 * @brief Free resources associated with an ArgxcOptions struct.
 	 *
 	 * @param option Pointer to the option to free.
 	 */
	void argxcFreeOption(ArgxcOptions *option);

	/**
 	 * @brief Free resources associated with an ArgxcParam struct.
 	 *
 	 * @param param Pointer to the parameter to free.
 	 */
	void argxcFreeParam(ArgxcParam *param);

	/**
 	 * @brief Free a string array returned from Argxc (e.g., argv array).
 	 *
 	 * @param array The array to free.
 	 * @param count Number of elements in the array.
 	 */
	void argxcFreeStringArray(char **array, size_t count);

	/**
 	 * @brief Create an ArgxcError object.
 	 *
 	 * @param type Type/category of the error.
 	 * @param error Error message.
 	 * @param help Help message for resolving the error.
 	 * @param code Exit code or error code.
 	 * @return ArgxcError The created error struct.
 	 */
	ArgxcError argxcCreateError(const char *type, const char *error, const char *help, int code);

	/**
 	 * @brief Free resources associated with an ArgxcError struct.
 	 *
 	 * @param error Pointer to the error to free.
 	 */
	void argxcFreeError(ArgxcError *error);

#ifdef __cplusplus
}
#endif
