/*
	Example 1: Basic Shell Commands
	-------------------------------
	Demonstrates proc_shell() and proc_run() for simple command execution.
	Cross-platform compatible.
*/

load "proc.ring"

func main
	? "=== Ring Proc - Basic Shell Commands ==="
	? ""

	? "1. Running a simple echo command:"
	if isWindows()
		result = proc_shell("echo Hello from Ring Proc")
	else
		result = proc_shell("echo 'Hello from Ring Proc'")
	ok
	? "   Exit code: " + result[1]
	? "   Output: " + trim(result[2])
	? ""

	? "2. Getting current directory:"
	if isWindows()
		result = proc_shell("cd")
	else
		result = proc_shell("pwd")
	ok
	? "   Exit code: " + result[1]
	? "   Current dir: " + trim(result[2])
	? ""

	? "3. Listing directory contents:"
	if isWindows()
		result = proc_run(["cmd", "/c", "dir", "/b"], PROC_SEARCH_USER_PATH)
	else
		result = proc_run(["ls", "-1"], PROC_SEARCH_USER_PATH)
	ok
	? "   Exit code: " + result[1]
	? "   First 5 entries:"
	aLines = str2list(result[2])
	nCount = min(5, len(aLines))
	for i = 1 to nCount
		if len(trim(aLines[i])) > 0
			? "      " + aLines[i]
		ok
	next
	? ""

	? "4. Running command that fails:"
	result = proc_shell("this_command_does_not_exist_xyz 2>&1")
	? "   Exit code: " + result[1]
	if result[1] != 0
		? "   Command failed as expected (non-zero exit)"
	ok
	? ""

	? "5. Using proc_run with multiple arguments:"
	if isWindows()
		result = proc_run(["cmd", "/c", "ver"], PROC_SEARCH_USER_PATH)
	else
		result = proc_run(["uname", "-a"], PROC_SEARCH_USER_PATH)
	ok
	? "   Exit code: " + result[1]
	? "   System info: " + trim(result[2])
	? ""

	? "Done!"