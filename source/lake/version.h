#pragma once

/** @file lake/bedrock/version.h 
 *  @brief TODO docs 
 */
#define LAKE_VERSION_NUM_MAJOR(VER)    ((VER) / 10000000)
#define LAKE_VERSION_NUM_MINOR(VER)    (((VER) / 10000) % 1000)
#define LAKE_VERSION_NUM_REVISION(VER) ((VER) % 10000)
#define LAKE_VERSION_NUM(MA, MI, REV)  ((MA) * 10000000 + (MI) * 10000 + (REV))

