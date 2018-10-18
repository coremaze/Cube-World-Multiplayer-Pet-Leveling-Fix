#include "main.h"

UINT_PTR base;

DWORD oldESP;
DWORD oldEBP;
signed int updateDue = -1;

__declspec(naked) void ASMCreatureUpdateInjection(){
    asm("push edi");
    asm("mov eax, [esi]");
    asm("mov edi, ecx");

    asm("push eax\n push ebx\n push ecx\n push edx\n push esi\n push edi");
    asm("mov [_oldESP], esp");
    asm("mov [_oldEBP], ebp");

    asm("push esi");
    asm("call [_UpdatePetPtr]");

    asm("mov esp, [_oldESP]");
    asm("mov ebp, [_oldEBP]");


    asm("pop edi\n pop esi\n pop edx\n pop ecx\n pop ebx\n pop eax");

    asm("mov ecx, [_base]"); //jump back
    asm("add ecx, 0x6605C");
    asm("jmp ecx");
}


void UpdatePet(DWORD creatureaddr){

    //pointer hell
    DWORD entityaddr = (DWORD)(base + 0x36b1c8);
    entityaddr = *(DWORD*)entityaddr;
    entityaddr += 0x39C;
    entityaddr = *(DWORD*)entityaddr;

    unsigned int* playerGUID = (unsigned int*)(entityaddr+0x08);
    unsigned int* ownerGUID = (unsigned int*)(creatureaddr+0x198 - 0x10);

    unsigned int* petLevel = (unsigned int*)(creatureaddr+0x190 - 0x10);
    unsigned int* petXP = (unsigned int*)(creatureaddr+0x194 - 0x10);
    unsigned int* petItemXP = (unsigned int*)(entityaddr+0x1024);
    unsigned int* petItemLevel = (unsigned int*)(entityaddr+0x1030);

    if (*playerGUID == *ownerGUID){
        //if the entity belongs to the local player
        if (updateDue == 0){
            *petItemXP = *petXP;
            *petItemLevel = *petLevel;
        }

        if (updateDue >= 0){
            updateDue -= 1;
        }
        else if (*petItemXP != *petXP || *petLevel != *petItemLevel){
            updateDue = 3; //This is to keep it from updating too often
        }
    }

}
DWORD UpdatePetPtr = (DWORD)&UpdatePet;

void WriteJMP(BYTE* location, BYTE* newFunction){
	DWORD dwOldProtection;
	VirtualProtect(location, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection);
    location[0] = 0xE9; //jmp
    *((DWORD*)(location + 1)) = (DWORD)(( (unsigned INT32)newFunction - (unsigned INT32)location ) - 5);
	VirtualProtect(location, 5, dwOldProtection, &dwOldProtection);
}


extern "C" DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    base = (UINT_PTR)GetModuleHandle(NULL);
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            WriteJMP((BYTE*)(base + 0x66057), (BYTE*)&ASMCreatureUpdateInjection); //This is some function that runs on entities a lot, so I'm leveraging it.
            break;

    }
    return TRUE;
}
