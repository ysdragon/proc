aPackageInfo = [
	:name = "Proc",
	:description = "Subprocess management library for Ring programming language.",
	:folder = "proc",
	:developer = "ysdragon",
	:email = "youssefelkholey@gmail.com",
	:license = "MIT License",
	:version = "1.0.1",
	:ringversion = "1.27",
	:versions = 	[
		[
			:version = "1.0.1",
			:branch = "master"
		]
	],
	:libs = 	[
		[
			:name = "",
			:version = "",
			:providerusername = ""
		]
	],
	:files = 	[
		"lib.ring",
		"main.ring",
		".clang-format",
		"CMakeLists.txt",
		"examples/example1.ring",
		"examples/example2.ring",
		"examples/example3.ring",
		"examples/example4.ring",
		"examples/example5.ring",
		"examples/example6.ring",
		"LICENSE",
		"README.md",
		"src/proc.ring",
		"src/proc.rh",
		"src/c_src/ring_proc.c",
		"src/utils/color.ring",
		"src/utils/install.ring",
		"src/utils/uninstall.ring"
	],
	:ringfolderfiles = 	[

	],
	:windowsfiles = 	[
		"lib/windows/i386/ring_proc.dll",
		"lib/windows/amd64/ring_proc.dll",
		"lib/windows/arm64/ring_proc.dll"
	],
	:linuxfiles = 	[
		"lib/linux/amd64/libring_proc.so",
		"lib/linux/arm64/libring_proc.so",
		"lib/linux/musl/amd64/libring_proc.so",
		"lib/linux/musl/arm64/libring_proc.so"
	],
	:ubuntufiles = 	[

	],
	:fedorafiles = 	[

	],
	:macosfiles = 	[
		"lib/macos/amd64/libring_proc.dylib",
		"lib/macos/arm64/libring_proc.dylib"
	],
	:freebsdfiles = 	[
		"lib/freebsd/amd64/libring_proc.so"
	],
	:windowsringfolderfiles = 	[

	],
	:linuxringfolderfiles = 	[

	],
	:ubunturingfolderfiles = 	[

	],
	:fedoraringfolderfiles = 	[

	],
	:freebsdringfolderfiles = 	[

	],
	:macosringfolderfiles = 	[

	],
	:run = "ring main.ring",
	:windowsrun = "",
	:linuxrun = "",
	:macosrun = "",
	:ubunturun = "",
	:fedorarun = "",
	:setup = "ring src/utils/install.ring",
	:windowssetup = "",
	:linuxsetup = "",
	:macossetup = "",
	:ubuntusetup = "",
	:fedorasetup = "",
	:remove = "ring src/utils/uninstall.ring",
	:windowsremove = "",
	:linuxremove = "",
	:macosremove = "",
	:ubunturemove = "",
	:fedoraremove = "",
    :remotefolder = "proc",
    :branch = "master",
    :providerusername = "ysdragon",
    :providerwebsite = "github.com"
]