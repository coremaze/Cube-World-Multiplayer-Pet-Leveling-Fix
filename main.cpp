#undef __STRICT_ANSI__
#include <windows.h>
#include "cube.h"
#include <vector>

#define no_shenanigans __attribute__((noinline)) __declspec(dllexport)
unsigned int base;
cube::GameController* GameController;

void no_shenanigans UpdatePet(){
    while (true){
        if (GameController == (cube::GameController*)nullptr){
            Sleep(1000);
            continue;
        }
        if (GameController->shutdown){
            break;
        }

        GameController->world.Lock();
        std::vector<cube::Creature*>* creatures = GameController->GetCreatures();
        cube::Creature* player = GameController->GetLocalPlayer();

        for (cube::Creature* creature : *creatures){

            int* petItemXP = (int*)((unsigned int)(player) + 0x1024);
            int* petItemLevel = (int*)((unsigned int)(player) + 0x1030);

            if (player->GUID == creature->parent_GUID){
                //if the entity belongs to the local player

                *petItemXP = creature->XP;
                *petItemLevel = creature->level;

            }
        }
        delete creatures;
        GameController->world.Unlock();
        Sleep(1000);
    }

}


extern "C" __declspec(dllexport) BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    base = (unsigned int)GetModuleHandle(NULL);
    cube::SetBase(base);
    GameController = cube::GetGameController();

    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UpdatePet, 0, 0, NULL);
            break;

    }
    return TRUE;
}
