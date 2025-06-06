#pragma once

// Versioning
#define ARGX_VERSION_MAJOR            1
#define ARGX_VERSION_MINOR            0
#define ARGX_VERSION_PATCH            2
#define ARGX_VERSION_STD              20250606

// Version states:
// * dev                    0
// * beta                   1
// * snap                   2
// * build (AKA: Release)   3
#define ARGX_VERSION_STATE           "build"

#define ARGX_VERSION                ((ARGX_VERSION_MAJOR<<16)|(ARGX_VERSION_MINOR<<8)|(ARGX_VERSION_PATCH)|(ARGX_VERSION_STATE << 24))
#define ARGX_VERSION_CHECK(ARGX_VERSION_MAJOR, ARGX_VERSION_MINOR, ARGX_VERSION_PATCH, ARGX_VERSION_STATE) \
    (((ARGX_VERSION_MAJOR)<<16)|((ARGX_VERSION_MINOR)<<8)|(ARGX_VERSION_PATCH)|((ARGX_VERSION_STATE) << 24))

// DEV or PROD
#define ARGX_DEV false

// Macro utils
#define ARGX_STRINGIFY(x) #x
#define ARGX_TOSTRING(x) ARGX_STRINGIFY(x)
