#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include<stdio.h>
#include<windows.h>
#include<tlhelp32.h>		// CreateToolhelp32Snapshot
#include<memoryapi.h>		// ReadProcessMemorry
#include"Captain.h"


typedef struct _CLIENT_ID{
	PVOID UniqueProcess;
	PVOID UniqueThread;
}CLIENT_ID, *PCLIENT_ID;


typedef NTSTATUS(NTAPI* _NtOpenProcess)(PHANDLE, ACCESS_MASK, void *, PCLIENT_ID ci);


int main(){
	int mode;

	DWORD pid = get_proc_id(TARGET);
	if(!pid){
		fprintf(stderr, "couldn't find %s process id\n", TARGET);
		return -1;
	}

	do{ 
		puts("choose the mode :");
		puts("1 - god mod");
		puts("2 - playable mode");
		printf(">>> ");
		fflush(stdout);
		scanf("%d", &mode);

	}while(mode != GOD_MODE && mode != PLAYABLE);

	HMODULE ntdll = LoadLibrary(TEXT("ntdll"));
	if(!ntdll){
		perror("LoadLibrary");
		return -1;
	}

	CLIENT_ID ci = {(HANDLE)&pid, NULL};

	_NtOpenProcess _OpenProcess = (_NtOpenProcess) GetProcAddress(ntdll, "NtOpenProcess");
	HANDLE h;
	_OpenProcess(&h, PROCESS_ALL_ACCESS, NULL, &ci);
	if(!h){
		fprintf(stderr, "OpenProcess failed with error %lld\n", GetLastError());
		return -1;
	}

	uintptr_t base = GetModuleBaseAddress(pid, TARGET);
	if(!base){
		fprintf(stderr, "coudln't find %s module base :(", TARGET);
		CloseHandle(h);
		return -1;
	}

	do{
		Sleep(1000);

	}while(IsLevelStarted(h, base + health_start, health_offsets, health_size));

	if(mode == GOD_MODE){
		// changing the code responsible for editting the :
	
		//// health
		WriteCode(h, (void *)(base + health_offset), health_opcodes, health_opcodes_size);

		//// pistol ammo
		// standing up
		WriteCode(h, (void *)(base + standing_ammo_offset), dec_to_nop, dec_to_nop_size);

		// croushing
		WriteCode(h, (void *)(base + croushing_ammo_offset), dec_to_nop, dec_to_nop_size);


		//// magic claw
		// standing up
		WriteCode(h, (void *)(base + standing_magic_offset), dec_to_nop, dec_to_nop_size);

		// croushing
		WriteCode(h, (void *)(base + croushing_magic_offset), dec_to_nop, dec_to_nop_size);


		//// dynamite ammo
		// standing
		WriteCode(h, (void *)(base + standing_dynamite_offset), dec_to_nop, dec_to_nop_size);

		// croushing
		WriteCode(h, (void *)(base + croushing_dynamite_offset), dec_to_nop, dec_to_nop_size);

		// jumper
		WriteCode(h, (void *)(base + jumper_limit_offset), jumper_limit_opcodes, jumper_limit_size);

	}
	else{

		// changing the health
		DWORD new_health = 120;
		ReadWriteMemory(h, base + health_start , health_offsets, health_size, (LPCVOID)&new_health, sizeof(new_health));


		// chaning the ammo
		struct weapons w = (struct weapons){30, 30, 30};

		ReadWriteMemory(h, base + ammo_start , ammo_offsets, ammo_size, (LPCVOID)&w, sizeof(w));

		//// extending the limites                                                                            	
		// health part1
		WriteCode(h, (void *)(base + health_limit_start1), health_limit_opcodes1, health_limit_opcodes_size1);
        
        	// health part2
        	WriteCode(h, (void *)(base + health_limit_start2), health_limit_opcodes2, health_limit_opcodes_size2);
        
        	// pistol ammo
        	WriteCode(h, (void *)(base + ammo_limit_offset), ammo_limit_opcodes, ammo_limit_size);
        
        	// magic claw
        	WriteCode(h, (void *)(base + magic_limit_offset), magic_limit_opcodes, magic_limit_size);

	
	}

	CloseHandle(h);
	puts("done");

	return 0;
}

void WriteCode(HANDLE h, void *ptr, char *opcodes, size_t size){

	if(!WriteProcessMemory(h, ptr, (void *)opcodes, size, NULL)){
		fprintf(stderr, "WriteProcessMemory failed with %ld\n", GetLastError());
		perrno("WriteProcessMemory");
	}
}

void ReadWriteMemory(HANDLE h, uintptr_t base, unsigned int *offsets, size_t size, LPCVOID new_value, size_t value_size){

	uintptr_t target_addr = resolve_dynamic_address(h, base, offsets, size);
	if(!WriteProcessMemory(h, (void *)target_addr, new_value, value_size, NULL)){ // handle errors!
		fprintf(stderr, "WriteProcessMemory failed with %ld\n", GetLastError());
		perrno("WriteProcessMemory");
	}
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
		//fprintf(stderr, "CreateToolhelp32Snapshot failed with %lld\n", GetLastError());
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
	DWORD errn;

	FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
         NULL, errn,
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
         err_msg, 256, NULL );

	 printf("\n WARNING: %s failed with error %d (%s)\n", func, errno, err_msg );
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
	else
		perrno("Process32First");

	CloseHandle(h);

	return pid;
}

/* 
 * the health value is accessible from the start of the game, and has a very large value
 * (probably negative), during the loading of both the initial screen and every level it 
 * has the value 0, and when the level is started it gets initiliazed with the value 100 
 * (the default health) this comes handy when testing whether a level has been started
 * yet or not
*/

BOOL IsLevelStarted(HANDLE h, uintptr_t base, unsigned int *offsets,size_t size){
	static uintptr_t target_addr = 0;
	if(!target_addr)
		resolve_dynamic_address(h, base, offsets, size);

	DWORD value = 0;

	ReadProcessMemory(h, (void *)target_addr, &value, sizeof(value), NULL);
	printf("value = %lx\n", value);

	if(value == 0){ // the level is currently loading
		Sleep(500); // for good luck
		return 1;
	}else if(0 < value && value <= 500){	
					// a level has started
					// normally this vale should be used 100, but that
					// would cause the cheat not to work, if it was
					// used more than once i.e it was started again for
					// trying a diff mode
		return 0;
	}else{
		puts("please start one of levels");
		return 1; // the game started but the palyed hasn't started a level yet
	}
}
