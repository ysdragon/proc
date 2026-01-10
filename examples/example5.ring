/*
	Example 5: Real-World Use Cases
	-------------------------------
	Practical examples of subprocess usage.
	Cross-platform compatible.
*/

load "proc.ring"

func main
	? "=== Ring Proc - Real-World Examples ==="
	? ""

	? "1. Git operations:"
	result = proc_run(["git", "--version"], PROC_SEARCH_USER_PATH)
	if result[1] = 0
		? "   Git version: " + trim(result[2])

		result = proc_run(["git", "status", "--porcelain"], PROC_SEARCH_USER_PATH)
		if result[1] = 0
			if len(trim(result[2])) = 0
				? "   Working directory is clean"
			else
				? "   Modified files: " + len(str2list(result[2]))
			ok
		ok
	else
		? "   Git not available"
	ok
	? ""

	? "2. Running Python code:"
	result = proc_run(["python3", "--version"], PROC_SEARCH_USER_PATH)
	if result[1] != 0
		result = proc_run(["python", "--version"], PROC_SEARCH_USER_PATH)
	ok

	if result[1] = 0
		? "   " + trim(result[2])

		cPythonCode = "print(sum(range(1, 101)))"
		if isWindows()
			result = proc_run(["python", "-c", cPythonCode], PROC_SEARCH_USER_PATH)
		else
			result = proc_run(["python3", "-c", cPythonCode], PROC_SEARCH_USER_PATH)
		ok
		? "   Sum 1-100 = " + trim(result[2])
	else
		? "   Python not available"
	ok
	? ""

	? "3. System information:"
	if isWindows()
		result = proc_shell("hostname")
	else
		result = proc_shell("hostname")
	ok
	? "   Hostname: " + trim(result[2])

	if isWindows()
		result = proc_shell("echo %USERNAME%")
	else
		result = proc_shell("whoami")
	ok
	? "   User: " + trim(result[2])
	? ""

	? "4. Parallel process execution:"
	? "   Starting 3 processes simultaneously..."

	aProcs = []
	for i = 1 to 3
		if isWindows()
			proc = proc_create(["cmd", "/c", "ping -n 1 127.0.0.1 >nul & echo Process " + i + " done"], PROC_SEARCH_USER_PATH)
		else
			proc = proc_create(["sh", "-c", "sleep 0.3 && echo Process " + i + " done"], PROC_SEARCH_USER_PATH)
		ok
		aProcs + proc
	next

	nStartTime = clock()
	for proc in aProcs
		proc_join(proc)
		? "      " + trim(proc_stdout(proc))
		proc_destroy(proc)
	next
	nElapsed = (clock() - nStartTime) / clockspersecond()

	? "   Total time: " + nElapsed + "s (parallel, not 0.9s sequential)"
	? ""

	? "5. Command pipeline simulation:"
	? "   Simulating: generate numbers | filter even | square"

	// Step 1: Generate numbers 1-10
	if isWindows()
		proc1 = proc_create(["cmd", "/c", "for /L %i in (1,1,10) do @echo %i"], PROC_SEARCH_USER_PATH)
	else
		proc1 = proc_create(["sh", "-c", "seq 1 10"], PROC_SEARCH_USER_PATH)
	ok
	proc_join(proc1)
	cData = proc_stdout(proc1)
	proc_destroy(proc1)

	// Step 2: Filter even numbers (using grep/findstr)
	if isWindows()
		proc2 = proc_create(["findstr", "[02468]$"], PROC_SEARCH_USER_PATH)
	else
		proc2 = proc_create(["grep", "-E", "[02468]$"], PROC_SEARCH_USER_PATH)
	ok
	proc_write(proc2, cData)
	proc_close_stdin(proc2)
	proc_join(proc2)
	cFiltered = proc_stdout(proc2)
	proc_destroy(proc2)

	// Step 3: Square each number using awk/powershell
	if isWindows()
		proc3 = proc_create(["powershell", "-Command", "$input | ForEach-Object { $n = [int]$_; $n * $n }"], PROC_SEARCH_USER_PATH)
	else
		proc3 = proc_create(["awk", "{print $1 * $1}"], PROC_SEARCH_USER_PATH)
	ok
	proc_write(proc3, cFiltered)
	proc_close_stdin(proc3)
	proc_join(proc3)

	aResults = str2list(proc_stdout(proc3))
	cResults = ""
	for line in aResults
		if len(trim(line)) > 0
			if len(cResults) > 0
				cResults += ", "
			ok
			cResults += trim(line)
		ok
	next
	proc_destroy(proc3)

	? "   Input: 1-10"
	? "   Filter: even numbers (2,4,6,8,10)"
	? "   Transform: square each"
	? "   Result: " + cResults
	? ""

	? "Done!"
