/*
	Example 6: Async Process Reading
	------------------------------
	Demonstrates async subprocess I/O - reading output incrementally
	while the process is still running.
*/

load "proc.ring"

func main
	? "=== Ring Proc - Async Example ==="
	? ""

	? "1. Async reading from a long-running process:"
	? "   (Reading output while process runs)"
	? ""

	if isWindows()
		proc = proc_create(["cmd", "/c", "for /L %i in (1,1,5) do @(echo Line %i & ping -n 2 127.0.0.1 >nul)"], 
			PROC_ENABLE_ASYNC | PROC_SEARCH_USER_PATH)
	else
		proc = proc_create(["sh", "-c", `for i in 1 2 3 4 5; do echo "Line $i"; sleep 0.5; done`],
			PROC_ENABLE_ASYNC | PROC_SEARCH_USER_PATH)
	ok

	? "   Process started, reading incrementally..."
	cBuffer = ""
	nReadCount = 0

	while proc_alive(proc)
		cChunk = proc_read_stdout(proc, 1024)
		if len(cChunk) > 0
			cBuffer += cChunk
			nReadCount++
			? "   [Read #" + nReadCount + "] Got " + len(cChunk) + " bytes"
		ok
	end

	cChunk = proc_read_stdout(proc, 1024)
	while len(cChunk) > 0
		cBuffer += cChunk
		nReadCount++
		cChunk = proc_read_stdout(proc, 1024)
	end

	nExitCode = proc_join(proc)
	proc_destroy(proc)

	? ""
	? "   Total reads: " + nReadCount
	? "   Exit code: " + nExitCode
	? "   Full output:"
	aLines = str2list(cBuffer)
	for line in aLines
		if len(trim(line)) > 0
			? "      " + trim(line)
		ok
	next
	? ""

	? "2. Async with stderr:"
	? ""

	if isWindows()
		proc = proc_create(["cmd", "/c", "echo stdout1 & echo stderr1 >&2 & echo stdout2 & echo stderr2 >&2"],
			PROC_ENABLE_ASYNC | PROC_SEARCH_USER_PATH)
	else
		proc = proc_create(["sh", "-c", "echo stdout1; echo stderr1 >&2; echo stdout2; echo stderr2 >&2"],
			PROC_ENABLE_ASYNC | PROC_SEARCH_USER_PATH)
	ok

	cStdout = ""
	cStderr = ""

	while proc_alive(proc)
		cChunk = proc_read_stdout(proc, 1024)
		if len(cChunk) > 0
			cStdout += cChunk
		ok
		cChunk = proc_read_stderr(proc, 1024)
		if len(cChunk) > 0
			cStderr += cChunk
		ok
	end

	cChunk = proc_read_stdout(proc, 1024)
	while len(cChunk) > 0
		cStdout += cChunk
		cChunk = proc_read_stdout(proc, 1024)
	end
	cChunk = proc_read_stderr(proc, 1024)
	while len(cChunk) > 0
		cStderr += cChunk
		cChunk = proc_read_stderr(proc, 1024)
	end

	proc_join(proc)
	proc_destroy(proc)

	? "   Stdout: " + trim(cStdout)
	? "   Stderr: " + trim(cStderr)
	? ""

	? "3. Interactive process with async:"
	? ""

	if isWindows()
		proc = proc_create(["cmd", "/c", "set /p x= & echo You said: %x%"],
			PROC_ENABLE_ASYNC | PROC_SEARCH_USER_PATH)
	else
		proc = proc_create(["sh", "-c", `read x && echo "You said: $x"`],
			PROC_ENABLE_ASYNC | PROC_SEARCH_USER_PATH)
	ok

	proc_write(proc, "Hello from Ring!" + char(10))
	proc_close_stdin(proc)

	cOutput = ""
	while proc_alive(proc)
		cChunk = proc_read_stdout(proc, 1024)
		if len(cChunk) > 0
			cOutput += cChunk
		ok
	end
	cChunk = proc_read_stdout(proc, 1024)
	while len(cChunk) > 0
		cOutput += cChunk
		cChunk = proc_read_stdout(proc, 1024)
	end

	proc_join(proc)
	proc_destroy(proc)

	? "   Sent: Hello from Ring!"
	? "   Response: " + trim(cOutput)
	? ""

	? "Done!"
