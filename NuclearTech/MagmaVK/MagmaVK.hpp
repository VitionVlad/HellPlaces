
#include <iostream>

#include <vector>

#include <array>

#include <string>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtx/transform.hpp>

#include "Reader.hpp"

using namespace std;

using namespace glm;

struct uniformbuf{
    mat4 proj;
    mat4 view;
    mat4 model;
    mat4 lightmat;
    vec4 massive[100];
    vec3 pPos;
};

struct vertexbuf{
    vec4 vertexpos;
    ivec3 vertexcol;
    vec3 normals;
    vec2 uv;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(vertexbuf);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }
    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(vertexbuf, vertexpos);
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SINT;
        attributeDescriptions[1].offset = offsetof(vertexbuf, vertexcol);
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(vertexbuf, normals);
        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(vertexbuf, uv);
        return attributeDescriptions;
    }
};

unsigned char pixels[99999999] = {0};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

vertexbuf vertex[999999] = {
    {vec4(0, 0, 0, 0), ivec3(0, 0, 0), vec3(0, 0, 0), vec2(0, 0)}
};

class MagmaVK{
    public:
    GLFWwindow* window;
    ivec2 resolution = {1280, 720};
    ivec2 oldresolution = {1280, 720};
    vec2 rot;
    vec3 pos;
    VkInstance instance{};
    VkPhysicalDevice physdevice;
    VkDevice device;
    VkSurfaceKHR Surface{};
    VkSwapchainKHR swapchain;
    VkPhysicalDeviceProperties physprop;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkImage> swapChainImages;
    VkViewport viewport;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout[100];
    VkRenderPass renderPass;
    VkPipeline pipeline[100];
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    uniformbuf ubo;
    const int prerenderframes = 2;
    uint32_t currentFrame = 0;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
    VkImage depthImage;
    VkDeviceMemory depthImageMemory{};
    VkImageView depthImageView;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    VkImage textureImage;
    VkDeviceMemory textureImageMemory{};
    VkImageView textureImageView;
    VkSampler textureSampler;

    VkImage cubemapImage;
    VkDeviceMemory cubemapImageMemory{};
    VkImageView cubemapImageView;
    VkSampler cubemapSampler;

    VkImage shadowImage;
    VkDeviceMemory shadowImageMemory{};
    VkImageView shadowImageView;
    VkImage shadowcImage;
    VkDeviceMemory shadowcImageMemory{};
    VkImageView shadowcImageView;
    VkSampler shadowSampler;
    VkFramebuffer shadowframebuffer;

    VkImage PredImage;
    VkDeviceMemory PredImageMemory{};
    VkImageView PredImageView;
    VkImage PreImage;
    VkDeviceMemory PreImageMemory{};
    VkImageView PreImageView;
    VkSampler PreSampler;
    VkFramebuffer Preframebuffer;
    ivec2 RenderResolution = resolution;

    string vPostPath = "App/Raw/pvert.spv";
    string fPostPath = "App/Raw/pfrag.spv";

    int shadowmapresolution = 800;

    string fshaderpath[100] = {
        "Engine/raw/frag.spv"
    };
    string vshaderpath[100] = {
        "Engine/raw/vert.spv"
    };
    bool enablepipeline[100] = {
        true
    };
    VkPolygonMode pippolymode[100] = {
        VK_POLYGON_MODE_FILL
    };
    VkCullModeFlags cullmode[100] = {
        VK_CULL_MODE_BACK_BIT
    };
    int pipelinecounts = 1;
    float fov = 110;
    int totalv = 3;
    vec3 clearcol = vec3(0, 0, 0);
    string TexToLoad[100] = {
        "App/Models/image.ppm",
        "App/Models/SpecularMap.ppm"
    };
    int TexToLoadCnt = 2;
    int totaltex;
    ivec2 tResolution;
    bool preloadtextures = true;
    VkDebugUtilsMessengerEXT debugMessenger;
    void* vertexdata;
    bool enablelayers = false;
    string vshadowpath = "App/Raw/smvert.spv";
    string fshadowpath = "App/Raw/smfrag.spv";
    VkPipeline shadowpipeline;
    VkPipelineLayout Shadowpipelinelayout;
    VkPipeline FinalPipeline;
    VkPipelineLayout Finalpipelinelayout;
    bool enableshadowmaps = true;
    bool useshadowmaps = true;
    string cubemapfaces[6] = {
        "App/Textures/2.ppm",
        "App/Textures/1.ppm",
        "App/Textures/4.ppm",
        "App/Textures/3.ppm",
        "App/Textures/5.ppm",
        "App/Textures/6.ppm"
    };
    bool preloadcubemaps = true;
    VkSampler PredSampler;
    void CreateInstance(){
        VkApplicationInfo appinfo;
        appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appinfo.apiVersion = VK_API_VERSION_1_2;
        appinfo.pNext = nullptr;
        appinfo.pApplicationName = "NuclearTech";
        appinfo.pEngineName = "NuclearTechVK";
        appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appinfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        VkInstanceCreateInfo instinfo{};
        instinfo.pApplicationInfo = &appinfo;
        instinfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        uint32 extcount;
        const char** ext = glfwGetRequiredInstanceExtensions(&extcount);
        for(int i = 0; i!= extcount; i++){
            cout << "Log:Enabling Instance Extension:" << ext[i] << endl;
        }
        std::vector<const char*> extensions(ext, ext + extcount);
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        instinfo.ppEnabledExtensionNames = extensions.data();
        instinfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());

        uint32 layercont = 0;
        vector<VkLayerProperties> lprop{};
        vkEnumerateInstanceLayerProperties(&layercont, nullptr);
        lprop.resize(layercont);
        vkEnumerateInstanceLayerProperties(&layercont, lprop.data());
        vector<char *>layerNames{};
        layerNames.resize(layercont);
        for(int i = 0; i!= layercont; i++){
            layerNames[i] = lprop[i].layerName;
            cout << "Log:Enabling Instance Layer:" << lprop[i].layerName << endl;
        }

        cout << "Log:Total layer count: " << layercont << endl;

        if(enablelayers == false){
            layercont = 0;
        }

        instinfo.ppEnabledLayerNames = layerNames.data();
        instinfo.enabledLayerCount = layercont;
        vkCreateInstance(&instinfo, nullptr, &instance);
        cout << "Log:instance created" << endl;
    }
    void CreateDevice(){
        uint32 devicecount;
        vkEnumeratePhysicalDevices(instance, &devicecount, nullptr);
        cout << "Device Count:" << devicecount << endl;
        vkEnumeratePhysicalDevices(instance, &devicecount, &physdevice);
        vkGetPhysicalDeviceProperties(physdevice, &physprop);
        cout << "Device Name: " << physprop.deviceName << endl;
        cout << "Device Type: " << physprop.deviceType << endl;
        cout << "Api version: " << physprop.apiVersion << endl;
        
        VkDeviceQueueCreateInfo queueinfo = {};
        const float prior[1] = {1.0f};
       queueinfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
       queueinfo.pNext = NULL;
       queueinfo.queueCount = 1;
       queueinfo.pQueuePriorities = prior;

        const char* extname = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

        uint32_t extensioncount = 0;
        vector<VkExtensionProperties> extensionproprites{};
        vkEnumerateDeviceExtensionProperties(physdevice, nullptr, &extensioncount, nullptr);
        extensionproprites.resize(extensioncount);
        vkEnumerateDeviceExtensionProperties(physdevice, nullptr, &extensioncount, extensionproprites.data());
        vector<char*> extensionNames{};
        extensionNames.resize(extensioncount);
        for(int i = 0; i!= extensioncount; i++){
            extensionNames[i] = extensionproprites[i].extensionName;
            cout << "Log:Enabling Device Extension:" << extensionproprites[i].extensionName << endl;
        }

        VkDeviceCreateInfo deviceinfo = {};
        deviceinfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceinfo.flags = 0;
        deviceinfo.enabledExtensionCount = extensioncount;
        deviceinfo.ppEnabledExtensionNames = extensionNames.data();
        deviceinfo.enabledLayerCount = 0;
        deviceinfo.ppEnabledLayerNames = nullptr;
        deviceinfo.queueCreateInfoCount = 1;
        deviceinfo.pQueueCreateInfos = &queueinfo;
        vkCreateDevice(physdevice, &deviceinfo, nullptr, &device);
        vkGetDeviceQueue(device, queueinfo.queueFamilyIndex, 0, &graphicsQueue);
        vkGetDeviceQueue(device, queueinfo.queueFamilyIndex, 0, &presentQueue);
        cout << "Log:Device created" << endl;
    }
    void CreateSwapChain(bool dbginfo){
        VkSurfaceCapabilitiesKHR capap;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physdevice, Surface, &capap);
        uint32 surfcont = 0;
        VkSurfaceFormatKHR form{};
        vkGetPhysicalDeviceSurfaceFormatsKHR(physdevice, Surface, &surfcont, &form);
        uint32_t imageCount = capap.minImageCount + 1;
        VkSwapchainCreateInfoKHR swapinfo = {};
        swapinfo.clipped = 1;
        swapinfo.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
        swapinfo.imageArrayLayers = capap.maxImageArrayLayers;
        swapinfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        swapinfo.imageExtent.height = resolution.y;
        swapinfo.imageExtent.width = resolution.x;
        swapinfo.imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
        swapinfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapinfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT ;
        swapinfo.minImageCount = capap.minImageCount;
        swapinfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        swapinfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapinfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapinfo.surface = Surface;
        swapinfo.oldSwapchain = VK_NULL_HANDLE;
        if(dbginfo == 1){
            cout << "Log:SwapChainCreateInfo Filled" << endl;
        }
        vkCreateSwapchainKHR(device, &swapinfo, nullptr, &swapchain);
        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapChainImages.data());
        if(dbginfo == 1){
            cout << "Log:SwapChain Created" << endl;
        }
    }
    void CreateImageViews(bool dbginfo){
        swapChainImageViews.resize(swapChainImages.size());
        for(size_t i = 0; i < swapChainImages.size(); i++){
            VkImageViewCreateInfo imginfo = {};
            imginfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imginfo.image = swapChainImages[i];
            imginfo.format = VK_FORMAT_R8G8B8A8_SRGB;
            imginfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imginfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imginfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imginfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imginfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            imginfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imginfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imginfo.subresourceRange.baseMipLevel = 0;
            imginfo.subresourceRange.levelCount = 1;
            imginfo.subresourceRange.baseArrayLayer = 0;
            imginfo.subresourceRange.layerCount = 1;
            vkCreateImageView(device, &imginfo, nullptr, &swapChainImageViews[i]);
            if(dbginfo == 1){
                cout << "Log:Image View " << i << " Created" << endl;
            }
        }
    };
    VkShaderModule shadermodule(const std::vector<char>& code){
        VkShaderModuleCreateInfo shadrinfo;
        shadrinfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shadrinfo.codeSize = code.size();
        shadrinfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
        shadrinfo.pNext = nullptr;
        shadrinfo.flags = 0;
        VkShaderModule shaderm;
        vkCreateShaderModule(device, &shadrinfo, nullptr, &shaderm);
        return shaderm;
    }
    void CreatePipeline(bool dbginfo, VkPipeline *pipeline, VkPipelineLayout &pipelineLayout, string vshaderpath, string fshaderpath, VkPolygonMode polygonMode, VkCullModeFlags cullmode, ivec2 resolution){
       auto vertShaderCode = loadbin(vshaderpath);
        auto fragShaderCode = loadbin(fshaderpath);
        VkShaderModule vertShaderModule = shadermodule(vertShaderCode);
        VkShaderModule fragShaderModule = shadermodule(fragShaderCode);
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";
        vertShaderStageInfo.pNext = nullptr;
        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";
        fragShaderStageInfo.pNext = nullptr;
        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        auto bindingDescription = vertexbuf::getBindingDescription();
        auto attributeDescriptions = vertexbuf::getAttributeDescriptions();
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) resolution.x;
        viewport.height = (float) resolution.y;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent.width = (float) resolution.x;
        scissor.extent.height = (float) resolution.y;
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = polygonMode;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = cullmode;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
        if(dbginfo == 1){
            cout << "Log:Pipeline Layout Created" << endl;
        }
        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, pipeline);
        if(dbginfo == 1){
            cout << "Log:Pipeline Created" << endl;
        }
        vkDestroyShaderModule(device, fragShaderModule, nullptr);
        vkDestroyShaderModule(device, vertShaderModule, nullptr);
    }
    void CreateRenderPass(bool dbginfo){
        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = VK_FORMAT_D32_SFLOAT;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        VkAttachmentDescription colatach = {};
        colatach.format = VK_FORMAT_R8G8B8A8_SRGB;
        colatach.samples = VK_SAMPLE_COUNT_1_BIT;
        colatach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colatach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colatach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colatach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colatach.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colatach.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        std::array<VkAttachmentDescription, 2> attachments = {colatach, depthAttachment};
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;
        vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
        if(dbginfo == 1){
            cout << "Log:Render Pass Created" << endl;
        }
    }
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, uint32_t arraylayers) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = arraylayers;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        vkCreateImage(device, &imageInfo, nullptr, &image);
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physdevice, &memProperties);
        uint32 finmem;
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if (memRequirements.memoryTypeBits & (1 << i)) {
                finmem = i;
                break;
            }
        }
        allocInfo.memoryTypeIndex = finmem;
        vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory);
        vkBindImageMemory(device, image, imageMemory, 0);
    }
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t layercount, VkImageViewType imgType) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = imgType;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = layercount;
        VkImageView imageView;
        if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
        return imageView;
    }
    void CreateDepthRes(){
        createImage(resolution.x, resolution.y, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory, 1);
        depthImageView = createImageView(depthImage, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT, 1, VK_IMAGE_VIEW_TYPE_2D);
        createImage(shadowmapresolution, shadowmapresolution, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, shadowImage, shadowImageMemory, 1);
        shadowImageView = createImageView(shadowImage, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT, 1, VK_IMAGE_VIEW_TYPE_2D);
        createImage(shadowmapresolution, shadowmapresolution, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, shadowcImage, shadowcImageMemory, 1);
        shadowcImageView = createImageView(shadowcImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, 1, VK_IMAGE_VIEW_TYPE_2D);
        createImage(RenderResolution.x, RenderResolution.y, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, PredImage, PredImageMemory, 1);
        PredImageView = createImageView(PredImage, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT, 1, VK_IMAGE_VIEW_TYPE_2D);
        createImage(RenderResolution.x, RenderResolution.y, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, PreImage, PreImageMemory, 1);
        PreImageView = createImageView(PreImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, 1, VK_IMAGE_VIEW_TYPE_2D);
    }
    void CreateVertexInput(){
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = 999999*64;
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer);
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physdevice, &memProperties);
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        uint32 finmem;
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if (memRequirements.memoryTypeBits & (1 << i)) {
                finmem = i;
                break;
            }
        }
        allocInfo.memoryTypeIndex = finmem;
        vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory);
        vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);
        vkMapMemory(device, vertexBufferMemory, 0, bufferInfo.size, 0, &vertexdata);
    }
    void updateVertexBuffer(vertexbuf *vertex){
        memcpy(vertexdata, vertex, (size_t) totalv*64);
    }
    void createDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        VkDescriptorSetLayoutBinding shadowbind{};
        shadowbind.binding = 2;
        shadowbind.descriptorCount = 1;
        shadowbind.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        shadowbind.pImmutableSamplers = nullptr;
        shadowbind.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        VkDescriptorSetLayoutBinding PreSampler{};
        PreSampler.binding = 3;
        PreSampler.descriptorCount = 1;
        PreSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        PreSampler.pImmutableSamplers = nullptr;
        PreSampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        VkDescriptorSetLayoutBinding cubemapsm{};
        cubemapsm.binding = 4;
        cubemapsm.descriptorCount = 1;
        cubemapsm.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        cubemapsm.pImmutableSamplers = nullptr;
        cubemapsm.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        VkDescriptorSetLayoutBinding depthmap{};
        depthmap.binding = 5;
        depthmap.descriptorCount = 1;
        depthmap.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        depthmap.pImmutableSamplers = nullptr;
        depthmap.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        std::array<VkDescriptorSetLayoutBinding, 6> bindings = {uboLayoutBinding, samplerLayoutBinding, shadowbind, PreSampler, cubemapsm, depthmap};
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();
        vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout);
    }
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physdevice, &memProperties);
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        uint32 finmem;
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if (memRequirements.memoryTypeBits & (1 << i)) {
                finmem = i;
                break;
            }
        }
        allocInfo.memoryTypeIndex = finmem;
        if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }
        vkBindBufferMemory(device, buffer, bufferMemory, 0);
    }
    void createUniformBuffers() {
        VkDeviceSize bufferSize = sizeof(uniformbuf);

        uniformBuffers.resize(prerenderframes);
        uniformBuffersMemory.resize(prerenderframes);

        for (size_t i = 0; i < prerenderframes; i++) {
            createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
        }
    }
    void CreateDescriptoPool(){
       std::array<VkDescriptorPoolSize, 6> poolSizes{};
       poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
       poolSizes[0].descriptorCount = static_cast<uint32_t>(prerenderframes);
       poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
       poolSizes[1].descriptorCount = static_cast<uint32_t>(prerenderframes);
       poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
       poolSizes[2].descriptorCount = static_cast<uint32_t>(prerenderframes);
       poolSizes[3].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
       poolSizes[3].descriptorCount = static_cast<uint32_t>(prerenderframes);
       poolSizes[4].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
       poolSizes[4].descriptorCount = static_cast<uint32_t>(prerenderframes);
       poolSizes[5].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
       poolSizes[5].descriptorCount = static_cast<uint32_t>(prerenderframes);
       VkDescriptorPoolCreateInfo poolInfo{};
       poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
       poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
       poolInfo.pPoolSizes = poolSizes.data();
       poolInfo.maxSets = static_cast<uint32_t>(prerenderframes);
        vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool);
    }
    void createDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(prerenderframes, descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(prerenderframes);
        allocInfo.pSetLayouts = layouts.data();
        descriptorSets.resize(prerenderframes);
        if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }
        for (size_t i = 0; i < prerenderframes; i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(ubo);
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = textureImageView;
            imageInfo.sampler = textureSampler;
            VkDescriptorImageInfo shadowinfo{};
            shadowinfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            shadowinfo.imageView = shadowImageView;
            shadowinfo.sampler = shadowSampler;
            VkDescriptorImageInfo PreInfo{};
            PreInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            PreInfo.imageView = PreImageView;
            PreInfo.sampler = PreSampler;
            VkDescriptorImageInfo cubemapinfo{};
            cubemapinfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            cubemapinfo.imageView = cubemapImageView;
            cubemapinfo.sampler = cubemapSampler;
            VkDescriptorImageInfo PredInfo{};
            PredInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            PredInfo.imageView = PredImageView;
            PredInfo.sampler = PreSampler;
            std::array<VkWriteDescriptorSet, 6> descriptorWrites{};
            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;
            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;
            descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[2].dstSet = descriptorSets[i];
            descriptorWrites[2].dstBinding = 2;
            descriptorWrites[2].dstArrayElement = 0;
            descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[2].descriptorCount = 1;
            descriptorWrites[2].pImageInfo = &shadowinfo;
            descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[3].dstSet = descriptorSets[i];
            descriptorWrites[3].dstBinding = 3;
            descriptorWrites[3].dstArrayElement = 0;
            descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[3].descriptorCount = 1;
            descriptorWrites[3].pImageInfo = &PreInfo;
            descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[4].dstSet = descriptorSets[i];
            descriptorWrites[4].dstBinding = 4;
            descriptorWrites[4].dstArrayElement = 0;
            descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[4].descriptorCount = 1;
            descriptorWrites[4].pImageInfo = &cubemapinfo;
            descriptorWrites[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[5].dstSet = descriptorSets[i];
            descriptorWrites[5].dstBinding = 5;
            descriptorWrites[5].dstArrayElement = 0;
            descriptorWrites[5].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[5].descriptorCount = 1;
            descriptorWrites[5].pImageInfo = &PredInfo;
            vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }
    void CreateFramebuffer(bool dbginfo){
        std::array<VkImageView, 2> attachments = {
            shadowcImageView,
            shadowImageView
        };
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = shadowmapresolution;
        framebufferInfo.height = shadowmapresolution;
        framebufferInfo.layers = 1;
        vkCreateFramebuffer(device, &framebufferInfo, nullptr, &shadowframebuffer);
        std::array<VkImageView, 2> pattachments = {
            PreImageView,
            PredImageView
        };
        VkFramebufferCreateInfo preframebufferInfo{};
        preframebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        preframebufferInfo.renderPass = renderPass;
        preframebufferInfo.attachmentCount = static_cast<uint32_t>(pattachments.size());
        preframebufferInfo.pAttachments = pattachments.data();
        preframebufferInfo.width = RenderResolution.x;
        preframebufferInfo.height = RenderResolution.y;
        preframebufferInfo.layers = 1;
        vkCreateFramebuffer(device, &preframebufferInfo, nullptr, &Preframebuffer);
        if(dbginfo == 1){
            cout << "Log:Shadowmap Framebuffer Created" << endl;
        }
        swapChainFramebuffers.resize(swapChainImageViews.size());
        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            std::array<VkImageView, 2> attachments = {
                swapChainImageViews[i],
                depthImageView
            };
            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = resolution.x;
            framebufferInfo.height = resolution.y;
            framebufferInfo.layers = 1;
            vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]);
            if(dbginfo == 1){
                cout << "Log:Framebuffer " << i << " Created" << endl;
            }
        }
    }
    void CreateCommandBuffer(){
        commandBuffers.resize(prerenderframes);
        VkCommandPoolCreateInfo compoolinfo{};
        compoolinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        compoolinfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        compoolinfo.queueFamilyIndex = 1;
        compoolinfo.pNext = nullptr;
        vkCreateCommandPool(device, &compoolinfo, nullptr, &commandPool);
        cout << "Log:Command Pool Created" << endl;
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();
        vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data());
        cout << "Log:Command Buffer Allocated" << endl;
    }
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.height = resolution.y;
        renderPassInfo.renderArea.extent.width = resolution.x;
        VkRenderPassBeginInfo shadowPass{};
        shadowPass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        shadowPass.renderPass = renderPass;
        shadowPass.framebuffer = shadowframebuffer;
        shadowPass.renderArea.offset = {0, 0};
        shadowPass.renderArea.extent.height = shadowmapresolution;
        shadowPass.renderArea.extent.width = shadowmapresolution;
        VkRenderPassBeginInfo MainPass{};
        MainPass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        MainPass.renderPass = renderPass;
        MainPass.framebuffer = Preframebuffer;
        MainPass.renderArea.offset = {0, 0};
        MainPass.renderArea.extent.height = RenderResolution.y;
        MainPass.renderArea.extent.width = RenderResolution.x;
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{clearcol.x, clearcol.y, clearcol.z, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();
        shadowPass.clearValueCount = static_cast<uint32_t>(clearValues.size());
        shadowPass.pClearValues = clearValues.data();
        MainPass.clearValueCount = static_cast<uint32_t>(clearValues.size());
        MainPass.pClearValues = clearValues.data();
        VkBuffer vertexBuffers[] = {vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBeginRenderPass(commandBuffer, &shadowPass, VK_SUBPASS_CONTENTS_INLINE);

        if(enableshadowmaps == true && useshadowmaps == true){
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowpipeline);
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Shadowpipelinelayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
            vkCmdDraw(commandBuffer, totalv, 1, 0, 0);
        }

        vkCmdEndRenderPass(commandBuffer);
        vkCmdBeginRenderPass(commandBuffer, &MainPass, VK_SUBPASS_CONTENTS_INLINE);

        for(int i = 0; i!= pipelinecounts; i++){
            if(enablepipeline[i] == true){
                vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline[i]);
                vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout[i], 0, 1, &descriptorSets[currentFrame], 0, nullptr);
                vkCmdDraw(commandBuffer, totalv, 1, 0, 0);
            }
        }

        vkCmdEndRenderPass(commandBuffer);
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, FinalPipeline);
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Finalpipelinelayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
        vkCmdDraw(commandBuffer, totalv, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffer);
        vkEndCommandBuffer(commandBuffer);
    }
    void cleanupSwapChain() {
        vkDestroyImage(device, depthImage, nullptr);
        vkFreeMemory(device, depthImageMemory, nullptr);
        vkDestroyImageView(device, depthImageView, nullptr);

        vkDestroyImage(device, shadowImage, nullptr);
        vkFreeMemory(device, shadowImageMemory, nullptr);
        vkDestroyImageView(device, shadowImageView, nullptr);

        vkDestroyImage(device, shadowcImage, nullptr);
        vkFreeMemory(device, shadowcImageMemory, nullptr);
        vkDestroyImageView(device, shadowcImageView, nullptr);

        vkDestroyImage(device, PredImage, nullptr);
        vkFreeMemory(device, PredImageMemory, nullptr);
        vkDestroyImageView(device, PredImageView, nullptr);

        vkDestroyImage(device, PreImage, nullptr);
        vkFreeMemory(device, PreImageMemory, nullptr);
        vkDestroyImageView(device, PreImageView, nullptr);

        for (auto framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }
        vkDestroyFramebuffer(device, shadowframebuffer, nullptr);
        vkDestroyFramebuffer(device, Preframebuffer, nullptr);

        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(device, swapchain, nullptr);

        for(int i = 0; i!= pipelinecounts; i++){
            vkDestroyPipeline(device, pipeline[i], nullptr);
            vkDestroyPipelineLayout(device, pipelineLayout[i], nullptr);
        }

        vkDestroyPipeline(device, shadowpipeline, nullptr);
        vkDestroyPipelineLayout(device, Shadowpipelinelayout, nullptr);

        vkDestroyPipeline(device, FinalPipeline, nullptr);
        vkDestroyPipelineLayout(device, Finalpipelinelayout, nullptr);

        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    }
    void Renewswap(){
        cleanupSwapChain();
        vkDeviceWaitIdle(device);
        CreateSwapChain(0);
        CreateImageViews(0);
        CreateDepthRes();
        CreateFramebuffer(0);
        createDescriptorSetLayout();
        if(enableshadowmaps == true){
            CreatePipeline(0, &shadowpipeline, Shadowpipelinelayout, vshadowpath, fshadowpath, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, ivec2(shadowmapresolution));
        }
        for(int i = 0; i!= pipelinecounts;i++){
            CreatePipeline(0, &pipeline[i], pipelineLayout[i], vshaderpath[i], fshaderpath[i], pippolymode[i], cullmode[i], RenderResolution);
        }
        CreatePipeline(0, &FinalPipeline, Finalpipelinelayout, vPostPath, fPostPath, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, resolution);
        CreateDescriptoPool();
        createDescriptorSets();
    }
    void calculatematrix(float fov){
        ubo.proj = perspective(radians(fov), float(resolution.x / resolution.y), 0.1f, 300.0f);
        ubo.view = rotate(mat4(1.0f), rot.y, vec3(1, 0, 0));
        ubo.view = rotate(ubo.view, rot.x, vec3(0, 1, 0));
        ubo.view = translate(ubo.view, pos);
        ubo.model = mat4(1.0f);
    }
    void updateUniformBuffer(float fov, uint32_t currentImage) {
        calculatematrix(fov);
        ubo.pPos = pos;
        void* data;
        vkMapMemory(device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device, uniformBuffersMemory[currentImage]);
    }
    void Draw(vertexbuf *vertex){
        glfwGetFramebufferSize(window, &resolution.x, &resolution.y);
        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &inFlightFences[currentFrame]);
        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
        while (resolution.x == 0 || resolution.y == 0) {
            glfwGetFramebufferSize(window, &resolution.x, &resolution.y);
            glfwWaitEvents();
        }
        if(result == VK_ERROR_OUT_OF_DATE_KHR || oldresolution != resolution){
            oldresolution = resolution;
            Renewswap();
        }
        vkResetCommandBuffer(commandBuffers[currentFrame], 0);
        recordCommandBuffer(commandBuffers[currentFrame], imageIndex);
        updateUniformBuffer(fov, currentFrame);
        updateVertexBuffer(vertex);
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
        vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]);
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        VkSwapchainKHR swapChains[] = {swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        vkQueuePresentKHR(presentQueue, &presentInfo);
        currentFrame = (currentFrame + 1) % prerenderframes;
    }
    void CreateSync(){
        imageAvailableSemaphores.resize(prerenderframes);
        renderFinishedSemaphores.resize(prerenderframes);
        inFlightFences.resize(prerenderframes);
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        for (size_t i = 0; i < prerenderframes; i++) {
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
            vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]);
        }
    }
    VkCommandBuffer beginSingleTimeCommands() {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;
        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        return commandBuffer;
    }
    void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
        vkEndCommandBuffer(commandBuffer);
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);
        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();
        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
        endSingleTimeCommands(commandBuffer);
    }
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = 0;
        vkCmdPipelineBarrier(commandBuffer, 0, 0, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        endSingleTimeCommands(commandBuffer);
    }
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 100;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {width, height, 1};
        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        endSingleTimeCommands(commandBuffer);
    }
    void createTextureImageView() {
        textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, 100, VK_IMAGE_VIEW_TYPE_2D_ARRAY);
    }
    void createTextureSampler(VkSampler& textureSampler) {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(physdevice, &properties);
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;
        if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }
    void createTexture(string *path, int cnt){
        for(int i = 0; i!=cnt; i++){
            readImage(pixels, tResolution.x, tResolution.y, path[i].c_str());
        }
        VkDeviceSize imgsize = 99999999;
        createBuffer(imgsize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, imgsize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imgsize));
        vkUnmapMemory(device, stagingBufferMemory);
        createImage(tResolution.x, tResolution.y, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory, 100);
        transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(tResolution.x), static_cast<uint32_t>(tResolution.y));
        transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        createTextureImageView();
    }
    void createTextureCubeMap(string *path, int cnt){
        begtexpos = 0;
        for(int i = 0; i!=cnt; i++){
            readImage(pixels, tResolution.x, tResolution.y, path[i].c_str());
        }
        VkDeviceSize imgsize = 99999999;
        createBuffer(imgsize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, imgsize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imgsize));
        vkUnmapMemory(device, stagingBufferMemory);
        createImage(tResolution.x, tResolution.y, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, cubemapImage, cubemapImageMemory, 6);
        transitionImageLayout(cubemapImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(stagingBuffer, cubemapImage, static_cast<uint32_t>(tResolution.x), static_cast<uint32_t>(tResolution.y));
        transitionImageLayout(cubemapImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        cubemapImageView = createImageView(cubemapImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, 6, VK_IMAGE_VIEW_TYPE_CUBE);
    }
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }
    void setupDGBcallback(){
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;
        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("Error:Failed to setup error callback");
        }
        cout << "Log:Error callback setup complete" << endl;

    }
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }
    void Init(){
        CreateInstance();
        setupDGBcallback();
        glfwCreateWindowSurface(instance, window, nullptr, &Surface);
        CreateDevice();
        CreateSwapChain(1);
        CreateImageViews(1);
        CreateRenderPass(1);
        createDescriptorSetLayout();
        CreateDepthRes();
        CreateFramebuffer(1);
        CreateVertexInput();
        cout << "Log:Vertex inputs created" << endl;
        CreateCommandBuffer();
        if(enableshadowmaps == true){
            CreatePipeline(1, &shadowpipeline, Shadowpipelinelayout, vshadowpath, fshadowpath, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, ivec2(shadowmapresolution));
        }
        for(int i = 0; i!= pipelinecounts;i++){
            CreatePipeline(1, &pipeline[i], pipelineLayout[i], vshaderpath[i], fshaderpath[i], pippolymode[i], cullmode[i], RenderResolution);
        }
        CreatePipeline(1, &FinalPipeline, Finalpipelinelayout, vPostPath, fPostPath, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, resolution);
        if(preloadtextures == true){
            createTexture(TexToLoad, TexToLoadCnt);
        }
        if(preloadcubemaps == true){
            createTextureCubeMap(cubemapfaces, 6);
        }
        createTextureSampler(cubemapSampler);
        createTextureSampler(shadowSampler);
        createTextureSampler(PreSampler);
        createTextureSampler(textureSampler);
        createTextureSampler(PredSampler);
        cout << "Log:Textures Load" << endl;
        createUniformBuffers();
        cout << "Log:Uniform buffers created" << endl;
        CreateDescriptoPool();
        cout << "Log:Descriptor pool created" << endl;
        createDescriptorSets();
        cout << "Log:Descriptor sets created" << endl;
        CreateSync();
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
    void Destroy(){
        vkUnmapMemory(device, vertexBufferMemory);
        cleanupSwapChain();
        vkDestroySampler(device, shadowSampler, nullptr);
        vkDestroySampler(device, PreSampler, nullptr);
        vkDestroySampler(device, PredSampler, nullptr);
        vkDestroyRenderPass(device, renderPass, nullptr);
        for (size_t i = 0; i < prerenderframes; i++) {
            vkDestroyBuffer(device, uniformBuffers[i], nullptr);
            vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
        }
        vkDestroySampler(device, textureSampler, nullptr);
        vkDestroyImageView(device, textureImageView, nullptr);
        vkDestroyImage(device, textureImage, nullptr);
        vkFreeMemory(device, textureImageMemory, nullptr);
        vkDestroyBuffer(device, vertexBuffer, nullptr);
        vkFreeMemory(device, vertexBufferMemory, nullptr);
        for (size_t i = 0; i < prerenderframes; i++) {
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(device, commandPool, nullptr);
        vkDestroyDevice(device, nullptr);
        vkDestroySurfaceKHR(instance, Surface, nullptr);
        vkDestroyInstance(instance, nullptr);
    }
};
