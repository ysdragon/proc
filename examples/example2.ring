/*
	Example 2: Interactive Process with stdin/stdout
	------------------------------------------------
	Demonstrates writing to stdin and reading from stdout.
	Cross-platform compatible.
*/

load "proc.ring"

func main
	? "=== Ring Proc - Interactive Process ==="
	? ""

	? "1. Processing data through 'sort' command:"
	if isWindows()
		proc = proc_create(["cmd", "/c", "sort"], PROC_SEARCH_USER_PATH)
	else
		proc = proc_create(["sort"], PROC_SEARCH_USER_PATH)
	ok

	cData = "zebra" + nl + "apple" + nl + "mango" + nl + "banana" + nl
	proc_write(proc, cData)
	proc_close_stdin(proc)
	proc_join(proc)

	? "   Input:  zebra, apple, mango, banana"
	? "   Sorted output:"
	aLines = str2list(proc_stdout(proc))
	for line in aLines
		if len(trim(line)) > 0
			? "      " + trim(line)
		ok
	next
	proc_destroy(proc)
	? ""

	? "2. Using 'cat' to echo data:"
	if isWindows()
		proc = proc_create(["cmd", "/c", "more"], PROC_SEARCH_USER_PATH)
	else
		proc = proc_create(["cat"], PROC_SEARCH_USER_PATH)
	ok

	aLines = ["Hello from Ring!", "Line 2", "Line 3"]
	for line in aLines
		proc_write(proc, line + nl)
	next
	proc_close_stdin(proc)
	proc_join(proc)

	? "   Sent " + len(aLines) + " lines"
	? "   Output:"
	aOutput = str2list(proc_stdout(proc))
	for line in aOutput
		if len(trim(line)) > 0
			? "      " + trim(line)
		ok
	next
	proc_destroy(proc)
	? ""

	? "3. Using 'wc' to count lines (Unix) / 'find /c' (Windows):"
	if isWindows()
		proc = proc_create(["cmd", "/c", "find", "/c", "/v", `""`], PROC_SEARCH_USER_PATH)
	else
		proc = proc_create(["wc", "-l"], PROC_SEARCH_USER_PATH)
	ok

	cText = "Line 1" + nl + "Line 2" + nl + "Line 3" + nl + "Line 4" + nl + "Line 5" + nl
	proc_write(proc, cText)
	proc_close_stdin(proc)
	proc_join(proc)

	? "   Sent 5 lines"
	? "   Count result: " + trim(proc_stdout(proc))
	proc_destroy(proc)
	? ""

	? "4. Using 'grep' to filter (Unix) / 'findstr' (Windows):"
	if isWindows()
		proc = proc_create(["findstr", "ring"], PROC_SEARCH_USER_PATH)
	else
		proc = proc_create(["grep", "ring"], PROC_SEARCH_USER_PATH)
	ok

	cText = "python is a language" + nl +
	        "ring is awesome" + nl +
	        "java is verbose" + nl +
	        "ring programming" + nl +
	        "ruby is dynamic" + nl
	proc_write(proc, cText)
	proc_close_stdin(proc)
	proc_join(proc)

	? "   Searching for 'ring' in 5 lines"
	? "   Matches:"
	aOutput = str2list(proc_stdout(proc))
	for line in aOutput
		if len(trim(line)) > 0
			? "      " + trim(line)
		ok
	next
	proc_destroy(proc)
	? ""

	? "Done!"
