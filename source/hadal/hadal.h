#pragma once

/** @file hadal/hadal.h
 *  @brief TODO docs
 */
#include "hadal_adapter.h"
#include "hadal_window.h"

/** Interface of the display backend. */
typedef struct {
    lake_interface_header header;

#ifdef MOON_VULKAN
    PFN_hadal_vulkan_write_instance         vulkan_write_instance;
    PFN_hadal_vulkan_presentation_support   vulkan_presentation_support;
    PFN_hadal_vulkan_create_surface         vulkan_create_surface;
#endif /* MOON_VULKAN */
} hadal_interface;

/** Interface of the VR/XR runtime backend. */
typedef struct {
    lake_interface_header header;
} hadalxr_interface;
