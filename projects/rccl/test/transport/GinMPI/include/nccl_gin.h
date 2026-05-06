/*************************************************************************
 * SPDX-FileCopyrightText: Copyright (c) 2017-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * See LICENSE.txt for more license information
 *
 *************************************************************************
 * RCCL TEST-SCOPE PORT.
 *
 * Adapted from upstream NCCL's src/include/plugin/nccl_gin.h. Lives under
 * test/transport/GinMPI/include/ and is consumed only when building the
 * GIN unit tests. Does NOT ship in librccl.so.
 *
 * Deviations from upstream:
 *   1. Only gin/gin_v13.h is included (and ncclGin_t is pinned to
 *      ncclGin_v13_t). Upstream also pulls v11 / v12 to support older
 *      external plugins via the NCCL plugin loader. Our tests link
 *      against IbCastGinIbProxy directly (no dlopen), so we only need
 *      the version that vtable was authored against - v13. This pin is
 *      load-bearing: if the GIN ABI rev advances, gin.cc and these
 *      tests must be re-validated together.
 *   2. NCCL_GIN_TYPE_{NONE,PROXY,GDAKI} are defined here (locally) instead
 *      of being pulled from upstream's nccl_device/core.h. RCCL doesn't
 *      ship the device-side core.h yet, and net_ib_cast/gin.cc only
 *      consumes the numeric backend selector - not the full enum. Values
 *      match upstream so casts stay valid when the device-side port lands.
 *   3. NCCL_NET_SIGNAL_OP_{INC,ADD} aliases are added because gin.cc uses
 *      the NCCL_NET_SIGNAL_OP_* spelling while upstream nccl_gin.h only
 *      defines NCCL_GIN_SIGNAL_OP_*. Both spellings refer to the same
 *      numeric op codes (0x1 increment, 0x2 add).
 *
 * When the host-side GIN port lands in tracked RCCL source under
 * src/include/plugin/, delete this file and let gin.cc resolve the
 * include via the production tree.
 *************************************************************************/

#ifndef NCCL_GIN_H_
#define NCCL_GIN_H_

#include "nccl.h"
#include "nccl_common.h"
#include "net_device.h"
#include "nccl_net.h"
#include <stdint.h>

#define NCCL_GIN_HANDLE_MAXSIZE 128
#define MAX_GIN_SIZE (1024*1024*1024L) // Rather than send INT_MAX which is 2G-1, send a power of two.

#ifndef NCCL_GIN_MAX_PLUGINS
#define NCCL_GIN_MAX_PLUGINS 16
#endif

#define NCCL_GIN_SIGNAL_OP_INC 0x1
#define NCCL_GIN_SIGNAL_OP_ADD 0x2

// Backend selector consumed by net_ib_cast/gin.cc (proxy / GDAKI / direct).
// Numeric values match upstream NCCL's ncclGinType_t and
// NCCL_NET_DEVICE_GIN_PROXY/GDAKI in net_device.h.
#define NCCL_GIN_TYPE_NONE   0
#define NCCL_GIN_TYPE_PROXY  2
#define NCCL_GIN_TYPE_GDAKI  3

// gin.cc uses the NCCL_NET_SIGNAL_OP_* spelling. Upstream defines this in
// nccl_net.h; we alias it here to keep this header self-contained for the
// test build and avoid touching the production nccl_net.h.
#ifndef NCCL_NET_SIGNAL_OP_INC
#define NCCL_NET_SIGNAL_OP_INC NCCL_GIN_SIGNAL_OP_INC
#endif
#ifndef NCCL_NET_SIGNAL_OP_ADD
#define NCCL_NET_SIGNAL_OP_ADD NCCL_GIN_SIGNAL_OP_ADD
#endif

#include "gin/gin_v13.h"

typedef ncclGin_v13_t ncclGin_t;
typedef ncclGinConfig_v13_t ncclGinConfig_t;

#define NCCL_GIN_PLUGIN_SYMBOL ncclGinPlugin_v13

#endif // end include guard
