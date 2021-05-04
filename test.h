#ifndef _CHEAT_H
	#define _CHEAT_H
	#define TARGET "CLAW.EXE"

	DWORD get_proc_id(char *name);
	uintptr_t GetModuleBaseAddress(DWORD pid, char *module_name);
	uintptr_t resolve_dynamic_address(HANDLE h, uintptr_t base, unsigned int *offsets, size_t size);
	void ReadWriteMemory(HANDLE h, uintptr_t base, unsigned int *offsets, size_t size, LPCVOID new_value, size_t value_size);
	void WriteCode(HANDLE h, void *ptr, char *opcodes, size_t size);
	void perrno(char *msg);

	///////////////////////////////// memory editting stuff ///////////////////////////
	
	// health stuff
	unsigned int health_start = 0x13B15C;
	unsigned int health_offsets[] = {0x30, 0x60, 0x20, 0x260, 0x24, 0xc, 0x128};
	size_t health_size = sizeof(health_offsets) / sizeof(health_offsets[0]);

	// ammo stuff
	unsigned int ammo_start = 0x13b0e4;
	unsigned int ammo_offsets[] = {0x448, 0x1c, 0x24, 0x1b0, 0x44c, 0x2f0, 0x70};
	size_t ammo_size = sizeof(ammo_offsets) / sizeof(ammo_offsets[0]);

	struct weapons{
		DWORD pistol;
		DWORD magic;
		DWORD bombs;
	};
	
	///////////////////////////////// code editting stuff /////////////////////////////
	
	// health
	unsigned int health_code_start = 0x1E849;
	unsigned char health_opcodes[] = {0x01, 0xf9}; 
	size_t health_opcodes_size = sizeof(health_opcodes) / sizeof(health_opcodes[0]);

	// ammo
	unsigned int standing_ammo_code_start = 0x1DAF6;
	unsigned int croushing_ammo_code_start = 0x1DE38;
	unsigned char ammo_opcodes[] = {0x90, 0x90, 0x90, 0x90, 0x41};
	size_t ammo_opcodes_size = sizeof(ammo_opcodes) / sizeof(ammo_opcodes[0]);


	

#endif
