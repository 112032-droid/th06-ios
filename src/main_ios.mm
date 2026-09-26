// ============================================================
// TH06 iOS Entry Point
// ============================================================
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#include <unistd.h>
#include <limits.h>
int main(int argc, char *argv[])
{
    @autoreleasepool
    {
        // ----------------------------------------------------
        // Get application bundle resource path
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
                // This allows the game to find files such as:
                //
                // IN.DAT
                // ST.DAT
                // TL.DAT
                // _MD.DAT
                // _ED.DAT
                // _CM.DAT
                // ------------------------------------------------
                chdir(path);
            }
        }
        // ----------------------------------------------------
        // Start iOS application
        // ----------------------------------------------------
        return UIApplicationMain(
            argc,
            argv,
            nil,
            nil
        );
    }
}
