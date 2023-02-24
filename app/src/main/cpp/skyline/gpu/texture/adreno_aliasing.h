#ifndef SKYLINE_GPU_TEXTURE_TEXTURE_HPP
#define SKYLINE_GPU_TEXTURE_TEXTURE_HPP

#include <array>
#include <boost/container/static_vector.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <common.h>

namespace skyline::gpu::texture {

/**
 * @return If a particular format is compatible to alias views of without VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT on Adreno GPUs
 */
bool IsAdrenoAliasCompatible(vk::Format lhs, vk::Format rhs) {
    if (lhs <= vk::Format::eUndefined || lhs >= vk::Format::eB10G11R11UfloatPack32 ||
        rhs <= vk::Format::eUndefined || rhs >= vk::Format::eB10G11R11UfloatPack32)
        return false; // Any complex (compressed/multi-planar/etc) formats cannot be properly aliased

    static const std::array<int, static_cast<size_t>(vk::Format::eLast) + 1> ComponentCounts = {
        /* eUndefined */ 0,
        /* eR4G4UnormPack8 */ 2,
        /* eR4G4B4A4UnormPack16 */ 4,
        /* ... */
        /* eLast */ 0
    };

    constexpr size_t MaxComponentCount = 4;
    using ComponentContainer = boost::container::static_vector<int, MaxComponentCount>;
    auto GetComponents = [](vk::Format format) -> ComponentContainer
