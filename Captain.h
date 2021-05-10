#ifndef _CHEAT_H
	#define _CHEAT_H
	#define TARGET "CLAW.EXE"

	// modes
	#define GOD_MODE 1
	#define PLAYABLE 2

	// functions
	DWORD get_proc_id(char *name);
	BOOL IsLevelStarted(HANDLE h, uintptr_t base, unsigned int *offsets,size_t size);
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
	unsigned int health_offset = 0x1E849;
	unsigned char health_opcodes[] = {0x90, 0x90}; 
	size_t health_opcodes_size = sizeof(health_opcodes) / sizeof(health_opcodes[0]);

	// pistol ammo
	unsigned int standing_ammo_offset = 0x1DAF6 + 4;
	unsigned int croushing_ammo_offset = 0x1DE38 + 4;
	unsigned char ammo_opcodes[] = {0x90};
	size_t ammo_opcodes_size = sizeof(ammo_opcodes) / sizeof(ammo_opcodes[0]);

	// magic claw
	unsigned int standing_magic_offset = 0x1DB6F + 4;
	unsigned int croushing_magic_offset = 0x1DEC5 + 4;
	unsigned char dec_to_nop[] = {0x90};
	size_t dec_to_nop_size = 1;
	
	// dynamites
	unsigned int standing_dynamite_offset = 0x1DC86;
	unsigned int croushing_dynamite_offset = 0x1DFEC;
	// uses dec_to_nop
	
	//// extending the limits
	// health
	//
	// the following 2 addresses does 2 things, first it doesn't let the player to pickup a bottle of water of the health was full, the second is resetting the health to 100 if it exceeds that value e.i if the health was 95 and the player pick up a bottle which gives more than 5 hp
	// of course, we need to get rid of both those functionalitles
	unsigned int health_limit_start1 = 0x1E454;
	unsigned char health_limit_opcodes1[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
	size_t health_limit_opcodes_size1 = sizeof(health_limit_opcodes1) / sizeof(health_limit_opcodes1[0]);

	unsigned int health_limit_start2 = 0x1E511;
	unsigned char health_limit_opcodes2[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
	size_t health_limit_opcodes_size2 = sizeof(health_limit_opcodes2) / sizeof(health_limit_opcodes2[0]);

	// ammo
	unsigned int ammo_limit_start = 0x1E58E; // resets the ammo to 99 if it exceeds it
	unsigned char ammo_limit_opcodes[] = {0x90, 0x90,0x90, 0x90, 0x90, 0x90, 0x90};
	size_t ammo_limit_size = sizeof(ammo_limit_opcodes) / sizeof(ammo_limit_opcodes[0]);

	// magic claw
	unsigned int magic_limit_start = 0x1E551; // resets the magic to 99 if it exceeds it
	unsigned char magic_limit_opcodes[] = {0x90, 0x90,0x90, 0x90, 0x90, 0x90, 0x90};
	size_t magic_limit_size = sizeof(magic_limit_opcodes) / sizeof(magic_limit_opcodes[0]);

	// jumper
	unsigned int jumper_limit_offset = 0x20F34; // decreases the jumper time
	unsigned char jumper_limit_opcodes[] = {0x90, 0x90};
	size_t jumper_limit_size = sizeof(jumper_limit_opcodes) / sizeof(jumper_limit_opcodes[0]);

#endif
