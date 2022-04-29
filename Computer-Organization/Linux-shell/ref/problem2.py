import subprocess
import os
import sys

'''
The run function in particular is used here to execute commands in a subshell.
This saves us from going about forking and creaing a child process and the waiting
for the child to finish execution.
'''
def execute_commands(command):
	''' execute commands and handle piping'''
	try:
		if "|" and ">" in command:
			cmd="".join(command.split(">")[0])
			for files in command.split(">")[1:]:
				subprocess.call([cmd], stdout=open(files, 'w+'), shell=True)

		elif "|" in command:
			# save stdin and stdout for restoring later on
			s_in, s_out=(0, 0)
			s_in=os.dup(0)
			s_out=os.dup(1)

			#first command takes 
			fdin=os.dup(s_in)

			#iterate over all the commands that are piped
			for cmd in command.split("|"):
				#fdin will be stdin if it's the first iteration
				#and the readable end of the pipe if not
				os.dup2(fdin, 0)
				os.close(fdin)

				#restore stdout if this is the last command
				if cmd==command.split("|")[-1]:
					fdout=os.dup(s_out)
				else:
					fdin, fdout=os.pipe()

				#redirect stdout to pipe
				os.dup2(fdout, 1)
				os.close(fdout)

				try:
					subprocess.run(cmd.strip().split())
				except Exception:
					print("command not found: {}".format(cmd.strip()))

			#restore stdout and stdin
			os.dup2(s_in, 0)
			os.dup2(s_out, 1)
			os.close(s_in)
			os.close(s_out)

		elif ">" in command:
			cmd="".join(command.split(">")[0])
			for files in command.split(">")[1:]:
				subprocess.call([cmd], stdout=open(files, 'w'), shell=True)

		elif "<" in command:
			cmd="".join(command.split("<")[0])
			for files in command.split("<")[1:]:
				with open(files, 'r') as f:
					subprocess.call([cmd], stdin=f, shell=True)			
		else:
			subprocess.run(command.split(" "))
	except Exception:
		print("command not found: {}".format(command)+", maybe add space at each token")

def psh_help():
	print("shell implementation in Python. Supports all basic shell commands.")

def psh_cd(path):
	'''convert to absolute path and change directory'''
	try:
		os.chdir(os.path.abspath(path))  
	except Exception:
		print("cd: no such file or directory: {}".format(path)+", try: /path1/path2")

def main():
	while True:
		inp=input("$ ")
		if inp=="exit":
			break
		elif inp[0]=='c' and inp[1]=='d':
			psh_cd(inp[3:])
		elif inp=="help":
			psh_help()
		else:
			execute_commands(inp)

if __name__ == '__main__':
	main()