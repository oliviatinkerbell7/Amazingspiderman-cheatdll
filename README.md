AmazingSpiderman-cheatdll
------------------------
Template source for a Windows dinput8-style DLL that writes cheats
(Health / XP / Tech) into GameLogic.dll addresses.

* Do not try to compile on Termux. Use GitHub Actions (Windows) or a Windows PC.
* Once compiled, drop the resulting dinput8.dll into the game folder.

Files:
- src/main.cpp   : DLL source
- include/cheats.h : offsets
- .github/workflows/build-windows.yml : CI to compile on GitHub Actions
