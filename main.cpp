#include <iostream>
#include <windows.h>

#include "Graven.h"

int APIENTRY WinMain(HINSTANCE /* hinst */, HINSTANCE /* hinstprev */, PSTR /* cmd_line */, int /* cmd_show */)
{

    std::cout << "VIDEO GAME!\n";

    Graven &graven = Graven::Get();

    return graven.Run();
}