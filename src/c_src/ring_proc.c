/*
 * Ring Proc Extension
 * -------------------
 * A Ring language extension for subprocess management.
 * Wraps the subprocess.h library by sheredom.
 *
 * Author: Youssef Saeed (ysdragon)
 * License: MIT
 */

#include "ring.h"
#include "subprocess/subprocess.h"

#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * Constants - Process Options
 * ============================================================================ */

#define PROC_OPTION_COMBINED_STDOUT_STDERR subprocess_option_combined_stdout_stderr
#define PROC_OPTION_INHERIT_ENVIRONMENT subprocess_option_inherit_environment
#define PROC_OPTION_ENABLE_ASYNC subprocess_option_enable_async
#define PROC_OPTION_NO_WINDOW subprocess_option_no_window
#define PROC_OPTION_SEARCH_USER_PATH subprocess_option_search_user_path

/* ============================================================================
 * Helper Functions
 * ============================================================================ */

static void free_process(void *pState, void *pPointer)
{
	struct subprocess_s *proc = (struct subprocess_s *)pPointer;
	if (proc)
	{
		subprocess_destroy(proc);
		ring_state_free(pState, proc);
	}
}

/* Convert Ring list to NULL-terminated string array */
static char **ring_list_to_string_array(void *pPointer, List *pList)
{
	VM *pVM = (VM *)pPointer;
	int nSize = ring_list_getsize(pList);
	char **arr = (char **)ring_state_calloc(pVM->pRingState, nSize + 1, sizeof(char *));

	if (!arr)
		return NULL;

	for (int i = 1; i <= nSize; i++)
	{
		if (ring_list_isstring(pList, i))
		{
			const char *str = ring_list_getstring(pList, i);
			arr[i - 1] = (char *)str;
		}
		else
		{
			arr[i - 1] = NULL;
		}
	}
	arr[nSize] = NULL;

	return arr;
}

/* ============================================================================
 * Ring Functions - Process Creation
 * ============================================================================ */

/*
 * proc_create(aCommandLine, nOptions) -> pProcess
 *
 * Create a new subprocess.
 *
 * Parameters:
 *   aCommandLine - List of strings (command and arguments)
 *   nOptions     - Bitfield of options (optional, default 0)
 *
 * Returns:
 *   C pointer to the subprocess handle
 */
RING_FUNC(ring_proc_create)
{
	if (RING_API_PARACOUNT < 1 || RING_API_PARACOUNT > 2)
	{
		RING_API_ERROR(RING_API_BADPARACOUNT);
		return;
	}

	if (!RING_API_ISLIST(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}

	List *pCommandList = RING_API_GETLIST(1);
	int nOptions = 0;

	if (RING_API_PARACOUNT == 2)
	{
		if (!RING_API_ISNUMBER(2))
		{
			RING_API_ERROR(RING_API_BADPARATYPE);
			return;
		}
		nOptions = (int)RING_API_GETNUMBER(2);
	}

	/* Convert Ring list to string array */
	char **commandLine = ring_list_to_string_array(pPointer, pCommandList);
	if (!commandLine)
	{
		RING_API_ERROR("Failed to allocate memory for command line");
		return;
	}

	/* Allocate subprocess structure */
	VM *pVM = (VM *)pPointer;
	struct subprocess_s *proc =
		(struct subprocess_s *)ring_state_calloc(pVM->pRingState, 1, sizeof(struct subprocess_s));

	if (!proc)
	{
		ring_state_free(pVM->pRingState, commandLine);
		RING_API_ERROR("Failed to allocate memory for subprocess");
		return;
	}

	/* Create the subprocess */
	int result = subprocess_create((const char *const *)commandLine, nOptions, proc);

	/* Free the command line array (strings are still owned by Ring) */
	ring_state_free(pVM->pRingState, commandLine);

	if (result != 0)
	{
		ring_state_free(pVM->pRingState, proc);
		RING_API_ERROR("Failed to create subprocess");
		return;
	}

	RING_API_RETMANAGEDCPOINTER(proc, "subprocess", free_process);
}

/*
 * proc_create_ex(aCommandLine, nOptions, aEnvironment) -> pProcess
 *
 * Create a new subprocess with custom environment.
 *
 * Parameters:
 *   aCommandLine  - List of strings (command and arguments)
 *   nOptions      - Bitfield of options
 *   aEnvironment  - List of "KEY=VALUE" strings (or NULL/empty list)
 *
 * Returns:
 *   C pointer to the subprocess handle
 */
RING_FUNC(ring_proc_create_ex)
{
	if (RING_API_PARACOUNT != 3)
	{
		RING_API_ERROR(RING_API_MISS3PARA);
		return;
	}

	if (!RING_API_ISLIST(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}

	if (!RING_API_ISNUMBER(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}

	List *pCommandList = RING_API_GETLIST(1);
	int nOptions = (int)RING_API_GETNUMBER(2);

	VM *pVM = (VM *)pPointer;

	/* Convert command line */
	char **commandLine = ring_list_to_string_array(pPointer, pCommandList);
	if (!commandLine)
	{
		RING_API_ERROR("Failed to allocate memory for command line");
		return;
	}

	/* Convert environment (if provided) */
	char **environment = NULL;
	if (RING_API_ISLIST(3))
	{
		List *pEnvList = RING_API_GETLIST(3);
		if (ring_list_getsize(pEnvList) > 0)
		{
			environment = ring_list_to_string_array(pPointer, pEnvList);
		}
	}

	/* Allocate subprocess structure */
	struct subprocess_s *proc =
		(struct subprocess_s *)ring_state_calloc(pVM->pRingState, 1, sizeof(struct subprocess_s));

	if (!proc)
	{
		ring_state_free(pVM->pRingState, commandLine);
		if (environment)
			ring_state_free(pVM->pRingState, environment);
		RING_API_ERROR("Failed to allocate memory for subprocess");
		return;
	}

	/* Create the subprocess */
	int result =
		subprocess_create_ex((const char *const *)commandLine, nOptions, (const char *const *)environment, proc);

	/* Free arrays */
	ring_state_free(pVM->pRingState, commandLine);
	if (environment)
		ring_state_free(pVM->pRingState, environment);

	if (result != 0)
	{
		ring_state_free(pVM->pRingState, proc);
		RING_API_ERROR("Failed to create subprocess");
		return;
	}

	RING_API_RETMANAGEDCPOINTER(proc, "subprocess", free_process);
}

/* ============================================================================
 * Ring Functions - I/O Operations
 * ============================================================================ */

/*
 * proc_write(pProcess, cData) -> nBytesWritten
 *
 * Write data to the subprocess stdin.
 */
RING_FUNC(ring_proc_write)
{
	if (RING_API_PARACOUNT != 2)
	{
		RING_API_ERROR(RING_API_MISS2PARA);
		return;
	}

	if (!RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_NOTPOINTER);
		return;
	}

	if (!RING_API_ISSTRING(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}

	struct subprocess_s *proc = (struct subprocess_s *)RING_API_GETCPOINTER(1, "subprocess");
	if (!proc)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}

	const char *data = RING_API_GETSTRING(2);
	int size = RING_API_GETSTRINGSIZE(2);

	FILE *stdin_file = subprocess_stdin(proc);
	if (!stdin_file)
	{
		RING_API_ERROR("Subprocess stdin is not available");
		return;
	}

	size_t written = fwrite(data, 1, size, stdin_file);
	fflush(stdin_file);

	RING_API_RETNUMBER((double)written);
}

/*
 * proc_close_stdin(pProcess) -> nSuccess
 *
 * Close the subprocess stdin (signal EOF).
 */
RING_FUNC(ring_proc_close_stdin)
{
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}

	if (!RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_NOTPOINTER);
		return;
	}

	struct subprocess_s *proc = (struct subprocess_s *)RING_API_GETCPOINTER(1, "subprocess");
	if (!proc)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}

	FILE *stdin_file = subprocess_stdin(proc);
	if (stdin_file)
	{
		fclose(stdin_file);
		proc->stdin_file = NULL;
	}

	RING_API_RETNUMBER(1);
}

/*
 * proc_read_stdout(pProcess, nMaxBytes) -> cData
 *
 * Read data from subprocess stdout (async mode).
 */
RING_FUNC(ring_proc_read_stdout)
{
	if (RING_API_PARACOUNT != 2)
	{
		RING_API_ERROR(RING_API_MISS2PARA);
		return;
	}

	if (!RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_NOTPOINTER);
		return;
	}

	if (!RING_API_ISNUMBER(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}

	struct subprocess_s *proc = (struct subprocess_s *)RING_API_GETCPOINTER(1, "subprocess");
	if (!proc)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}

	unsigned int size = (unsigned int)RING_API_GETNUMBER(2);
	if (size == 0)
	{
		RING_API_RETSTRING("");
		return;
	}

	VM *pVM = (VM *)pPointer;
	char *buffer = (char *)ring_state_malloc(pVM->pRingState, size + 1);
	if (!buffer)
	{
		RING_API_ERROR("Failed to allocate read buffer");
		return;
	}

	unsigned int bytes_read = subprocess_read_stdout(proc, buffer, size);
	buffer[bytes_read] = '\0';

	RING_API_RETSTRING2(buffer, bytes_read);
	ring_state_free(pVM->pRingState, buffer);
}

/*
 * proc_read_stderr(pProcess, nMaxBytes) -> cData
 *
 * Read data from subprocess stderr (async mode).
 */
RING_FUNC(ring_proc_read_stderr)
{
	if (RING_API_PARACOUNT != 2)
	{
		RING_API_ERROR(RING_API_MISS2PARA);
		return;
	}

	if (!RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_NOTPOINTER);
		return;
	}

	if (!RING_API_ISNUMBER(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}

	struct subprocess_s *proc = (struct subprocess_s *)RING_API_GETCPOINTER(1, "subprocess");
	if (!proc)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}

	unsigned int size = (unsigned int)RING_API_GETNUMBER(2);
	if (size == 0)
	{
		RING_API_RETSTRING("");
		return;
	}

	VM *pVM = (VM *)pPointer;
	char *buffer = (char *)ring_state_malloc(pVM->pRingState, size + 1);
	if (!buffer)
	{
		RING_API_ERROR("Failed to allocate read buffer");
		return;
	}

	unsigned int bytes_read = subprocess_read_stderr(proc, buffer, size);
	buffer[bytes_read] = '\0';

	RING_API_RETSTRING2(buffer, bytes_read);
	ring_state_free(pVM->pRingState, buffer);
}

/*
 * proc_stdout(pProcess) -> cAllOutput
 *
 * Read all remaining stdout (blocking, for use after join).
 */
RING_FUNC(ring_proc_stdout)
{
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}

	if (!RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_NOTPOINTER);
		return;
	}

	struct subprocess_s *proc = (struct subprocess_s *)RING_API_GETCPOINTER(1, "subprocess");
	if (!proc)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}

	FILE *stdout_file = subprocess_stdout(proc);
	if (!stdout_file)
	{
		RING_API_RETSTRING("");
		return;
	}

	/* Read all data */
	VM *pVM = (VM *)pPointer;
	size_t capacity = 4096;
	size_t length = 0;
	char *buffer = (char *)ring_state_malloc(pVM->pRingState, capacity);

	if (!buffer)
	{
		RING_API_ERROR("Failed to allocate buffer");
		return;
	}

	int c;
	while ((c = fgetc(stdout_file)) != EOF)
	{
		if (length + 1 >= capacity)
		{
			capacity *= 2;
			char *new_buffer = (char *)ring_state_realloc(pVM->pRingState, buffer, length, capacity);
			if (!new_buffer)
			{
				ring_state_free(pVM->pRingState, buffer);
				RING_API_ERROR("Failed to reallocate buffer");
				return;
			}
			buffer = new_buffer;
		}
		buffer[length++] = (char)c;
	}
	buffer[length] = '\0';

	RING_API_RETSTRING2(buffer, length);
	ring_state_free(pVM->pRingState, buffer);
}

/*
 * proc_stderr(pProcess) -> cAllOutput
 *
 * Read all remaining stderr (blocking, for use after join).
 */
RING_FUNC(ring_proc_stderr)
{
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}

	if (!RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_NOTPOINTER);
		return;
	}

	struct subprocess_s *proc = (struct subprocess_s *)RING_API_GETCPOINTER(1, "subprocess");
	if (!proc)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}

	FILE *stderr_file = subprocess_stderr(proc);
	if (!stderr_file)
	{
		RING_API_RETSTRING("");
		return;
	}

	/* Read all data */
	VM *pVM = (VM *)pPointer;
	size_t capacity = 4096;
	size_t length = 0;
	char *buffer = (char *)ring_state_malloc(pVM->pRingState, capacity);

	if (!buffer)
	{
		RING_API_ERROR("Failed to allocate buffer");
		return;
	}

	int c;
	while ((c = fgetc(stderr_file)) != EOF)
	{
		if (length + 1 >= capacity)
		{
			capacity *= 2;
			char *new_buffer = (char *)ring_state_realloc(pVM->pRingState, buffer, length, capacity);
			if (!new_buffer)
			{
				ring_state_free(pVM->pRingState, buffer);
				RING_API_ERROR("Failed to reallocate buffer");
				return;
			}
			buffer = new_buffer;
		}
		buffer[length++] = (char)c;
	}
	buffer[length] = '\0';

	RING_API_RETSTRING2(buffer, length);
	ring_state_free(pVM->pRingState, buffer);
}

/* ============================================================================
 * Ring Functions - Process Control
 * ============================================================================ */

/*
 * proc_join(pProcess) -> nReturnCode
 *
 * Wait for subprocess to finish and get return code.
 */
RING_FUNC(ring_proc_join)
{
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}

	if (!RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_NOTPOINTER);
		return;
	}

	struct subprocess_s *proc = (struct subprocess_s *)RING_API_GETCPOINTER(1, "subprocess");
	if (!proc)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}

	int return_code = 0;
	int result = subprocess_join(proc, &return_code);

	if (result != 0)
	{
		RING_API_ERROR("Failed to join subprocess");
		return;
	}

	RING_API_RETNUMBER((double)return_code);
}

/*
 * proc_alive(pProcess) -> lIsAlive
 *
 * Check if subprocess is still running.
 */
RING_FUNC(ring_proc_alive)
{
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}

	if (!RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_NOTPOINTER);
		return;
	}

	struct subprocess_s *proc = (struct subprocess_s *)RING_API_GETCPOINTER(1, "subprocess");
	if (!proc)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}

	int alive = subprocess_alive(proc);
	RING_API_RETNUMBER((double)(alive != 0 ? 1 : 0));
}

/*
 * proc_terminate(pProcess) -> nSuccess
 *
 * Terminate (kill) the subprocess.
 */
RING_FUNC(ring_proc_terminate)
{
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}

	if (!RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_NOTPOINTER);
		return;
	}

	struct subprocess_s *proc = (struct subprocess_s *)RING_API_GETCPOINTER(1, "subprocess");
	if (!proc)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}

	int result = subprocess_terminate(proc);
	RING_API_RETNUMBER((double)(result == 0 ? 1 : 0));
}

/*
 * proc_destroy(pProcess) -> nSuccess
 *
 * Destroy subprocess and free resources.
 */
RING_FUNC(ring_proc_destroy)
{
	if (RING_API_PARACOUNT != 1)
	{
		RING_API_ERROR(RING_API_MISS1PARA);
		return;
	}

	if (!RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_NOTPOINTER);
		return;
	}

	struct subprocess_s *proc = (struct subprocess_s *)RING_API_GETCPOINTER(1, "subprocess");
	if (!proc)
	{
		RING_API_ERROR(RING_API_NULLPOINTER);
		return;
	}

	int result = subprocess_destroy(proc);

	/* Free the subprocess struct */
	VM *pVM = (VM *)pPointer;
	ring_state_free(pVM->pRingState, proc);

	/* Mark the pointer as freed */
	RING_API_SETNULLPOINTER(1);

	RING_API_RETNUMBER((double)(result == 0 ? 1 : 0));
}

/* ============================================================================
 * Ring Functions - Utility / High-Level
 * ============================================================================ */

/*
 * proc_run(aCommandLine) -> aResult
 *
 * Simple synchronous run - execute command and return result.
 * Returns a list: [return_code, stdout, stderr]
 */
RING_FUNC(ring_proc_run)
{
	if (RING_API_PARACOUNT < 1 || RING_API_PARACOUNT > 2)
	{
		RING_API_ERROR(RING_API_BADPARACOUNT);
		return;
	}

	if (!RING_API_ISLIST(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}

	List *pCommandList = RING_API_GETLIST(1);
	int nOptions = 0;

	if (RING_API_PARACOUNT == 2)
	{
		if (!RING_API_ISNUMBER(2))
		{
			RING_API_ERROR(RING_API_BADPARATYPE);
			return;
		}
		nOptions = (int)RING_API_GETNUMBER(2);
	}

	VM *pVM = (VM *)pPointer;

	/* Convert command line */
	char **commandLine = ring_list_to_string_array(pPointer, pCommandList);
	if (!commandLine)
	{
		RING_API_ERROR("Failed to allocate memory for command line");
		return;
	}

	/* Create subprocess */
	struct subprocess_s proc;
	memset(&proc, 0, sizeof(proc));

	int result = subprocess_create((const char *const *)commandLine, nOptions, &proc);
	ring_state_free(pVM->pRingState, commandLine);

	if (result != 0)
	{
		RING_API_ERROR("Failed to create subprocess");
		return;
	}

	/* Wait for completion */
	int return_code = 0;
	subprocess_join(&proc, &return_code);

	/* Read stdout */
	FILE *stdout_file = subprocess_stdout(&proc);
	size_t stdout_capacity = 4096;
	size_t stdout_len = 0;
	char *stdout_buf = (char *)ring_state_malloc(pVM->pRingState, stdout_capacity);

	if (stdout_buf && stdout_file)
	{
		int c;
		while ((c = fgetc(stdout_file)) != EOF)
		{
			if (stdout_len + 1 >= stdout_capacity)
			{
				stdout_capacity *= 2;
				char *new_buf = (char *)ring_state_realloc(pVM->pRingState, stdout_buf, stdout_len, stdout_capacity);
				if (!new_buf)
					break;
				stdout_buf = new_buf;
			}
			stdout_buf[stdout_len++] = (char)c;
		}
	}
	if (stdout_buf)
		stdout_buf[stdout_len] = '\0';

	/* Read stderr */
	FILE *stderr_file = subprocess_stderr(&proc);
	size_t stderr_capacity = 4096;
	size_t stderr_len = 0;
	char *stderr_buf = (char *)ring_state_malloc(pVM->pRingState, stderr_capacity);

	if (stderr_buf && stderr_file)
	{
		int c;
		while ((c = fgetc(stderr_file)) != EOF)
		{
			if (stderr_len + 1 >= stderr_capacity)
			{
				stderr_capacity *= 2;
				char *new_buf = (char *)ring_state_realloc(pVM->pRingState, stderr_buf, stderr_len, stderr_capacity);
				if (!new_buf)
					break;
				stderr_buf = new_buf;
			}
			stderr_buf[stderr_len++] = (char)c;
		}
	}
	if (stderr_buf)
		stderr_buf[stderr_len] = '\0';

	/* Cleanup */
	subprocess_destroy(&proc);

	/* Build result list */
	List *pResultList = RING_API_NEWLIST;
	ring_list_adddouble_gc(pVM->pRingState, pResultList, (double)return_code);

	if (stdout_buf)
	{
		ring_list_addstring2_gc(pVM->pRingState, pResultList, stdout_buf, stdout_len);
		ring_state_free(pVM->pRingState, stdout_buf);
	}
	else
	{
		ring_list_addstring_gc(pVM->pRingState, pResultList, "");
	}

	if (stderr_buf)
	{
		ring_list_addstring2_gc(pVM->pRingState, pResultList, stderr_buf, stderr_len);
		ring_state_free(pVM->pRingState, stderr_buf);
	}
	else
	{
		ring_list_addstring_gc(pVM->pRingState, pResultList, "");
	}

	RING_API_RETLIST(pResultList);
}

/*
 * proc_shell(cCommand) -> aResult
 *
 * Run a shell command string.
 * Returns a list: [return_code, stdout, stderr]
 */
RING_FUNC(ring_proc_shell)
{
	if (RING_API_PARACOUNT < 1 || RING_API_PARACOUNT > 2)
	{
		RING_API_ERROR(RING_API_BADPARACOUNT);
		return;
	}

	if (!RING_API_ISSTRING(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}

	const char *command = RING_API_GETSTRING(1);
	int nOptions = 0;

	if (RING_API_PARACOUNT == 2)
	{
		if (!RING_API_ISNUMBER(2))
		{
			RING_API_ERROR(RING_API_BADPARATYPE);
			return;
		}
		nOptions = (int)RING_API_GETNUMBER(2);
	}

	VM *pVM = (VM *)pPointer;

/* Build shell command */
#ifdef _WIN32
	const char *shell_cmd[] = {"cmd.exe", "/c", command, NULL};
#else
	const char *shell_cmd[] = {"/bin/sh", "-c", command, NULL};
#endif

	/* Create subprocess */
	struct subprocess_s proc;
	memset(&proc, 0, sizeof(proc));

	int result = subprocess_create(shell_cmd, nOptions | PROC_OPTION_SEARCH_USER_PATH, &proc);

	if (result != 0)
	{
		RING_API_ERROR("Failed to create shell subprocess");
		return;
	}

	/* Wait for completion */
	int return_code = 0;
	subprocess_join(&proc, &return_code);

	/* Read stdout */
	FILE *stdout_file = subprocess_stdout(&proc);
	size_t stdout_capacity = 4096;
	size_t stdout_len = 0;
	char *stdout_buf = (char *)ring_state_malloc(pVM->pRingState, stdout_capacity);

	if (stdout_buf && stdout_file)
	{
		int c;
		while ((c = fgetc(stdout_file)) != EOF)
		{
			if (stdout_len + 1 >= stdout_capacity)
			{
				stdout_capacity *= 2;
				char *new_buf = (char *)ring_state_realloc(pVM->pRingState, stdout_buf, stdout_len, stdout_capacity);
				if (!new_buf)
					break;
				stdout_buf = new_buf;
			}
			stdout_buf[stdout_len++] = (char)c;
		}
	}
	if (stdout_buf)
		stdout_buf[stdout_len] = '\0';

	/* Read stderr */
	FILE *stderr_file = subprocess_stderr(&proc);
	size_t stderr_capacity = 4096;
	size_t stderr_len = 0;
	char *stderr_buf = (char *)ring_state_malloc(pVM->pRingState, stderr_capacity);

	if (stderr_buf && stderr_file)
	{
		int c;
		while ((c = fgetc(stderr_file)) != EOF)
		{
			if (stderr_len + 1 >= stderr_capacity)
			{
				stderr_capacity *= 2;
				char *new_buf = (char *)ring_state_realloc(pVM->pRingState, stderr_buf, stderr_len, stderr_capacity);
				if (!new_buf)
					break;
				stderr_buf = new_buf;
			}
			stderr_buf[stderr_len++] = (char)c;
		}
	}
	if (stderr_buf)
		stderr_buf[stderr_len] = '\0';

	/* Cleanup */
	subprocess_destroy(&proc);

	/* Build result list */
	List *pResultList = RING_API_NEWLIST;
	ring_list_adddouble_gc(pVM->pRingState, pResultList, (double)return_code);

	if (stdout_buf)
	{
		ring_list_addstring2_gc(pVM->pRingState, pResultList, stdout_buf, stdout_len);
		ring_state_free(pVM->pRingState, stdout_buf);
	}
	else
	{
		ring_list_addstring_gc(pVM->pRingState, pResultList, "");
	}

	if (stderr_buf)
	{
		ring_list_addstring2_gc(pVM->pRingState, pResultList, stderr_buf, stderr_len);
		ring_state_free(pVM->pRingState, stderr_buf);
	}
	else
	{
		ring_list_addstring_gc(pVM->pRingState, pResultList, "");
	}

	RING_API_RETLIST(pResultList);
}

/* ============================================================================
 * Ring Functions - Constants
 * ============================================================================ */

RING_FUNC(ring_proc_option_combined_stdout_stderr)
{
	RING_API_RETNUMBER((double)PROC_OPTION_COMBINED_STDOUT_STDERR);
}

RING_FUNC(ring_proc_option_inherit_environment)
{
	RING_API_RETNUMBER((double)PROC_OPTION_INHERIT_ENVIRONMENT);
}

RING_FUNC(ring_proc_option_enable_async)
{
	RING_API_RETNUMBER((double)PROC_OPTION_ENABLE_ASYNC);
}

RING_FUNC(ring_proc_option_no_window)
{
	RING_API_RETNUMBER((double)PROC_OPTION_NO_WINDOW);
}

RING_FUNC(ring_proc_option_search_user_path)
{
	RING_API_RETNUMBER((double)PROC_OPTION_SEARCH_USER_PATH);
}

/* ============================================================================
 * Library Initialization
 * ============================================================================ */

RING_LIBINIT
{
	/* Process creation */
	RING_API_REGISTER("proc_create", ring_proc_create);
	RING_API_REGISTER("proc_create_ex", ring_proc_create_ex);

	/* I/O operations */
	RING_API_REGISTER("proc_write", ring_proc_write);
	RING_API_REGISTER("proc_close_stdin", ring_proc_close_stdin);
	RING_API_REGISTER("proc_read_stdout", ring_proc_read_stdout);
	RING_API_REGISTER("proc_read_stderr", ring_proc_read_stderr);
	RING_API_REGISTER("proc_stdout", ring_proc_stdout);
	RING_API_REGISTER("proc_stderr", ring_proc_stderr);

	/* Process control */
	RING_API_REGISTER("proc_join", ring_proc_join);
	RING_API_REGISTER("proc_alive", ring_proc_alive);
	RING_API_REGISTER("proc_terminate", ring_proc_terminate);
	RING_API_REGISTER("proc_destroy", ring_proc_destroy);

	/* High-level utilities */
	RING_API_REGISTER("proc_run", ring_proc_run);
	RING_API_REGISTER("proc_shell", ring_proc_shell);

	/* Constants */
	RING_API_REGISTER("get_proc_option_combined_stdout_stderr", ring_proc_option_combined_stdout_stderr);
	RING_API_REGISTER("get_proc_option_inherit_environment", ring_proc_option_inherit_environment);
	RING_API_REGISTER("get_proc_option_enable_async", ring_proc_option_enable_async);
	RING_API_REGISTER("get_proc_option_no_window", ring_proc_option_no_window);
	RING_API_REGISTER("get_proc_option_search_user_path", ring_proc_option_search_user_path);
}
