/*
	Example 3: Custom Environment Variables
	---------------------------------------
	Demonstrates proc_create_ex() with custom environment.
	Cross-platform compatible.
*/

load "proc.ring"

func main
	? "=== Ring Proc - Custom Environment ==="
	? ""

	? "1. Setting custom environment variables:"
	aEnv = [
		"MY_APP_NAME=RingProc",
		"MY_APP_VERSION=1.0.0",
		"MY_DEBUG=true",
		"PATH=" + sysget("PATH")
	]

	if isWindows()
		proc = proc_create_ex(["cmd", "/c", "echo %MY_APP_NAME% %MY_APP_VERSION%"], PROC_SEARCH_USER_PATH, aEnv)
	else
		proc = proc_create_ex(["printenv", "MY_APP_NAME"], PROC_SEARCH_USER_PATH, aEnv)
	ok
	proc_join(proc)
	? "   MY_APP_NAME = " + trim(proc_stdout(proc))
	proc_destroy(proc)

	if !isWindows()
		proc = proc_create_ex(["printenv", "MY_APP_VERSION"], PROC_SEARCH_USER_PATH, aEnv)
		proc_join(proc)
		? "   MY_APP_VERSION = " + trim(proc_stdout(proc))
		proc_destroy(proc)

		proc = proc_create_ex(["printenv", "MY_DEBUG"], PROC_SEARCH_USER_PATH, aEnv)
		proc_join(proc)
		? "   MY_DEBUG = " + trim(proc_stdout(proc))
		proc_destroy(proc)
	ok
	? ""

	? "2. Isolated environment (custom only):"
	aEnv = ["ISOLATED_VAR=yes", "PATH=" + sysget("PATH")]

	if isWindows()
		proc = proc_create_ex(["cmd", "/c", "echo %ISOLATED_VAR%"], PROC_SEARCH_USER_PATH, aEnv)
	else
		proc = proc_create_ex(["printenv", "ISOLATED_VAR"], PROC_SEARCH_USER_PATH, aEnv)
	ok
	proc_join(proc)
	? "   ISOLATED_VAR = " + trim(proc_stdout(proc))
	proc_destroy(proc)

	// HOME should be empty in isolated environment
	if !isWindows()
		proc = proc_create_ex(["printenv", "HOME"], PROC_SEARCH_USER_PATH, aEnv)
		proc_join(proc)
		cHome = trim(proc_stdout(proc))
		if len(cHome) = 0
			? "   HOME = (empty - isolated environment)"
		else
			? "   HOME = " + cHome
		ok
		proc_destroy(proc)
	ok
	? ""

	? "3. Inheriting parent environment:"
	if isWindows()
		proc = proc_create_ex(["cmd", "/c", "echo %PATH%"], PROC_INHERIT_ENVIRONMENT | PROC_SEARCH_USER_PATH, NULL)
	else
		proc = proc_create_ex(["printenv", "HOME"], PROC_INHERIT_ENVIRONMENT | PROC_SEARCH_USER_PATH, NULL)
	ok
	proc_join(proc)
	cResult = trim(proc_stdout(proc))
	if len(cResult) > 50
		cResult = left(cResult, 50) + "..."
	ok
	? "   Using PROC_INHERIT_ENVIRONMENT"
	? "   Result: " + cResult
	proc_destroy(proc)
	? ""

	? "4. Passing configuration data:"
	cConfigJson = '{"host":"localhost","port":8080}'
	aEnv = [
		"APP_CONFIG=" + cConfigJson,
		"APP_MODE=production",
		"PATH=" + sysget("PATH")
	]

	if isWindows()
		proc = proc_create_ex(["cmd", "/c", "echo %APP_MODE%"], PROC_SEARCH_USER_PATH, aEnv)
	else
		proc = proc_create_ex(["sh", "-c", "echo $APP_CONFIG"], PROC_SEARCH_USER_PATH, aEnv)
	ok
	proc_join(proc)
	? "   Passed JSON config via environment"
	? "   APP_CONFIG: " + trim(proc_stdout(proc))
	proc_destroy(proc)
	? ""

	? "Done!"
