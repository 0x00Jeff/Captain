#ifndef _CHEAT_H
	#define _CHEAT_H
	#define TARGET "CLAW.EXE"

	// modes
	// this can start from any value other than 0, cause that's what scanf fill  
	// the variable with when usign %d and there is no approriate input
	enum cheating_modes {GOD_MODE = 1, PLAYABLE};

	// array size
	#define SIZE(x)	(sizeof(x) / sizeof(x[0]))

	// functions
	DWORD get_proc_id(char *name);
	BOOL IsLevelStarted(HANDLE h, uintptr_t base, const unsigned int *offsets,size_t size);
	uintptr_t GetModuleBaseAddress(DWORD pid, char *module_name);
	uintptr_t resolve_dynamic_address(HANDLE h, const uintptr_t base, const unsigned int *offsets, size_t size);
	void ReadWriteMemory(HANDLE h, uintptr_t base, const unsigned int *offsets, size_t size, LPCVOID new_value, size_t value_size);
	void WriteCode(HANDLE h, void *ptr, const unsigned char *opcodes, size_t size);
	void perrno(char *msg);

	///////////////////////////////// memory editting stuff ///////////////////////////
	
	// health stuff
	const unsigned int health_start = 0x1365d4;
	const unsigned int health_offsets[] = {0x128};

	// ammo stuff
	const unsigned int ammo_start = 0x1365d4;
	const unsigned int ammo_offsets[] = {0x7c, 0x14, 0x70};

	// weapons
	struct __attribute__((packed)) weapons{
		DWORD pistol;
		DWORD magic;
		DWORD bombs;
	};
	
	///////////////////////////////// code editting stuff /////////////////////////////
	
	// health
	const unsigned int health_offset = 0x1E849;
	const  unsigned char health_opcodes[] = {0x90, 0x90}; 
	
	// pistol ammo
	const unsigned int standing_ammo_offset = 0x1DAF6 + 4;
	const unsigned int croushing_ammo_offset = 0x1DE38 + 4;
	const unsigned char ammo_opcodes[] = {0x90};
	
	// magic claw
	const unsigned int standing_magic_offset = 0x1DB6F + 4;
	const unsigned int croushing_magic_offset = 0x1DEC5 + 4;
	const unsigned char dec_to_nop[] = {0x90};
	
	// dynamites
	const unsigned int standing_dynamite_offset = 0x1DC86;
	const unsigned int croushing_dynamite_offset = 0x1DFEC;
	
	//// extending the limits
	// health
	//
	// the following 2 addresses does 2 things, first it doesn't let the player to pickup a bottle of water of the health was full, the second is resetting the health to 100 if it exceeds that value e.i if the health was 95 and the player pick up a bottle which gives more than 5 hp
	// of course, we need to get rid of both those functionalitles
	const unsigned int health_limit_start1 = 0x1E454;
	const unsigned char health_limit_opcodes1[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
	
	const unsigned int health_limit_start2 = 0x1E511;
	const unsigned char health_limit_opcodes2[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
	
	// ammo
	const unsigned int ammo_limit_offset = 0x1E58E; // resets the ammo to 99 if it exceeds it
	const unsigned char ammo_limit_opcodes[] = {0x90, 0x90,0x90, 0x90, 0x90, 0x90, 0x90};
	
	// magic claw
	const unsigned int magic_limit_offset = 0x1E551; // resets the magic to 99 if it exceeds it
	const unsigned char magic_limit_opcodes[] = {0x90, 0x90,0x90, 0x90, 0x90, 0x90, 0x90};
	
	// jumper
	const unsigned int jumper_limit_offset = 0x20F34; // decreases the jumper time
	const unsigned char jumper_limit_opcodes[] = {0x90, 0x90};
	
#endif