if isWindows()
	loadlib("ring_proc.dll")
but isLinux() or isFreeBSD()
	loadlib("libring_proc.so")
but isMacOSX()
	loadlib("libring_proc.dylib")
else
	raise("Unsupported OS! You need to build the library for your OS.")
ok

load "src/proc.ring"
