// SPDX-License-Identifier: MPL-2.0
// Copyright © 2022 Skyline Team and Contributors (https://github.com/skyline-emu/)

#pragma once

#include <common.h>
#include "macro_interpreter.h"

namespace skyline::soc::gm20b {
    namespace macro_hle {
        using Function = void (*)(size_t offset, span<u32> args, engine::MacroEngineBase *targetEngine);
    }

    /**
     * @brief Holds per-channel macro state
     */
    struct MacroState {
        struct MacroHleEntry {
            macro_hle::Function function;
            bool valid;
        };

        engine::MacroInterpreter macroInterpreter; //!< The macro interpreter for handling 3D/2D macros
        std::array<u32, 0x2000> macroCode{}; //!< Stores GPU macros, writes to it will wraparound on overflow
        std::array<size_t, 0x80> macroPositions{}; //!< The positions of each individual macro in macro code memory, there can be a maximum of 0x80 macros at any one time
        std::array<MacroHleEntry, 0x80> macroHleFunctions{}; //!< The HLE functions for each macro position, used to optionally override the interpreter
        bool invalidatePending{};

        MacroState() : macroInterpreter(macroCode) {}

        void Invalidate();

        void Execute(u32 position, span<u32> args, engine::MacroEngineBase *targetEngine);
    };
}
