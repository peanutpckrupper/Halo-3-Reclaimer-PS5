# Halo-3-Reclaimer-PS5
HALO 3 THROUGH PROJECT RECLAIMER ON PS5 (prospero-win)

If you just want to play Halo 3 (or the other MCC campaigns) on PS5,
Installation 08 is much simpler:
https://github.com/peanutpckrupper/installation08/releases

This way takes building a patched Wine on a PC.


IN THIS FOLDER
  halo3-reclaimer.profile   the prospero-win profile that runs it
  halo3-reclaimer.yml       prospero-win recipe that builds the Wine prefix
  0603-...patch             Wine patch: lets Reclaimer hand Halo 3 its settings
                            (WriteProcessMemory to itself)
  0170-...patch             Wine patch: lets Reclaimer's local network binds
                            (127.0.x.x addresses) work on the PS5
  dwmapi-cleaner/           a small dwmapi.dll (with its source) that clears the
                            worker's leftover native-data folder at each start
Both patches are needed. Stock prospero-win doesn't include them.


YOU NEED
  - A jailbroken PS5 with prospero-win installed and working.
  - A PC with WSL (Ubuntu 24.04), prospero-win's source and its build setup
    (docs/DEVELOPMENT.md).
  - Halo: The Master Chief Collection on Steam with Halo 3 Campaign and Halo 3
    Multiplayer installed. Uninstall the other MCC games first to keep the copy
    small.
  - Project Reclaimer v0.9.13 (project-reclaimer-v0.9.13.exe) in your MCC
    folder. Run it once on the PC first so it's set up there.


1. PATCH AND BUILD THE PS5 WINE
   Copy both .patch files into prospero-win's wine/patches/ folder and build
   the PS5 Wine (tools/build_wine_ps5.sh, see docs/WINE_PS5_BUILD.md).
   On the PS5, back up and replace:
     /data/homebrew/PPSA99995/win/wine/lib/wine/x86_64-unix/ntdll.prx
   with the one you built, and set its permission to 755 in your FTP client.

2. BUILD THE PC WINE (once)
     tools/build_host_wine.sh --jobs $(nproc)
   It prints the path of its wine when done.

3. MAKE THE PREFIX
   From your prospero-win folder (adjust the paths if your Steam library is
   somewhere else):
     python3 tools/pw_install.py /path/to/halo3-reclaimer.yml \
         --library ~/prospero-library \
         --wine <PC wine from step 2> \
         --file game="/mnt/c/Program Files (x86)/Steam/steamapps/common/Halo The Master Chief Collection" \
         --file libraryfolders="/mnt/c/Program Files (x86)/Steam/steamapps/libraryfolders.vdf" \
         --file appmanifest="/mnt/c/Program Files (x86)/Steam/steamapps/appmanifest_976730.acf"

4. FINISH THE PREFIX
     P=~/prospero-library/prefixes/halo3-reclaimer/drive_c
     MCC="$P/Program Files (x86)/Steam/steamapps/common/Halo The Master Chief Collection"
     # MCC's own XAudio (Wine's doesn't work with the game)
     cp "$MCC/mcc/binaries/win64/XAudio2_9Redist.dll" "$P/windows/system32/xaudio2_9.dll"
     # the native-data cleaner
     cp dwmapi-cleaner/dwmapi.dll "$P/windows/system32/dwmapi.dll"
     # the worker's working folder, with Halo 3's map list in it: the worker runs
     # from this folder, and Halo 3 looks for halo3\maps\info here
     W="$P/users/prospero/AppData/Local/Temp/mcc-startup-6e1b2c8d-1b2f-4ad1-9243-a1b2c3d4e5f6"
     mkdir -p "$W/halo3/maps"
     cp -r "$MCC/halo3/maps/info" "$W/halo3/maps/"
   Then replace ~/prospero-library/profiles/halo3-reclaimer.profile with the
   halo3-reclaimer.profile from this folder.

5. SEND IT TO THE PS5
     python3 tools/pw_prefix.py push halo3-reclaimer --library ~/prospero-library \
         --host <PS5 IP> \
         --cpu-dll <prospero-win build>/dlls/wowprospero/x86_64-windows/wowprospero.dll
   The first push takes a while (tens of GB). If it stops, run the same command
   again and it carries on. Check that the profile ended up in
   /data/prospero-win/profiles/ on the PS5 (FTP it there if not).

6. PLAY
   Start prospero-win and choose "Halo 3 (Reclaimer)".
   Don't use Reclaimer's "Update & restart" on the console. Update on the PC
   and push again; if the new version has a different file name, change
   "executable" in the profile to match.

If it fails, the logs are in /data/prospero-win/logs/ on the PS5:
  - "File already exists (os error 183)": dwmapi.dll isn't in system32, or
    dwmapi=n is missing from the profile.
  - socket error 10049 / "Cannot assign requested address": ntdll.prx was
    built without patch 0170.
  - Halo 3 starts but ignores settings and keeps retrying: ntdll.prx was built
    without patch 0603.
  - "Guardian configuration refused: Native map catalog lookup did not resolve
    the selected map" and it goes back to prospero-win's list: the working folder
    has no halo3\maps\info (step 4).
