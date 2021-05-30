# Captain

![claw.jpg](https://github.com/0x00Jeff/Captain/blob/main/Assets/claw.jpg)

Captain is cheat for the the game known as Captain claw, it has 2 cheating modes and each has its features : 

### god mod
- the player doesn't get any damage when hit
- the player has infinite ammo of the 3 weapons in the game (pistol, magic claws and dynamites)
- once you pickup a jumper, you don't loose it till the end of the current level unless you die 

### playable mode
- the game starts with all weapons having 30 ammo
- there is no limit to any of the health, postol ammo and the magic claws. so the player can pick up health/ammo bottles beyond what they can normally carry

# how to build
this code was written and built under GNU/Linux, if you're are using it as well, you should install `mingw-w64` package using one of the following commands depending on your distro, if you're building on it windows, figure it out.
#### ubuntu
        -sudo apt update && sudo apt install -y gcc-mingw-w64
#### Arch linux
        -sudo pacman -Sy mingw-64
        
then run 
```bash
git clone https://github.com/0x00Jeff/Captain.git
cd Captain
make
```
and you're good to go

## thanks for
@yezz123 for helping me find the right package name for ubuntu
