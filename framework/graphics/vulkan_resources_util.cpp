/*
** Copyright (c) 2023 LunarG, Inc.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and associated documentation files (the "Software"),
** to deal in the Software without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense,
** and/or sell copies of the Software, and to permit persons to whom the
** Software is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
** DEALINGS IN THE SOFTWARE.
*/

#include "vulkan_resources_util.h"

GFXRECON_BEGIN_NAMESPACE(gfxrecon)
GFXRECON_BEGIN_NAMESPACE(graphics)

static constexpr bool IsMemoryCoherent(VkMemoryPropertyFlags property_flags)
{
    return ((property_flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void GetFormatAspects(VkFormat format, std::vector<VkImageAspectFlagBits>* aspects, bool* combined_depth_stencil)
{
    assert((aspects != nullptr) && (combined_depth_stencil != nullptr));

    (*combined_depth_stencil) = false;

    switch (format)
    {
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            aspects->push_back(VK_IMAGE_ASPECT_DEPTH_BIT);
            aspects->push_back(VK_IMAGE_ASPECT_STENCIL_BIT);
            (*combined_depth_stencil) = true;
            break;
        case VK_FORMAT_D16_UNORM:
        case VK_FORMAT_X8_D24_UNORM_PACK32:
        case VK_FORMAT_D32_SFLOAT:
            aspects->push_back(VK_IMAGE_ASPECT_DEPTH_BIT);
            break;
        case VK_FORMAT_S8_UINT:
            aspects->push_back(VK_IMAGE_ASPECT_STENCIL_BIT);
            break;
        case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
        case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
        case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
        case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
        case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
        case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
            aspects->push_back(VK_IMAGE_ASPECT_PLANE_0_BIT);
            aspects->push_back(VK_IMAGE_ASPECT_PLANE_1_BIT);
            aspects->push_back(VK_IMAGE_ASPECT_PLANE_2_BIT);
            break;
        case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
        case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
        case VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT:
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT:
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT:
        case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
        case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:
        case VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT:
            aspects->push_back(VK_IMAGE_ASPECT_PLANE_0_BIT);
            aspects->push_back(VK_IMAGE_ASPECT_PLANE_1_BIT);
            break;
        default:
            aspects->push_back(VK_IMAGE_ASPECT_COLOR_BIT);
            break;
    }
}

VkImageAspectFlags GetFormatAspectMask(VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        case VK_FORMAT_D16_UNORM:
        case VK_FORMAT_X8_D24_UNORM_PACK32:
        case VK_FORMAT_D32_SFLOAT:
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        case VK_FORMAT_S8_UINT:
            return VK_IMAGE_ASPECT_STENCIL_BIT;
        case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
        case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
        case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
        case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
        case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
        case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
            return VK_IMAGE_ASPECT_PLANE_0_BIT | VK_IMAGE_ASPECT_PLANE_1_BIT | VK_IMAGE_ASPECT_PLANE_2_BIT;
        case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
        case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
        case VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT:
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT:
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT:
        case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
        case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:
        case VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT:
            return VK_IMAGE_ASPECT_PLANE_0_BIT | VK_IMAGE_ASPECT_PLANE_1_BIT;
        default:
            return VK_IMAGE_ASPECT_COLOR_BIT;
    }
}

VkFormat GetImageAspectFormat(VkFormat format, VkImageAspectFlagBits aspect)
{
    switch (format)
    {
        // Per-aspect compatible formats as defined by the VkBufferImageCopy documentation.
        case VK_FORMAT_D16_UNORM_S8_UINT:
            if (aspect == VK_IMAGE_ASPECT_DEPTH_BIT)
            {
                return VK_FORMAT_D16_UNORM;
            }
            else
            {
                assert(aspect == VK_IMAGE_ASPECT_STENCIL_BIT);
                return VK_FORMAT_S8_UINT;
            }
        case VK_FORMAT_D24_UNORM_S8_UINT:
            if (aspect == VK_IMAGE_ASPECT_DEPTH_BIT)
            {
                // Effectively the same format (D24 texels packed into a 32-bit word).
                return VK_FORMAT_D24_UNORM_S8_UINT;
            }
            else
            {
                assert(aspect == VK_IMAGE_ASPECT_STENCIL_BIT);
                return VK_FORMAT_S8_UINT;
            }
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            if (aspect == VK_IMAGE_ASPECT_DEPTH_BIT)
            {
                return VK_FORMAT_D32_SFLOAT;
            }
            else
            {
                assert(aspect == VK_IMAGE_ASPECT_STENCIL_BIT);
                return VK_FORMAT_S8_UINT;
            }
        // Per-aspect/plane compatible formats as defined by the "Plane Format Compatibility Table" from the
        // Vulkan specification.
        case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
        case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
        case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
            // All planes share the same format.
            return VK_FORMAT_R8_UNORM;
        case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
        case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
        case VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT:
            if (aspect == VK_IMAGE_ASPECT_PLANE_0_BIT)
            {
                return VK_FORMAT_R8_UNORM;
            }
            else
            {
                assert(aspect == VK_IMAGE_ASPECT_PLANE_1_BIT);
                return VK_FORMAT_R8G8_UNORM;
            }
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
            // All planes share the same format.
            return VK_FORMAT_R10X6_UNORM_PACK16;
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT:
            if (aspect == VK_IMAGE_ASPECT_PLANE_0_BIT)
            {
                return VK_FORMAT_R10X6_UNORM_PACK16;
            }
            else
            {
                assert(aspect == VK_IMAGE_ASPECT_PLANE_1_BIT);
                return VK_FORMAT_R10X6G10X6_UNORM_2PACK16;
            }
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
            // All planes share the same format.
            return VK_FORMAT_R12X4_UNORM_PACK16;
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT:
            if (aspect == VK_IMAGE_ASPECT_PLANE_0_BIT)
            {
                return VK_FORMAT_R12X4_UNORM_PACK16;
            }
            else
            {
                assert(aspect == VK_IMAGE_ASPECT_PLANE_1_BIT);
                return VK_FORMAT_R12X4G12X4_UNORM_2PACK16;
            }
        case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
        case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
        case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
            // All planes share the same format.
            return VK_FORMAT_R16_UNORM;
        case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
        case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:
        case VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT:
            if (aspect == VK_IMAGE_ASPECT_PLANE_0_BIT)
            {
                return VK_FORMAT_R16_UNORM;
            }
            else
            {
                assert(aspect == VK_IMAGE_ASPECT_PLANE_1_BIT);
                return VK_FORMAT_R16G16_UNORM;
            }
        default:
            assert((aspect == VK_IMAGE_ASPECT_COLOR_BIT) || (aspect == VK_IMAGE_ASPECT_DEPTH_BIT) ||
                   (aspect == VK_IMAGE_ASPECT_STENCIL_BIT));
            return format;
    }
}

bool FindMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties& memory_properties,
                         uint32_t                                memory_type_bits,
                         VkMemoryPropertyFlags                   desired_flags,
                         uint32_t*                               found_index,
                         VkMemoryPropertyFlags*                  found_flags)
{
    bool found = false;

    assert(memory_properties.memoryTypeCount > 0);

    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
    {
        if ((memory_type_bits & (1 << i)) &&
            ((memory_properties.memoryTypes[i].propertyFlags & desired_flags) == desired_flags))
        {
            found = true;

            if (found_index != nullptr)
            {
                (*found_index) = i;
            }

            if (found_flags != nullptr)
            {
                (*found_flags) = memory_properties.memoryTypes[i].propertyFlags;
            }

            break;
        }
    }

    return found;
}

uint64_t VulkanResourcesUtil::GetImageResourceSizes(VkImage                image,
                                                    VkFormat               format,
                                                    VkImageType            type,
                                                    const VkExtent3D&      extent,
                                                    uint32_t               mip_levels,
                                                    uint32_t               array_layers,
                                                    VkImageTiling          tiling,
                                                    VkImageAspectFlagBits  aspect,
                                                    std::vector<uint64_t>* mip_level_offsets,
                                                    std::vector<uint64_t>* mip_level_sizes,
                                                    bool                   need_staging_copy,
                                                    bool                   all_layers_per_level)
{
    if (mip_level_sizes != nullptr)
    {
        mip_level_sizes->clear();
    }

    if (mip_level_offsets != nullptr)
    {
        mip_level_offsets->clear();
    }

    VkDeviceSize resource_size = 0;
    if (need_staging_copy)
    {
        VkImageCreateInfo create_info     = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        create_info.pNext                 = nullptr;
        create_info.flags                 = 0;
        create_info.imageType             = type;
        create_info.format                = GetImageAspectFormat(format, aspect);
        create_info.extent                = extent;
        create_info.mipLevels             = 1;
        create_info.arrayLayers           = all_layers_per_level ? array_layers : 1;
        create_info.samples               = VK_SAMPLE_COUNT_1_BIT;
        create_info.tiling                = tiling;
        create_info.usage                 = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        create_info.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices   = nullptr;
        create_info.initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED;

        for (uint32_t m = 0; m < mip_levels; ++m)
        {
            create_info.extent.width  = std::max(1u, (extent.width >> m));
            create_info.extent.height = std::max(1u, (extent.height >> m));
            create_info.extent.depth  = std::max(1u, (extent.depth >> m));

            VkImage  temp_image;
            VkResult result = device_table_.CreateImage(device_, &create_info, nullptr, &temp_image);
            if (result != VK_SUCCESS)
            {
                GFXRECON_LOG_ERROR("VulkanResourcesUtil::%s() Failed creating VkImage", __func__)

                if (mip_level_offsets != nullptr)
                {
                    mip_level_offsets->clear();
                }

                if (mip_level_sizes != nullptr)
                {
                    mip_level_sizes->clear();
                }

                return 0;
            }

            VkMemoryRequirements memory_requirements;
            device_table_.GetImageMemoryRequirements(device_, temp_image, &memory_requirements);

            for (uint32_t l = 0; l < array_layers; ++l)
            {
                if (mip_level_offsets != nullptr)
                {
                    mip_level_offsets->push_back(resource_size);
                }

                if (mip_level_sizes != nullptr)
                {
                    mip_level_sizes->push_back(memory_requirements.size);
                }

                resource_size += memory_requirements.size;

                if (all_layers_per_level)
                {
                    break;
                }
            }

            device_table_.DestroyImage(device_, temp_image, nullptr);
        }
    }
    else
    {
        VkMemoryRequirements memory_requirements;
        device_table_.GetImageMemoryRequirements(device_, image, &memory_requirements);
        resource_size = memory_requirements.size;

        if (mip_level_sizes || mip_level_offsets)
        {
            uint64_t total_offset = 0;
            for (uint32_t m = 0; m < mip_levels; ++m)
            {
                for (uint32_t l = 0; l < array_layers; ++l)
                {
                    VkSubresourceLayout layout;
                    VkImageSubresource  subresource;
                    subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    subresource.mipLevel   = m;
                    subresource.arrayLayer = l;

                    device_table_.GetImageSubresourceLayout(device_, image, &subresource, &layout);

                    if (all_layers_per_level)
                    {
                        const uint64_t level_size = layout.size * array_layers;

                        if (mip_level_sizes != nullptr)
                        {
                            mip_level_sizes->push_back(level_size);
                        }

                        if (mip_level_offsets != nullptr)
                        {
                            mip_level_offsets->push_back(total_offset);
                        }

                        total_offset += level_size;

                        // Continue with next mipmap level
                        break;
                    }
                    else
                    {
                        if (mip_level_sizes != nullptr)
                        {
                            mip_level_sizes->push_back(layout.size);
                        }

                        if (mip_level_offsets != nullptr)
                        {
                            mip_level_offsets->push_back(layout.offset);
                        }
                    }
                }
            }
        }
    }

    return resource_size;
}

VkResult VulkanResourcesUtil::CreateStagingBuffer(VkDeviceSize size)
{
    assert(size);

    if (staging_buffer_.buffer != VK_NULL_HANDLE)
    {
        if (staging_buffer_.size < size)
        {
            DestroyStagingBuffer();
        }
        else
        {
            return VK_SUCCESS;
        }
    }

    assert(staging_buffer_.buffer == VK_NULL_HANDLE && staging_buffer_.size == 0);

    VkBufferCreateInfo create_info    = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    create_info.pNext                 = nullptr;
    create_info.flags                 = 0;
    create_info.size                  = size;
    create_info.usage                 = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    create_info.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices   = nullptr;

    VkResult result = device_table_.CreateBuffer(device_, &create_info, nullptr, &staging_buffer_.buffer);
    if (result == VK_SUCCESS)
    {
        uint32_t             memory_type_index = std::numeric_limits<uint32_t>::max();
        VkMemoryRequirements memory_requirements;

        device_table_.GetBufferMemoryRequirements(device_, staging_buffer_.buffer, &memory_requirements);

        bool found = FindMemoryTypeIndex(memory_properties_,
                                         memory_requirements.memoryTypeBits,
                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
                                         &memory_type_index,
                                         &staging_buffer_.memory_property_flags);
        if (!found)
        {
            // If we are here it is likely that we lack support for HOST_CACHED, fallback to COHERENT
            found = FindMemoryTypeIndex(memory_properties_,
                                        memory_requirements.memoryTypeBits,
                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                        &memory_type_index,
                                        &staging_buffer_.memory_property_flags);
        }

        if (found)
        {
            VkMemoryAllocateInfo alloc_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
            alloc_info.pNext                = nullptr;
            alloc_info.allocationSize       = memory_requirements.size;
            alloc_info.memoryTypeIndex      = memory_type_index;

            result = device_table_.AllocateMemory(device_, &alloc_info, nullptr, &staging_buffer_.memory);
            if (result == VK_SUCCESS)
            {
                device_table_.BindBufferMemory(device_, staging_buffer_.buffer, staging_buffer_.memory, 0);
            }
            else
            {
                GFXRECON_LOG_ERROR("Failed to allocate staging buffer memory for resource memory snapshot");

                device_table_.DestroyBuffer(device_, staging_buffer_.buffer, nullptr);
                staging_buffer_.buffer = VK_NULL_HANDLE;
            }
        }
        else
        {
            GFXRECON_LOG_ERROR("Failed to find a memory type with host visible and host cached or coherent "
                               "properties for resource memory snapshot staging buffer creation");

            result = VK_ERROR_INITIALIZATION_FAILED;
        }

        if (result == VK_SUCCESS)
        {
            staging_buffer_.size       = size;
            staging_buffer_.mapped_ptr = nullptr;
        }
    }
    else
    {
        GFXRECON_LOG_ERROR("Failed to create staging buffer for resource memory snapshot");
    }

    return result;
}

VkResult VulkanResourcesUtil::MapStagingBuffer()
{
    assert(staging_buffer_.buffer != VK_NULL_HANDLE);
    assert(staging_buffer_.memory != VK_NULL_HANDLE);
    assert(staging_buffer_.size);

    VkResult result = VK_SUCCESS;

    if (staging_buffer_.mapped_ptr == nullptr)
    {
        result =
            device_table_.MapMemory(device_, staging_buffer_.memory, 0, VK_WHOLE_SIZE, 0, &staging_buffer_.mapped_ptr);

        if (result != VK_SUCCESS)
        {
            GFXRECON_LOG_ERROR("Failed mapping staging buffer");
        }
    }

    return result;
}

void VulkanResourcesUtil::UnmapStagingBuffer()
{
    if (staging_buffer_.mapped_ptr != nullptr)
    {
        assert(staging_buffer_.buffer != VK_NULL_HANDLE);
        assert(staging_buffer_.memory != VK_NULL_HANDLE);
        assert(staging_buffer_.size);

        device_table_.UnmapMemory(device_, staging_buffer_.memory);
        staging_buffer_.mapped_ptr = nullptr;
    }
}

void VulkanResourcesUtil::InvalidateStagingBuffer()
{
    assert(staging_buffer_.buffer != VK_NULL_HANDLE);
    assert(staging_buffer_.memory != VK_NULL_HANDLE);
    assert(staging_buffer_.size);

    if (!IsMemoryCoherent(staging_buffer_.memory_property_flags))
    {
        assert(staging_buffer_.mapped_ptr != nullptr);

        const VkMappedMemoryRange range{
            VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE, nullptr, staging_buffer_.memory, 0, staging_buffer_.size
        };

        device_table_.InvalidateMappedMemoryRanges(device_, 1, &range);
    }
}

void VulkanResourcesUtil::DestroyStagingBuffer()
{
    UnmapStagingBuffer();

    if (staging_buffer_.buffer != VK_NULL_HANDLE)
    {
        device_table_.DestroyBuffer(device_, staging_buffer_.buffer, nullptr);
        staging_buffer_.buffer = VK_NULL_HANDLE;
    }

    if (staging_buffer_.memory != VK_NULL_HANDLE)
    {
        device_table_.FreeMemory(device_, staging_buffer_.memory, nullptr);
        staging_buffer_.memory = VK_NULL_HANDLE;
    }

    staging_buffer_.memory_property_flags = VkMemoryPropertyFlags(0);
    staging_buffer_.size                  = 0;
}

void VulkanResourcesUtil::InvalidateMappedMemoryRange(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size)
{
    VkMappedMemoryRange invalidate_range = { VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE };
    invalidate_range.pNext               = nullptr;
    invalidate_range.memory              = memory;
    invalidate_range.offset              = offset;
    invalidate_range.size                = size;

    device_table_.InvalidateMappedMemoryRanges(device_, 1, &invalidate_range);
}

VkResult VulkanResourcesUtil::CreateCommandPool(uint32_t queue_family_index)
{
    VkResult result = VK_SUCCESS;

    if (queue_family_index != queue_family_index_ && command_pool_ != VK_NULL_HANDLE)
    {
        DestroyCommandPool();
    }

    if (command_pool_ == VK_NULL_HANDLE)
    {
        VkCommandPoolCreateInfo create_info = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        create_info.pNext                   = nullptr;
        create_info.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        create_info.queueFamilyIndex        = queue_family_index;

        result = device_table_.CreateCommandPool(device_, &create_info, nullptr, &command_pool_);

        if (result == VK_SUCCESS)
        {
            queue_family_index_ = queue_family_index;
        }
        else
        {
            GFXRECON_LOG_ERROR("Failed to create a command pool for resource memory snapshot");
        }
    }

    return result;
}

void VulkanResourcesUtil::DestroyCommandPool()
{
    if (command_pool_ != VK_NULL_HANDLE)
    {
        device_table_.DestroyCommandPool(device_, command_pool_, nullptr);
        command_pool_ = VK_NULL_HANDLE;
    }
}

VkResult VulkanResourcesUtil::CreateCommandBuffer(uint32_t queue_family_index)
{
    if (queue_family_index != queue_family_index_ && command_buffer_ != VK_NULL_HANDLE)
    {
        DestroyCommandBuffer();
    }

    VkResult result = VK_SUCCESS;

    if (command_buffer_ == VK_NULL_HANDLE)
    {
        result = CreateCommandPool(queue_family_index);
        if (result != VK_SUCCESS)
        {
            return result;
        }

        assert(command_pool_ != VK_NULL_HANDLE);

        VkCommandBufferAllocateInfo alloc_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        alloc_info.pNext                       = nullptr;
        alloc_info.commandPool                 = command_pool_;
        alloc_info.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount          = 1;

        result = device_table_.AllocateCommandBuffers(device_, &alloc_info, &command_buffer_);

        if (result == VK_SUCCESS)
        {
            // Because this command buffer was not allocated through the loader, it must be assigned a dispatch
            // table.
            *reinterpret_cast<void**>(command_buffer_) = *reinterpret_cast<void**>(device_);
        }
        else
        {
            GFXRECON_LOG_ERROR("Failed to create a command buffer for resource memory snapshot");
        }
    }

    if (result == VK_SUCCESS)
    {
        result = BeginCommandBuffer();
    }

    return result;
}

void VulkanResourcesUtil::ResetCommandBuffer()
{
    assert(command_buffer_ != VK_NULL_HANDLE);

    device_table_.ResetCommandBuffer(command_buffer_, VkCommandBufferResetFlags(0));
}

VkResult VulkanResourcesUtil::BeginCommandBuffer()
{
    assert(command_buffer_ != VK_NULL_HANDLE);

    VkCommandBufferBeginInfo begin_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    begin_info.pNext                    = nullptr;
    begin_info.flags                    = 0;
    begin_info.pInheritanceInfo         = nullptr;

    VkResult result = device_table_.BeginCommandBuffer(command_buffer_, &begin_info);

    if (result != VK_SUCCESS)
    {
        GFXRECON_LOG_ERROR("Failed to begin a command buffer for resource memory snapshot");
    }

    return result;
}

VkResult VulkanResourcesUtil::EndCommandBuffer()
{
    assert(command_buffer_ != VK_NULL_HANDLE);

    VkResult result = device_table_.EndCommandBuffer(command_buffer_);

    if (result != VK_SUCCESS)
    {
        GFXRECON_LOG_ERROR("Failed to end a command buffer for resource memory snapshot");
    }

    return result;
}

void VulkanResourcesUtil::DestroyCommandBuffer()
{
    if (command_buffer_ != VK_NULL_HANDLE)
    {
        assert(command_pool_ != VK_NULL_HANDLE);

        device_table_.FreeCommandBuffers(device_, command_pool_, 1, &command_buffer_);
        command_buffer_ = VK_NULL_HANDLE;
    }
}

void VulkanResourcesUtil::TransitionImageToSrcOptimal(VkImage            image,
                                                      VkImageLayout      current_layout,
                                                      VkImageAspectFlags aspect)
{
    assert(image != VK_NULL_HANDLE);
    assert(command_buffer_ != VK_NULL_HANDLE);

    VkImageMemoryBarrier memory_barrier;
    memory_barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memory_barrier.pNext                           = nullptr;
    memory_barrier.srcAccessMask                   = 0;
    memory_barrier.dstAccessMask                   = VK_ACCESS_TRANSFER_READ_BIT;
    memory_barrier.oldLayout                       = current_layout;
    memory_barrier.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    memory_barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    memory_barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    memory_barrier.image                           = image;
    memory_barrier.subresourceRange.aspectMask     = aspect;
    memory_barrier.subresourceRange.baseMipLevel   = 0;
    memory_barrier.subresourceRange.levelCount     = VK_REMAINING_MIP_LEVELS;
    memory_barrier.subresourceRange.baseArrayLayer = 0;
    memory_barrier.subresourceRange.layerCount     = VK_REMAINING_ARRAY_LAYERS;

    device_table_.CmdPipelineBarrier(command_buffer_,
                                     VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                     VK_PIPELINE_STAGE_TRANSFER_BIT,
                                     0,
                                     0,
                                     nullptr,
                                     0,
                                     nullptr,
                                     1,
                                     &memory_barrier);
}

void VulkanResourcesUtil::TransitionImageFromSrcOptimal(VkImage            image,
                                                        VkImageLayout      new_layout,
                                                        VkImageAspectFlags aspect)
{
    assert(image != VK_NULL_HANDLE);
    assert(command_buffer_ != VK_NULL_HANDLE);

    VkImageMemoryBarrier memory_barrier;
    memory_barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memory_barrier.pNext                           = nullptr;
    memory_barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    memory_barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    memory_barrier.image                           = image;
    memory_barrier.subresourceRange.aspectMask     = aspect;
    memory_barrier.subresourceRange.baseMipLevel   = 0;
    memory_barrier.subresourceRange.levelCount     = VK_REMAINING_MIP_LEVELS;
    memory_barrier.subresourceRange.baseArrayLayer = 0;
    memory_barrier.subresourceRange.layerCount     = VK_REMAINING_ARRAY_LAYERS;

    memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    memory_barrier.dstAccessMask = 0;
    memory_barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    memory_barrier.newLayout     = new_layout;

    device_table_.CmdPipelineBarrier(command_buffer_,
                                     VK_PIPELINE_STAGE_TRANSFER_BIT,
                                     VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                     0,
                                     0,
                                     nullptr,
                                     0,
                                     nullptr,
                                     1,
                                     &memory_barrier);
}

void VulkanResourcesUtil::CopyImageToBuffer(VkImage                      image,
                                            const VkExtent3D&            extent,
                                            uint32_t                     mip_levels,
                                            uint32_t                     array_layers,
                                            VkImageAspectFlags           aspect,
                                            const std::vector<uint64_t>& sizes,
                                            VkBuffer                     destination_buffer,
                                            bool                         all_layers_per_level)
{
    assert(command_buffer_ != VK_NULL_HANDLE);

    const uint32_t n_subresources = all_layers_per_level ? mip_levels : mip_levels * array_layers;

    assert(sizes.size() == n_subresources);

    std::vector<VkBufferImageCopy> copy_regions;

    VkBufferImageCopy copy_region;
    copy_region.bufferRowLength             = 0; // Request tightly packed data.
    copy_region.bufferImageHeight           = 0; // Request tightly packed data.
    copy_region.bufferOffset                = 0;
    copy_region.imageOffset.x               = 0;
    copy_region.imageOffset.y               = 0;
    copy_region.imageOffset.z               = 0;
    copy_region.imageSubresource.aspectMask = aspect;
    copy_region.imageSubresource.layerCount = all_layers_per_level ? array_layers : 1;

    uint32_t sr = 0;
    for (uint32_t m = 0; m < mip_levels; ++m)
    {
        copy_region.imageSubresource.mipLevel = m;
        copy_region.imageExtent.width         = std::max(1u, (extent.width >> m));
        copy_region.imageExtent.height        = std::max(1u, (extent.height >> m));
        copy_region.imageExtent.depth         = std::max(1u, (extent.depth >> m));

        for (uint32_t l = 0; l < array_layers; ++l)
        {
            copy_region.imageSubresource.baseArrayLayer = l;
            copy_regions.push_back(copy_region);

            copy_region.bufferOffset += sizes[sr];
            ++sr;

            if (all_layers_per_level)
            {
                break;
            }
        }
    }
    assert(sr == n_subresources);

    device_table_.CmdCopyImageToBuffer(command_buffer_,
                                       image,
                                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                       destination_buffer,
                                       static_cast<uint32_t>(copy_regions.size()),
                                       copy_regions.data());
}

void VulkanResourcesUtil::CopyBuffer(VkBuffer source_buffer, VkBuffer destination_buffer, uint64_t size)
{
    assert(source_buffer != VK_NULL_HANDLE);
    assert(command_buffer_ != VK_NULL_HANDLE);

    VkBufferCopy copy_region;
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size      = size;

    device_table_.CmdCopyBuffer(command_buffer_, source_buffer, destination_buffer, 1, &copy_region);
}

VkQueue VulkanResourcesUtil::GetQueue(uint32_t queue_family_index, uint32_t queue_index)
{
    VkQueue queue = VK_NULL_HANDLE;
    device_table_.GetDeviceQueue(device_, queue_family_index, queue_index, &queue);

    if (queue != VK_NULL_HANDLE)
    {
        // Because this queue was not allocated through the loader, it must be assigned a dispatch table.
        *reinterpret_cast<void**>(queue) = *reinterpret_cast<void**>(device_);
    }
    else
    {
        GFXRECON_LOG_ERROR("Failed to retrieve a queue for resource memory snapshot");
    }

    return queue;
}

VkResult VulkanResourcesUtil::SubmitCommandBuffer(VkQueue queue)
{
    assert(command_buffer_ != VK_NULL_HANDLE);
    assert(queue != VK_NULL_HANDLE);

    device_table_.EndCommandBuffer(command_buffer_);

    VkSubmitInfo submit_info         = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submit_info.pNext                = nullptr;
    submit_info.waitSemaphoreCount   = 0;
    submit_info.pWaitSemaphores      = nullptr;
    submit_info.pWaitDstStageMask    = nullptr;
    submit_info.commandBufferCount   = 1;
    submit_info.pCommandBuffers      = &command_buffer_;
    submit_info.signalSemaphoreCount = 0;
    submit_info.pSignalSemaphores    = nullptr;

    VkResult result = device_table_.QueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
    if (result != VK_SUCCESS)
    {
        GFXRECON_LOG_ERROR("Failed to submit command buffer for execution while taking a resource memory snapshot");
        return result;
    }

    result = device_table_.QueueWaitIdle(queue);
    if (result != VK_SUCCESS)
    {
        GFXRECON_LOG_ERROR("QueueWaitIdle returned %d while taking a resource memory snapshot", result);
        return result;
    }

    ResetCommandBuffer();

    return result;
}

VkResult VulkanResourcesUtil::ResolveImage(VkImage           image,
                                           VkFormat          format,
                                           VkImageType       type,
                                           const VkExtent3D& extent,
                                           uint32_t          array_layers,
                                           VkImageLayout     current_layout,
                                           VkQueue           queue,
                                           uint32_t          queue_family_index,
                                           VkImage*          resolved_image,
                                           VkDeviceMemory*   resolved_image_memory)
{
    assert((image != VK_NULL_HANDLE) && (resolved_image != nullptr) && (resolved_image_memory != nullptr));

    VkImageCreateInfo create_info     = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    create_info.pNext                 = nullptr;
    create_info.flags                 = 0;
    create_info.imageType             = type;
    create_info.format                = format;
    create_info.extent                = extent;
    create_info.mipLevels             = 1;
    create_info.arrayLayers           = array_layers;
    create_info.samples               = VK_SAMPLE_COUNT_1_BIT;
    create_info.tiling                = VK_IMAGE_TILING_OPTIMAL;
    create_info.usage                 = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    create_info.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices   = nullptr;
    create_info.initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED;

    VkResult result = device_table_.CreateImage(device_, &create_info, nullptr, resolved_image);
    if (result == VK_SUCCESS)
    {
        uint32_t             memory_type_index = std::numeric_limits<uint32_t>::max();
        VkMemoryRequirements memory_requirements;

        device_table_.GetImageMemoryRequirements(device_, *resolved_image, &memory_requirements);

        bool found = FindMemoryTypeIndex(memory_properties_,
                                         memory_requirements.memoryTypeBits,
                                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                         &memory_type_index,
                                         nullptr);

        if (found)
        {
            VkMemoryAllocateInfo alloc_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
            alloc_info.pNext                = nullptr;
            alloc_info.allocationSize       = memory_requirements.size;
            alloc_info.memoryTypeIndex      = memory_type_index;

            result = device_table_.AllocateMemory(device_, &alloc_info, nullptr, resolved_image_memory);
            if (result == VK_SUCCESS)
            {
                device_table_.BindImageMemory(device_, *resolved_image, *resolved_image_memory, 0);

                result = CreateCommandPool(queue_family_index);
                if (result != VK_SUCCESS)
                {
                    return result;
                }

                result = CreateCommandBuffer(queue_family_index);
                if (result != VK_SUCCESS)
                {
                    return result;
                }

                if (result == VK_SUCCESS)
                {
                    VkImageAspectFlags aspect_mask = GetFormatAspectMask(format);

                    uint32_t             num_barriers = 1;
                    VkImageMemoryBarrier memory_barriers[2];

                    // Destination image
                    memory_barriers[0].sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                    memory_barriers[0].pNext                           = nullptr;
                    memory_barriers[0].srcAccessMask                   = 0;
                    memory_barriers[0].dstAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
                    memory_barriers[0].oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
                    memory_barriers[0].newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    memory_barriers[0].srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
                    memory_barriers[0].dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
                    memory_barriers[0].image                           = image;
                    memory_barriers[0].subresourceRange.aspectMask     = aspect_mask;
                    memory_barriers[0].subresourceRange.baseMipLevel   = 0;
                    memory_barriers[0].subresourceRange.levelCount     = 1;
                    memory_barriers[0].subresourceRange.baseArrayLayer = 0;
                    memory_barriers[0].subresourceRange.layerCount     = array_layers;

                    // Multi-sample source image
                    if (current_layout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
                    {
                        num_barriers = 2;

                        memory_barriers[1].sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                        memory_barriers[1].pNext                           = nullptr;
                        memory_barriers[1].srcAccessMask                   = 0;
                        memory_barriers[1].dstAccessMask                   = VK_ACCESS_TRANSFER_READ_BIT;
                        memory_barriers[1].oldLayout                       = current_layout;
                        memory_barriers[1].newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                        memory_barriers[1].srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
                        memory_barriers[1].dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
                        memory_barriers[1].image                           = image;
                        memory_barriers[1].subresourceRange.aspectMask     = aspect_mask;
                        memory_barriers[1].subresourceRange.baseMipLevel   = 0;
                        memory_barriers[1].subresourceRange.levelCount     = 1;
                        memory_barriers[1].subresourceRange.baseArrayLayer = 0;
                        memory_barriers[1].subresourceRange.layerCount     = array_layers;
                    }

                    device_table_.CmdPipelineBarrier(command_buffer_,
                                                     VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                                     VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                     0,
                                                     0,
                                                     nullptr,
                                                     0,
                                                     nullptr,
                                                     num_barriers,
                                                     memory_barriers);

                    VkImageResolve region;
                    region.srcSubresource.aspectMask     = aspect_mask;
                    region.srcSubresource.mipLevel       = 0;
                    region.srcSubresource.baseArrayLayer = 0;
                    region.srcSubresource.layerCount     = array_layers;
                    region.srcOffset.x                   = 0;
                    region.srcOffset.y                   = 0;
                    region.srcOffset.z                   = 0;
                    region.dstSubresource.aspectMask     = aspect_mask;
                    region.dstSubresource.mipLevel       = 0;
                    region.dstSubresource.baseArrayLayer = 0;
                    region.dstSubresource.layerCount     = array_layers;
                    region.dstOffset.x                   = 0;
                    region.dstOffset.y                   = 0;
                    region.dstOffset.z                   = 0;
                    region.extent.width                  = extent.width;
                    region.extent.height                 = extent.height;
                    region.extent.depth                  = extent.depth;

                    device_table_.CmdResolveImage(command_buffer_,
                                                  image,
                                                  VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                                  image,
                                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                  1,
                                                  &region);

                    memory_barriers[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                    memory_barriers[0].dstAccessMask = 0;
                    memory_barriers[0].oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

                    // Prepare the resolved image for the next staging copy.
                    memory_barriers[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

                    if (num_barriers == 2)
                    {
                        memory_barriers[1].srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                        memory_barriers[1].dstAccessMask = 0;
                        memory_barriers[1].oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                        memory_barriers[1].newLayout     = current_layout;
                    }

                    device_table_.CmdPipelineBarrier(command_buffer_,
                                                     VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                     VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                                     0,
                                                     0,
                                                     nullptr,
                                                     0,
                                                     nullptr,
                                                     num_barriers,
                                                     memory_barriers);

                    result = SubmitCommandBuffer(queue);

                    if (result != VK_SUCCESS)
                    {
                        GFXRECON_LOG_ERROR("Failed to resolve multisample image");
                        device_table_.DestroyImage(device_, *resolved_image, nullptr);
                        device_table_.FreeMemory(device_, *resolved_image_memory, nullptr);

                        *resolved_image        = VK_NULL_HANDLE;
                        *resolved_image_memory = VK_NULL_HANDLE;
                    }
                }
            }
            else
            {
                GFXRECON_LOG_ERROR("Failed to allocate temporary image memory for multisample resolve");
                device_table_.DestroyImage(device_, *resolved_image, nullptr);
                *resolved_image = VK_NULL_HANDLE;
            }
        }
        else
        {
            GFXRECON_LOG_ERROR(
                "Failed to find a device local memory type for multisample resolve temporary image creation");
            result = VK_ERROR_INITIALIZATION_FAILED;
            device_table_.DestroyImage(device_, *resolved_image, nullptr);
            *resolved_image = VK_NULL_HANDLE;
        }
    }
    else
    {
        GFXRECON_LOG_ERROR("Failed to create temporary image for multisample resolve.");
    }

    return result;
}

VkResult VulkanResourcesUtil::ReadFromImageResource(VkImage                image,
                                                    VkFormat               format,
                                                    VkImageType            type,
                                                    const VkExtent3D&      extent,
                                                    uint32_t               mip_levels,
                                                    uint32_t               array_layers,
                                                    VkImageTiling          tiling,
                                                    VkSampleCountFlags     samples,
                                                    VkImageLayout          layout,
                                                    uint32_t               queue_family_index,
                                                    VkImageAspectFlagBits  aspect,
                                                    std::vector<uint8_t>&  data,
                                                    std::vector<uint64_t>& subresource_offsets,
                                                    std::vector<uint64_t>& subresource_sizes,
                                                    bool                   need_staging_copy,
                                                    bool                   all_layers_per_level)
{
    subresource_offsets.clear();
    subresource_sizes.clear();

    const uint64_t resource_size = GetImageResourceSizes(image,
                                                         format,
                                                         type,
                                                         extent,
                                                         mip_levels,
                                                         array_layers,
                                                         tiling,
                                                         aspect,
                                                         &subresource_offsets,
                                                         &subresource_sizes,
                                                         all_layers_per_level,
                                                         need_staging_copy);

    const VkQueue queue = GetQueue(queue_family_index, 0);
    if (queue == VK_NULL_HANDLE)
    {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkResult       result;
    VkImage        resolve_image  = VK_NULL_HANDLE;
    VkDeviceMemory resolve_memory = VK_NULL_HANDLE;
    if (samples != VK_SAMPLE_COUNT_1_BIT)
    {
        result = ResolveImage(image,
                              format,
                              type,
                              extent,
                              array_layers,
                              layout,
                              queue,
                              queue_family_index,
                              &resolve_image,
                              &resolve_memory);
        if (result != VK_SUCCESS)
        {
            return result;
        }
    }

    result = CreateStagingBuffer(resource_size);
    if (result != VK_SUCCESS)
    {
        return result;
    }

    result = CreateCommandPool(queue_family_index);
    if (result != VK_SUCCESS)
    {
        return result;
    }

    result = CreateCommandBuffer(queue_family_index);
    if (result != VK_SUCCESS)
    {
        return result;
    }

    VkImageAspectFlags transition_aspect = aspect;
    if ((transition_aspect == VK_IMAGE_ASPECT_DEPTH_BIT) || (transition_aspect == VK_IMAGE_ASPECT_STENCIL_BIT))
    {
        // Depth and stencil aspects need to be transitioned together, so get full aspect
        // mask for image.
        transition_aspect = GetFormatAspectMask(format);
    }

    VkImage copy_image = image;
    if (samples != VK_SAMPLE_COUNT_1_BIT)
    {
        copy_image = resolve_image;
    }
    else
    {
        TransitionImageToSrcOptimal(image, layout, transition_aspect);
    }

    CopyImageToBuffer(copy_image,
                      extent,
                      mip_levels,
                      array_layers,
                      aspect,
                      subresource_sizes,
                      staging_buffer_.buffer,
                      all_layers_per_level);

    if ((samples == VK_SAMPLE_COUNT_1_BIT) && (layout != VK_IMAGE_LAYOUT_UNDEFINED) &&
        (layout != VK_IMAGE_LAYOUT_PREINITIALIZED) && (layout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL))
    {
        TransitionImageFromSrcOptimal(image, layout, transition_aspect);
    }

    result = SubmitCommandBuffer(queue);

    result = MapStagingBuffer();
    if (result != VK_SUCCESS)
    {
        return result;
    }

    data.resize(static_cast<size_t>(resource_size));

    InvalidateStagingBuffer();

    util::platform::MemoryCopy(data.data(), resource_size, staging_buffer_.mapped_ptr, resource_size);

    UnmapStagingBuffer();

    if (samples != VK_SAMPLE_COUNT_1_BIT)
    {
        device_table_.DestroyImage(device_, resolve_image, nullptr);
        device_table_.FreeMemory(device_, resolve_memory, nullptr);
    }

    return VK_SUCCESS;
}

VkResult VulkanResourcesUtil::ReadFromBufferResource(VkBuffer              buffer,
                                                     uint64_t              size,
                                                     uint32_t              queue_family_index,
                                                     std::vector<uint8_t>& data)
{
    assert(buffer != VK_NULL_HANDLE);
    assert(size);

    const VkQueue queue = GetQueue(queue_family_index, 0);
    if (queue == VK_NULL_HANDLE)
    {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkResult result = CreateStagingBuffer(size);
    if (result != VK_SUCCESS)
    {
        return result;
    }

    result = CreateCommandPool(queue_family_index);
    if (result != VK_SUCCESS)
    {
        return result;
    }

    result = CreateCommandBuffer(queue_family_index);
    if (result != VK_SUCCESS)
    {
        return result;
    }

    CopyBuffer(buffer, staging_buffer_.buffer, size);

    result = SubmitCommandBuffer(queue);
    if (result != VK_SUCCESS)
    {
        return result;
    }

    result = MapStagingBuffer();
    if (result != VK_SUCCESS)
    {
        return result;
    }

    data.resize(static_cast<size_t>(size));

    InvalidateStagingBuffer();
    util::platform::MemoryCopy(data.data(), size, staging_buffer_.mapped_ptr, size);

    return result;
}

GFXRECON_END_NAMESPACE(gfxrecon)
GFXRECON_END_NAMESPACE(encode)