#include<stdio.h>
#include<stdlib.h>
#include<string.h>		// starchr
#include<windows.h>
#include<tlhelp32.h>		// CreateToolhelp32Snapshot
#include<memoryapi.h>		// ReadProcessMemorry
#include"test.h"


int main(){
	// check the PROCESS_ALL_ACCESS problem @ https://docs.microsoft.com/en-us/windows/win32/procthread/process-security-and-access-rights, this is only a problem if claw runs on windowsX where X >= 8, look up if that's true, if it is, use the solution mentioned in the said link
	DWORD pid = get_proc_id(TARGET);
	if(!pid){
		fprintf(stderr, "couldn't find %s process id\n", TARGET);
	}

	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if(!h){
		fprintf(stderr, "OpenProcess failed with error %lld\n", GetLastError());
		return -1;
	}

	uintptr_t base = GetModuleBaseAddress(pid, TARGET);
	if(!base){
		fprintf(stderr, "coudln't find %s module base :(", TARGET);
		return -1;
	}

	printf("CLAW.EXE module found @ %p\n", base);

	// changing the health
	DWORD new_health = 120;
	ReadWriteMemory(h, base + health_start , health_offsets, health_size, (LPCVOID)&new_health, sizeof(new_health));// TODO : might define the 420 to NEW_HEALTH as well


	// chaning the ammo
	struct weapons w;
	w.pistol = 15;
	w.magic  = 25;
	w.bombs  = 35;

	ReadWriteMemory(h, base + ammo_start , ammo_offsets, ammo_size, (LPCVOID)&w, sizeof(w));

	// PTR : TODO find 7n9aza offsets

	// changing the code responsible for editting the health
	
	//// health
	WriteCode(h, (void *)(base + health_code_start), health_opcodes, health_opcodes_size);



	//// pistol ammo
	// standing up
	WriteCode(h, (void *)(base + standing_ammo_code_start), ammo_opcodes, ammo_opcodes_size);

	// croushing
	WriteCode(h, (void *)(base + croushing_ammo_code_start), ammo_opcodes, ammo_opcodes_size);


	//// magic claw
	// standing up
	WriteCode(h, (void *)(base + standing_magic_claw_start), ammo_opcodes, ammo_opcodes_size);
	// croushing
	WriteCode(h, (void *)(base + croushing_magic_claw_start), ammo_opcodes, ammo_opcodes_size);



	//// dynamite ammo
	// standing
	WriteCode(h, (void *)(base + standing_dynamite_code_start), dec_to_inc, dec_to_inc_size);

	// croushing
	WriteCode(h, (void *)(base + croushing_dynamite_code_start), dec_to_inc, dec_to_inc_size);

	//// extending the limites
	// health part1
	WriteCode(h, (void *)(base + health_limit_start1), health_limit_opcodes1, health_limit_opcodes_size1);

	// health part2
	WriteCode(h, (void *)(base + health_limit_start2), health_limit_opcodes2, health_limit_opcodes_size2);


	puts("check now!");
}

void WriteCode(HANDLE h, void *ptr, char *opcodes, size_t size){

	DWORD new_perms = PAGE_READWRITE;
	DWORD old_perms;

/*/	puts("calling the first VirtualProtectEx");
	if(!VirtualProtectEx(h, ptr, size, new_perms, &old_perms)){
		fprintf(stderr, "VirtualProtectEx failed with %ld\n", GetLastError());
		perrno("VirtualProtectEx");
		CloseHandle(h);
		//return -1; // TODO : use this for error check later!
		return;
	}
*/

//	printf("writing to address %p\n", ptr);

	if(!WriteProcessMemory(h, ptr, (void *)opcodes, size, NULL)){
		fprintf(stderr, "WriteProcessMemory failed with %ld\n", GetLastError());
		perrno("WriteProcessMemory");
		return;

	}

/*
	if(!VirtualProtectEx(h, ptr, size, old_perms, NULL)){
		fprintf(stderr, "VirtualProtectEx failed with %ld\n", GetLastError());
		perrno("VirtualProtectEx");
		CloseHandle(h);
		//return -1; // TODO : use this for error check later!
		return;
	}
*/
}

void ReadWriteMemory(HANDLE h, uintptr_t base, unsigned int *offsets, size_t size, LPCVOID new_value, size_t value_size){

	uintptr_t target_addr = resolve_dynamic_address(h, base, offsets, size);

	DWORD value;

//	ReadProcessMemory(h, (void *)target_addr, &value, sizeof(value), NULL); // handle errors!

	WriteProcessMemory(h, (void *)target_addr, new_value, value_size, NULL); // handle errors!
}

uintptr_t resolve_dynamic_address(HANDLE h, uintptr_t base, unsigned int *offsets, size_t size){
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
		fprintf(stderr, "CreateToolhelp32Snapshot failed with %lld\n", GetLastError());
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
		perrno("Module32Next");

	CloseHandle(snapshot);
	return module_base;
}

void perrno(char *func){ // TODO : prolly gonna remove this function
	TCHAR err_msg[256] = {0};
	DWORD errn;

	FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
         NULL, errn,
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
         err_msg, 256, NULL );

	 printf("\n WARNING: %s failed with error %d (%s)\n", func, errn, err_msg );
}

DWORD get_proc_id(char *name){
	DWORD pid = 0;
	PROCESSENTRY32 pe = {0};

	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(h == INVALID_HANDLE_VALUE){
		fprintf(stderr, "CreateToolhelp32Snapshot failed with %lld\n", GetLastError());
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

	CloseHandle(h);

	return pid;
}
