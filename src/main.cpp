#include <SDL2/SDL.h>
#include <SDL2/SDL_mouse.h>
#include <cstdio>
#include "AnmManager.hpp"
#include "Chain.hpp"
#include "FileSystem.hpp"
#include "GameErrorContext.hpp"
#include "GameWindow.hpp"
#include "SoundPlayer.hpp"
#include "Stage.hpp"
#include "Supervisor.hpp"
#include "ZunResult.hpp"
#include "i18n.hpp"
#include "utils.hpp"
// ============================================================
// TH06 iOS / Cross-platform game entry point
//
// NOTE:
// The original main() has been renamed to th06_main().
//
// main_ios.mm will call th06_main() from the iOS entry point.
// ============================================================
int th06_main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    i32 renderResult = 0;
    // --------------------------------------------------------
    // Load configuration
    // --------------------------------------------------------
    if (g_Supervisor.LoadConfig(TH_CONFIG_FILE) != ZUN_SUCCESS)
    {
        g_GameErrorContext.Flush();
        return -1;
    }
restart:
    // --------------------------------------------------------
    // Create SDL game window
    // --------------------------------------------------------
    GameWindow::CreateGameWindow();
    // --------------------------------------------------------
    // Create animation manager
    // --------------------------------------------------------
    g_AnmManager = new AnmManager();
    // --------------------------------------------------------
    // Initialize rendering
    // --------------------------------------------------------
    if (GameWindow::InitD3dRendering() != ZUN_SUCCESS)
    {
        g_GameErrorContext.Flush();
        return 1;
    }
    // --------------------------------------------------------
    // Initialize sound
    // --------------------------------------------------------
    g_SoundPlayer.InitializeDSound();
    // --------------------------------------------------------
    // Initialize controller / keyboard
    // --------------------------------------------------------
    Controller::GetJoystickCaps();
    Controller::ResetKeyboard();
    // --------------------------------------------------------
    // Register game state chain
    // --------------------------------------------------------
    if (Supervisor::RegisterChain() != ZUN_SUCCESS)
    {
        goto stop;
    }
    // --------------------------------------------------------
    // Hide mouse cursor when not running windowed
    // --------------------------------------------------------
    if (!g_Supervisor.cfg.windowed)
    {
        SDL_ShowCursor(SDL_DISABLE);
    }
    g_GameWindow.curFrame = 0;
    // ========================================================
    // Main game loop
    // ========================================================
    while (true)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                goto stop;
            }
        }
        renderResult = g_GameWindow.Render();
        if (renderResult != 0)
        {
            break;
        }
    }
    // ========================================================
    // Shutdown
    // ========================================================
stop:
    g_Chain.Release();
    g_SoundPlayer.Release();
    delete g_AnmManager;
    g_AnmManager = NULL;
    if (g_GfxBackend != NULL)
    {
        delete g_GfxBackend;
        g_GfxBackend = NULL;
    }
    SDL_Quit();
    // --------------------------------------------------------
    // Restart requested by game options
    // --------------------------------------------------------
    if (renderResult == 2)
    {
        g_GameErrorContext.ResetContext();
        g_GameErrorContext.Log(
            TH_ERR_OPTION_CHANGED_RESTART
        );
        if (!g_Supervisor.cfg.windowed)
        {
            SDL_ShowCursor(SDL_ENABLE);
        }
        goto restart;
    }
    // --------------------------------------------------------
    // Save configuration
    // --------------------------------------------------------
    FileSystem::WriteDataToFile(
        TH_CONFIG_FILE,
        &g_Supervisor.cfg,
        sizeof(g_Supervisor.cfg)
    );
    SDL_ShowCursor(SDL_ENABLE);
    g_GameErrorContext.Flush();
    return 0;
}
