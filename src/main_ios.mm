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
//   3. Verifies TH06 DAT resources
//   4. Starts SDL2's UIKit application system
//   5. Passes control to th06_main()
//
// ============================================================

#define SDL_MAIN_HANDLED

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

#include <unistd.h>
#include <limits.h>
#include <cstdio>

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
        NSLog(@"========================================");
        NSLog(@"TH06 iOS START");
        NSLog(@"========================================");

        // ----------------------------------------------------
        // Get application Bundle resource path
        // ----------------------------------------------------

        NSString *resourcePath =
            [[NSBundle mainBundle] resourcePath];

        if (resourcePath == nil)
        {
            NSLog(@"TH06 ERROR: Bundle resource path is NULL");
        }
        else
        {
            const char *path =
                [resourcePath fileSystemRepresentation];

            if (path == nullptr)
            {
                NSLog(@"TH06 ERROR: Bundle path is NULL");
            }
            else
            {
                NSLog(
                    @"TH06 Bundle Resource Path: %s",
                    path
                );

                // ------------------------------------------------
                // Change current working directory to .app
                // ------------------------------------------------

                if (chdir(path) != 0)
                {
                    NSLog(
                        @"TH06 ERROR: chdir failed: %s",
                        path
                    );
                }
                else
                {
                    NSLog(
                        @"TH06 Working Directory changed to: %s",
                        path
                    );
                }

                // ------------------------------------------------
                // Verify current working directory
                // ------------------------------------------------

                char currentPath[PATH_MAX];

                if (getcwd(
                        currentPath,
                        sizeof(currentPath)
                    ) != nullptr)
                {
                    NSLog(
                        @"TH06 Current Working Directory: %s",
                        currentPath
                    );
                }
                else
                {
                    NSLog(
                        @"TH06 ERROR: getcwd failed"
                    );
                }

                // ------------------------------------------------
                // Verify TH06 DAT files
                // ------------------------------------------------

                const char *datFiles[] =
                {
                    "KOUMAKYO_IN.dat",
                    "KOUMAKYO_ST.dat",
                    "KOUMAKYO_TL.dat",
                    "KOUMAKYO_MD.dat",
                    "KOUMAKYO_ED.dat",
                    "KOUMAKYO_CM.dat"
                };

                const int datCount =
                    sizeof(datFiles) / sizeof(datFiles[0]);

                NSLog(@"========================================");
                NSLog(@"TH06 DAT CHECK");
                NSLog(@"========================================");

                for (int i = 0; i < datCount; ++i)
                {
                    const char *filename =
                        datFiles[i];

                    FILE *fp =
                        fopen(filename, "rb");

                    if (fp != nullptr)
                    {
                        fseek(fp, 0, SEEK_END);

                        long size =
                            ftell(fp);

                        fclose(fp);

                        NSLog(
                            @"TH06 DAT OK: %s (%ld bytes)",
                            filename,
                            size
                        );
                    }
                    else
                    {
                        NSLog(
                            @"TH06 DAT MISSING: %s",
                            filename
                        );
                    }
                }

                NSLog(@"========================================");
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
