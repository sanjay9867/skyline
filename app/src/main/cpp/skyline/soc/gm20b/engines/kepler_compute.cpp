// SPDX-License-Identifier: MPL-2.0
// Copyright © 2022 Skyline Team and Contributors (https://github.com/skyline-emu/)

#include <soc.h>
#include <soc/gm20b/channel.h>
#include "kepler_compute/qmd.h"
#include "kepler_compute.h"

namespace skyline::soc::gm20b::engine {
    KeplerCompute::KeplerCompute(const DeviceState &state, ChannelContext &channelCtx)
        : syncpoints{state.soc->host1x.syncpoints}, i2m{state, channelCtx} {}

    __attribute__((always_inline)) void KeplerCompute::CallMethod(u32 method, u32 argument) {
        Logger::Verbose("Called method in Kepler compute: 0x{:X} args: 0x{:X}", method, argument);

        HandleMethod(method, argument);
    }

    void KeplerCompute::HandleMethod(u32 method, u32 argument) {
        registers.raw[method] = argument;

        switch (method) {
            ENGINE_STRUCT_CASE(i2m, launchDma, {
                i2m.LaunchDma(*registers.i2m);
            })
            ENGINE_STRUCT_CASE(i2m, loadInlineData, {
                i2m.LoadInlineData(*registers.i2m, argument);
            })
            ENGINE_CASE(sendSignalingPcasB, {
                Logger::Warn("Attempted to execute compute kernel!");
            })
            ENGINE_STRUCT_CASE(reportSemaphore, action, {
                throw exception("Compute semaphores are unimplemented!");
            })
            default:
                return;
        }
    }

    void KeplerCompute::CallMethodBatchNonInc(u32 method, span<u32> arguments) {
        switch (method) {
            case ENGINE_STRUCT_OFFSET(i2m, loadInlineData):
                i2m.LoadInlineData(*registers.i2m, arguments);
                return;
            default:
                break;
        }

        for (u32 argument : arguments)
            HandleMethod(method, argument);
    }
}
