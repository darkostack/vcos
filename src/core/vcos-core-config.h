#ifndef VCOS_CORE_CONFIG_H
#define VCOS_CORE_CONFIG_H

#define VCOS_CORE_CONFIG_H_IN

#ifdef VCOS_PROJECT_CORE_CONFIG_FILE
#include VCOS_PROJECT_CORE_CONFIG_FILE
#endif

#include "vcos-core-default-config.h"
#include "vcos-core-net-default-config.h"

#undef VCOS_CORE_CONFIG_H_IN

#endif // VCOS_CORE_CONFIG_H
