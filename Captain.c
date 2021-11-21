#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include<stdio.h>
#include<windows.h>
#include<tlhelp32.h>		// CreateToolhelp32Snapshot
#include<memoryapi.h>		// ReadProcessMemorry
#include"Captain.h"


int main(){
	int mode;

	DWORD pid = get_proc_id(TARGET);
	if(!pid){
		fprintf(stderr, "couldn't find %s process id\n", TARGET);
		return -1;
	}

	do{ 
		puts("choose the cheating mode :");
		printf("%d - god mode\n", GOD_MODE);
		printf("%d - playable mode\n", PLAYABLE);
		printf(">>> ");
		fflush(stdout);

		scanf("%d", &mode);
		getc(stdin);
	}while(mode != GOD_MODE && mode != PLAYABLE);

	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if(!h){
		perror("OpenProcess");
		return -1;
	}

	uintptr_t base = GetModuleBaseAddress(pid, TARGET);
	if(!base){
		fprintf(stderr, "coudln't find %s module base :(", TARGET);
		CloseHandle(h);
		return -1;
	}

	do{
		Sleep(1500);
	}while(IsLevelStarted(h, base + health_start, health_offsets, SIZE(health_offsets)));

	if(mode == GOD_MODE){
		// changing the code responsible for editting the :
	
		//// health
		WriteCode(h, (void *)(base + health_offset), health_opcodes, SIZE(health_opcodes));

		//// pistol ammo
		// standing up
		WriteCode(h, (void *)(base + standing_ammo_offset), dec_to_nop, SIZE(dec_to_nop));

		// croushing
		WriteCode(h, (void *)(base + croushing_ammo_offset), dec_to_nop, SIZE(dec_to_nop));


		//// magic claw
		// standing up
		WriteCode(h, (void *)(base + standing_magic_offset), dec_to_nop, SIZE(dec_to_nop));

		// croushing
		WriteCode(h, (void *)(base + croushing_magic_offset), dec_to_nop, SIZE(dec_to_nop));


		//// dynamite ammo
		// standing
		WriteCode(h, (void *)(base + standing_dynamite_offset), dec_to_nop, SIZE(dec_to_nop));

		// croushing
		WriteCode(h, (void *)(base + croushing_dynamite_offset), dec_to_nop, SIZE(dec_to_nop));

		// jumper
		WriteCode(h, (void *)(base + jumper_limit_offset), jumper_limit_opcodes, SIZE(jumper_limit_opcodes));

	}
	else{

		// changing the health
		DWORD new_health = 120;
		ReadWriteMemory(h, base + health_start, health_offsets, SIZE(health_offsets), (LPCVOID)&new_health, sizeof(new_health));

		// changing the ammo
		struct weapons w = (struct weapons){30, 30, 30};
		ReadWriteMemory(h, base + ammo_start, ammo_offsets, SIZE(ammo_offsets), (LPCVOID)&w, sizeof(w));

		//// extending the limits                                                                            	
		// health part1
		WriteCode(h, (void *)(base + health_limit_start1), health_limit_opcodes1, SIZE(health_limit_opcodes1));

        // health part2
        WriteCode(h, (void *)(base + health_limit_start2), health_limit_opcodes2, SIZE(health_limit_opcodes2));
        	
        // pistol ammo
        WriteCode(h, (void *)(base + ammo_limit_offset), ammo_limit_opcodes, SIZE(ammo_limit_opcodes));
        	
        // magic claw
        WriteCode(h, (void *)(base + magic_limit_offset), magic_limit_opcodes, SIZE(magic_limit_opcodes));
	}

	CloseHandle(h);
	puts("done");

	return 0;
}

inline void WriteCode(HANDLE h, void *ptr, const unsigned char *opcodes, size_t size){
	if(!WriteProcessMemory(h, ptr, (void *)opcodes, size, NULL))
		perrno("WriteProcessMemory");
}

void ReadWriteMemory(HANDLE h, uintptr_t base, const unsigned int *offsets, size_t size, LPCVOID new_value, size_t value_size){
	uintptr_t target_addr = resolve_dynamic_address(h, base, offsets, size);
	if(!WriteProcessMemory(h, (void *)target_addr, new_value, value_size, NULL))
		perrno("WriteProcessMemory");
}

uintptr_t resolve_dynamic_address(HANDLE h, uintptr_t base, const unsigned int *offsets, size_t size){
	unsigned int i = 0;
	intptr_t ptr = base;
	for(i = 0; i < size; i++){
		ReadProcessMemory(h, (void *)ptr, &ptr, sizeof(DWORD), NULL);
		ptr += offsets[i];
	}
	return ptr;
}

uintptr_t GetModuleBaseAddress(DWORD pid, char *module_name){
	uintptr_t module_base = 0;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE32 | TH32CS_SNAPMODULE, pid);
	if(snapshot == INVALID_HANDLE_VALUE){
		perrno("CreatetoolHelp32Snapshot");
		return module_base;
	}

	MODULEENTRY32 module = {0};
	module.dwSize = sizeof(MODULEENTRY32);

	if(Module32First(snapshot, &module) == TRUE){
		do{
			if(!strcmp(module.szModule, module_name)){
				module_base = (uintptr_t)module.modBaseAddr;
				break;
			}
		}while(Module32Next(snapshot, &module) || GetLastError() == ERROR_BAD_LENGTH);
	}
	else
		perrno("Module32First");

	CloseHandle(snapshot);
	return module_base;
}

/* 
 * the weird equivalent of linux's perror()
*/

void perrno(char *func){
	TCHAR err_msg[256] = {0};
	DWORD err = GetLastError();

	FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
         NULL, err,
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
         err_msg, 256, NULL );

	 fprintf(stderr, "%s failed with erro %ld : %s", func, err, err_msg);
}

DWORD get_proc_id(char *name){
	DWORD pid = 0;
	PROCESSENTRY32 pe = {0};

	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(h == INVALID_HANDLE_VALUE){
		perrno("CreateToolhelp32Snapshot");
		return pid;
	}

	pe.dwSize = sizeof(PROCESSENTRY32);
	if(Process32First(h, &pe)){
		do{
			if(!strcmp(pe.szExeFile, name)){
				pid = pe.th32ProcessID;
				break;
			}
		}while(Process32Next(h, &pe));
	}
	else
		perrno("Process32First");

	CloseHandle(h);

	return pid;
}

/* 
 * the health pointer is accessible from the start of the game but when used with ReadProcessMemory
 * in resolve_dynamic_address function it returns and error with an errno value of 299 (partial read)
 * the "value" variable gets filled with a 0
 * we can use this to detect if a level has been started yet or not
*/

BOOL IsLevelStarted(HANDLE h, uintptr_t base, const unsigned int *offsets, size_t size){
	// this variable can't be made static since resolve_dynamic_address returns diff values
	// after and before loading a level, and the former is not what we wan't
	uintptr_t target_addr = resolve_dynamic_address(h, base, offsets, size);

	DWORD health = 0;

	if(!ReadProcessMemory(h, (void *)target_addr, &health, sizeof(health), NULL))
		if(GetLastError() != 299) // partial read as if a level hasn't started yet
			perrno("ReadProcessMemory");

	if(0 < health && health <= 1000){	
					// a level has started
					// normally this vale should be == 100, but that
					// would cause the cheat not to work, if it was
					// used more than once i.e it was started again for
					// trying a diff mode
		return 0;
	}else
		puts("please start one of levels");
		
	return 1;
}
