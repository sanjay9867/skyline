// SPDX-License-Identifier: MPL-2.0
// Copyright © 2021 Skyline Team and Contributors (https://github.com/skyline-emu/)

#pragma once

#include <common.h>

namespace skyline::soc::host1x {
    namespace Host1x {

class Host1x;

class Nvdec {
public:
    explicit Nvdec(Host1x& host1x);
    ~Nvdec();

    /// Writes the method into the state, Invoke Execute() if encountered
    void ProcessMethod(u32 method, u32 argument);

    /// Return most recently decoded frame
    [[nodiscard]] AVFramePtr GetFrame();

private:
    /// Invoke codec to decode a frame
    void Execute();

    Host1x& host1x;
    NvdecCommon::NvdecRegisters state;
    std::unique_ptr<Codec> codec;
};

} // namespace Host1x

} // namespace Tegra
