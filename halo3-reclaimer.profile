; Halo 3 through Project Reclaimer 0.9.13 on prospero-win (PS5).
;
; Reclaimer's normal launcher starts Halo 3 in a second process, which Wine on
; the console can't do. So this profile starts Reclaimer's Halo 3 engine worker
; directly, with the arguments its launcher would give it.
;
; Needs:
; - a PS5 Wine ntdll.prx built with patches 0603 and 0170 (see README.txt);
; - dwmapi.dll from dwmapi-cleaner in the prefix's C:\windows\system32. It
;   clears the worker's leftover "native-data" folder at each start; without it
;   the second launch fails with "File already exists (os error 183)";
; - the working folder below created in the prefix, with a copy of MCC's
;   halo3\maps\info in it (Halo 3 reads its map list relative to it);
; - MCC's XAudio2_9Redist.dll copied to C:\windows\system32\xaudio2_9.dll.
;
; Goes in /data/prospero-win/profiles/ on the PS5.
[application]
id = halo3-reclaimer
name = Halo 3 (Reclaimer)
executable = C:\Program Files (x86)\Steam\steamapps\common\Halo The Master Chief Collection\project-reclaimer-v0.9.13.exe
working_directory = C:\users\prospero\AppData\Local\Temp\mcc-startup-6e1b2c8d-1b2f-4ad1-9243-a1b2c3d4e5f6
arguments = probe-engine-worker "\\?\C:\Program Files (x86)\Steam\steamapps\common\Halo The Master Chief Collection\halo3\halo3.dll" --guardian-slayer --client-window --startup --skip-render-warmup --shell-callbacks --native-udp
dll_overrides = d3d8,d3d9,d3d10core,d3d11,dxgi=n;xaudio2_9=n;dwmapi=n
prefix = halo3-reclaimer
runtime = wine-wow64
architecture = pe64
graphics = opengl

[display]
desktop = 1920x1080

[input]
mode = xinput
