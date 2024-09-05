#pragma once

#include <d3d12.h>

// #bfx by Tozic

struct bgfx_commandbuffer
{
    unsigned int m_pos;
    unsigned int m_size;
    unsigned __int8 m_buffer[262144];
};


struct bgfx_frame // Size: 0x2DEE8C0
{
    BUILD_ACCESS(int, m_debug, 0x061CD7D8 - 0x4);
    BUILD_ACCESS(bgfx_commandbuffer, m_cmdPre, 0x061CD7D8);
    BUILD_ACCESS(bgfx_commandbuffer, m_cmdPost, 0x620D7E0);
};


struct bgfx_rendercontexti
{

};

struct bgfx_d3d12_RendererContextD3D12 : bgfx_rendercontexti {
    BUILD_ACCESS(ID3D12CommandQueue*, m_commandQueue, 0x0CC8A90); // Thanks Brady
};

//
struct bgfx_context
{

    BUILD_ACCESS(bgfx_frame*, m_render, 0x0C4F6680);
    BUILD_ACCESS(uintptr_t, m_clearQuad, 0x0C4F6680);
    BUILD_ACCESS(uintptr_t, m_textVideoMemBlitter, 0x0C6C07E0);
    BUILD_ACCESS(bgfx_rendercontexti*, m_renderCtx, 0x0C6D29D0); // 48 8B ? ? ? ? ? 48 8B ? 48 8B ? ? FF 15 ? ? ? ? 48 89 ? ? ? ? ? 48 63
    BUILD_ACCESS(int, m_debug, 0x05D9B64C);
    BUILD_ACCESS(bool, m_rendererInitialized, 0x5D9B758);
    BUILD_ACCESS(bool, m_exit, 0x0C6C086F);
    BUILD_ACCESS(bool, m_flipAfterRender, 0x0C6C086C);
    BUILD_ACCESS(bool, m_singleThreaded, 0x0C6C0871);
    BUILD_ACCESS(bool, m_flipped, 0x0C6C0871 + 0x1);
};