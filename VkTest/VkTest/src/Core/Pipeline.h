#pragma once

#include <Core/Device.h>

#include <string>
#include <vector>

struct PipelineConfigInfo {
public:
    PipelineConfigInfo();

    PipelineConfigInfo(const PipelineConfigInfo&) = delete;
    PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

    PipelineConfigInfo(PipelineConfigInfo&&) = delete;
    PipelineConfigInfo& operator=(PipelineConfigInfo&&) = delete;
public:
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    std::vector<VkDynamicState> dynamicStateEnables;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo;
    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;
    uint32_t subpass;
};

class Pipeline {
public:
    Pipeline(
        Device& device,
        const std::string& vertFilePath,
        const std::string& fragFilePath,
        const PipelineConfigInfo& info);
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    Pipeline(Pipeline&&) = delete;
    Pipeline& operator=(Pipeline&&) = delete;
public:
    static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
    void Bind(VkCommandBuffer& cmdBuffer);
private:
    static std::vector<char> ReadFile(const std::string& filePath);
    void CreatePipeline(
        const std::string& vertFilePath,
        const std::string& fragFilePath,
        const PipelineConfigInfo& info);
    void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
private:
    Device&			m_Device;
    VkPipeline		m_GraphicsPipeline;
    VkShaderModule	m_VertShaderModule;
    VkShaderModule	m_FragShaderModule;
};