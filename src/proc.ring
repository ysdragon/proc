/*
	proc.ring - OOP Wrapper for Ring Proc
*/

load "proc.rh"

class Process

	pHandle = NULL
	nExitCode = 0
	lJoined = false

	func init aCommand, nOptions
		if nOptions = NULL
			nOptions = 0
		ok
		if isList(aCommand)
			pHandle = proc_create(aCommand, nOptions)
		else
			pHandle = proc_create([aCommand], nOptions)
		ok

	func initWithEnv aCommand, nOptions, aEnv
		if nOptions = NULL
			nOptions = 0
		ok
		if isList(aCommand)
			pHandle = proc_create_ex(aCommand, nOptions, aEnv)
		else
			pHandle = proc_create_ex([aCommand], nOptions, aEnv)
		ok
		return self

	func write cData
		if pHandle != NULL
			return proc_write(pHandle, cData)
		ok
		return 0

	func writeLine cData
		return write(cData + nl)

	func closeStdin
		if pHandle != NULL
			proc_close_stdin(pHandle)
		ok
		return self

	func readStdout nMaxBytes
		if pHandle != NULL
			return proc_read_stdout(pHandle, nMaxBytes)
		ok
		return ""

	func readStderr nMaxBytes
		if pHandle != NULL
			return proc_read_stderr(pHandle, nMaxBytes)
		ok
		return ""

	func stdout
		if pHandle != NULL
			return proc_stdout(pHandle)
		ok
		return ""

	func stderr
		if pHandle != NULL
			return proc_stderr(pHandle)
		ok
		return ""

	func join
		if pHandle != NULL and not lJoined
			nExitCode = proc_join(pHandle)
			lJoined = true
		ok
		return nExitCode

	func wait
		return join()

	func isAlive
		if pHandle != NULL
			return proc_alive(pHandle) = 1
		ok
		return false

	func terminate
		if pHandle != NULL
			return proc_terminate(pHandle)
		ok
		return 0

	func kill
		return terminate()

	func destroy
		if pHandle != NULL
			proc_destroy(pHandle)
			pHandle = NULL
		ok

	func exitCode
		return nExitCode

	func handle
		return pHandle

class Shell

	func run cCommand, nOptions
		if nOptions = NULL
			nOptions = 0
		ok
		return proc_shell(cCommand, nOptions)

	func exec aCommand, nOptions
		if nOptions = NULL
			nOptions = 0
		ok
		return proc_run(aCommand, nOptions)

	func capture cCommand
		result = proc_shell(cCommand)
		return trim(result[2])

	func captureLines cCommand
		result = proc_shell(cCommand)
		return str2list(result[2])

	func exitCode cCommand
		result = proc_shell(cCommand)
		return result[1]

	func success cCommand
		result = proc_shell(cCommand)
		return result[1] = 0
