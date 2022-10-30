// SPDX-License-Identifier: MPL-2.0
// Copyright © 2022 Skyline Team and Contributors (https://github.com/skyline-emu/)

#pragma once

#include <gpu/descriptor_allocator.h>
#include "common.h"
#include "active_state.h"
#include "constant_buffers.h"
#include "samplers.h"
#include "textures.h"

namespace skyline::gpu::interconnect::maxwell3d {
    /**
     * @brief The core Maxwell 3D interconnect object, directly accessed by the engine code to perform rendering operations
     */
    class Maxwell3D {
      public:
        struct ClearEngineRegisters {
            const engine::Scissor &scissor0;
            const engine::ViewportClip &viewportClip0;
            const engine::ClearRect &clearRect;
            const std::array<u32, 4> &colorClearValue;
            const float &depthClearValue;
            const u32 &stencilClearValue;
            const engine::SurfaceClip &surfaceClip;
            const engine::ClearSurfaceControl &clearSurfaceControl;
        };

        /**
         * @brief The full set of register state used by the GPU interconnect
         */
        struct EngineRegisterBundle {
            ActiveState::EngineRegisters activeStateRegisters;
            ClearEngineRegisters clearRegisters;
            ConstantBufferSelectorState::EngineRegisters constantBufferSelectorRegisters;
            SamplerPoolState::EngineRegisters samplerPoolRegisters;
            TexturePoolState::EngineRegisters texturePoolRegisters;
        };

      private:
        InterconnectContext ctx;
        ActiveState activeState;
        ClearEngineRegisters clearEngineRegisters;
        ConstantBuffers constantBuffers;
        Samplers samplers;
        Textures textures;
        std::shared_ptr<memory::Buffer> quadConversionBuffer{};
        bool quadConversionBufferAttached{};

        static constexpr size_t DescriptorBatchSize{0x100};
        std::shared_ptr<boost::container::static_vector<DescriptorAllocator::ActiveDescriptorSet, DescriptorBatchSize>> attachedDescriptorSets;
        DescriptorAllocator::ActiveDescriptorSet *activeDescriptorSet{};
        std::vector<TextureView *> activeDescriptorSetSampledImages{};

        size_t UpdateQuadConversionBuffer(u32 count, u32 firstVertex);

        vk::Rect2D GetClearScissor();

      public:
        DirectPipelineState &directState;

        Maxwell3D(GPU &gpu,
                  soc::gm20b::ChannelContext &channelCtx,
                  nce::NCE &nce,
                  kernel::MemoryManager &memoryManager,
                  DirtyManager &manager,
                  const EngineRegisterBundle &registerBundle);

        /**
         * @brief Loads the given data into the constant buffer pointed by the constant buffer selector starting at the given offset
         */
        void LoadConstantBuffer(span<u32> data, u32 offset);

        /**
         * @brief Binds the constant buffer selector to the given pipeline stage
         */
        void BindConstantBuffer(engine::ShaderStage stage, u32 index, bool enable);

        /**
         * @note See ConstantBuffers::DisableQuickBind
         */
        void DisableQuickConstantBufferBind();

        void Clear(engine::ClearSurface &clearSurface);

        void Draw(engine::DrawTopology topology, bool transformFeedbackEnable, bool indexed, u32 count, u32 first, u32 instanceCount, u32 vertexOffset, u32 firstInstance);
    };
}
