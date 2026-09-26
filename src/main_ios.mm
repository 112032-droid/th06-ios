// ============================================================
// TH06 iOS Entry Point
// ============================================================
//
// iOS entry point for Touhou TH06.
//
// The actual game entry point is:
//
//     th06_main(argc, argv)
//
// This file:
//   1. Gets the application Bundle resource path
//   2. Changes the current working directory to the Bundle
//   3. Starts SDL2's UIKit application system
//   4. Passes control to th06_main()
//
// ============================================================
#define SDL_MAIN_HANDLED
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <unistd.h>
#include <limits.h>
// ============================================================
// TH06 game entry point
//
// Defined in:
//     src/main.cpp
//
// main() in main.cpp was renamed to th06_main()
// ============================================================
extern int th06_main(int argc, char *argv[]);
// ============================================================
// iOS Entry Point
// ============================================================
int main(int argc, char *argv[])
{
    @autoreleasepool
    {
        // ----------------------------------------------------
        // Get application Bundle resource path
        // ----------------------------------------------------
        NSString *resourcePath =
            [[NSBundle mainBundle] resourcePath];
        if (resourcePath != nil)
        {
            const char *path =
                [resourcePath fileSystemRepresentation];
            if (path != nullptr)
            {
                // ------------------------------------------------
                // Make the App Bundle the current working directory.
                //
                // This allows TH06 to find:
                //
                // IN.DAT
                // ST.DAT
                // TL.DAT
                // _MD.DAT
                // _ED.DAT
                // _CM.DAT
                //
                // and directories such as:
                //
                // bgm/
                // data/
                // etc.
                // ------------------------------------------------
                if (chdir(path) != 0)
                {
                    NSLog(
                        @"TH06: Failed to change working directory: %s",
                        path
                    );
                }
                else
                {
                    NSLog(
                        @"TH06: Working directory: %s",
                        path
                    );
                }
            }
        }
        // ----------------------------------------------------
        // Start SDL2 UIKit application
        //
        // SDL_UIKitRunApp() creates the iOS application
        // environment and calls th06_main().
        // ----------------------------------------------------
        return SDL_UIKitRunApp(
            argc,
            argv,
            th06_main
        );
    }
}
