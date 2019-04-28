#undef __STRICT_ANSI__
#include <windows.h>
#include "cube.h"
#include <vector>

#define no_shenanigans __attribute__((noinline)) __declspec(dllexport)
unsigned int base;
cube::GameController* GameController;

void no_shenanigans UpdatePet(){
    while (!GameController){
        GameController = cube::GetGameController();
        Sleep(100);
    }

    while (GameController && !GameController->shutdown){
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
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            GameController = (cube::GameController*)nullptr;
            base = (unsigned int)GetModuleHandle(NULL);
            cube::SetBase(base);
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UpdatePet, 0, 0, NULL);
            break;

    }
    return TRUE;
}
