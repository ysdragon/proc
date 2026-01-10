/*
	Example 4: Process Monitoring and Control
	-----------------------------------------
	Demonstrates proc_alive(), proc_terminate(), async reads.
	Cross-platform compatible.
*/

load "proc.ring"

func main
	? "=== Ring Proc - Process Monitoring ==="
	? ""

	? "1. Monitoring process state:"
	if isWindows()
		proc = proc_create(["cmd", "/c", "ping -n 2 127.0.0.1 >nul"], PROC_SEARCH_USER_PATH)
	else
		proc = proc_create(["sleep", "1"], PROC_SEARCH_USER_PATH)
	ok

	? "   Started subprocess (runs for ~1 second)"
	nChecks = 0
	while proc_alive(proc)
		nChecks++
		? "      Check " + nChecks + ": alive"
		syssleep(300)
	end

	? "      Process finished"
	returnCode = proc_join(proc)
	? "   Exit code: " + returnCode
	? "   Total checks: " + nChecks
	proc_destroy(proc)
	? ""

	? "2. Terminating a long-running process:"
	if isWindows()
		proc = proc_create(["cmd", "/c", "ping -n 20 127.0.0.1 >nul"], PROC_SEARCH_USER_PATH)
	else
		proc = proc_create(["sleep", "10"], PROC_SEARCH_USER_PATH)
	ok

	? "   Started long subprocess (would run 10+ seconds)"
	syssleep(300)

	if proc_alive(proc)
		? "   Process is running, terminating..."
		proc_terminate(proc)
		? "   Terminated!"
	ok

	proc_join(proc)
	proc_destroy(proc)
	? ""

	? "3. Async reading while process runs:"
	if isWindows()
		proc = proc_create(["cmd", "/c", "for /L %i in (1,1,5) do @echo Line %i & ping -n 1 127.0.0.1 >nul"], PROC_ENABLE_ASYNC | PROC_SEARCH_USER_PATH)
	else
		proc = proc_create(["sh", "-c", "for i in 1 2 3 4 5; do echo Line $i; sleep 0.2; done"], PROC_ENABLE_ASYNC | PROC_SEARCH_USER_PATH)
	ok

	? "   Reading output in real-time:"
	cTotal = ""
	while proc_alive(proc)
		cChunk = proc_read_stdout(proc, 1024)
		if len(cChunk) > 0
			cTotal += cChunk
			aLines = str2list(cChunk)
			for line in aLines
				if len(trim(line)) > 0
					? "      [live] " + trim(line)
				ok
			next
		ok
		syssleep(100)
	end

	// Read any remaining output
	cChunk = proc_read_stdout(proc, 1024)
	if len(cChunk) > 0
		cTotal += cChunk
		aLines = str2list(cChunk)
		for line in aLines
			if len(trim(line)) > 0
				? "      [live] " + trim(line)
			ok
		next
	ok

	proc_join(proc)
	? "   Total bytes received: " + len(cTotal)
	proc_destroy(proc)
	? ""

	? "4. Different exit codes:"
	aExitCodes = [0, 1, 42]
	for code in aExitCodes
		if isWindows()
			proc = proc_create(["cmd", "/c", "exit " + code], PROC_SEARCH_USER_PATH)
		else
			proc = proc_create(["sh", "-c", "exit " + code], PROC_SEARCH_USER_PATH)
		ok
		exitCode = proc_join(proc)
		? "   Expected exit " + code + " -> received: " + exitCode
		proc_destroy(proc)
	next
	? ""

	? "5. Capturing stderr:"
	if isWindows()
		proc = proc_create(["cmd", "/c", "echo stdout message & echo stderr message 1>&2"], PROC_SEARCH_USER_PATH)
	else
		proc = proc_create(["sh", "-c", "echo stdout message; echo stderr message >&2"], PROC_SEARCH_USER_PATH)
	ok
	proc_join(proc)

	cOut = trim(proc_stdout(proc))
	cErr = trim(proc_stderr(proc))

	? "   stdout: " + cOut
	? "   stderr: " + cErr
	proc_destroy(proc)
	? ""

	? "Done!"
