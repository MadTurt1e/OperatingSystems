.section .bss
.lcomm buffer, 4096

.section .text
.global _start

_start:
	mov $0, %rax 							# syscall num for read
	mov $0, %rdi 							# read from stdin
	lea buffer(%rip), %rsi 		# read to buffer
	mov $4096, %rdx 					# read 4k
	syscall 									# call read
	mov %rax, %rdx 						# use bytes read as bytes to write
	mov $1, %rax 							# syscall num for write
	mov $1, %rdi 							# write to stdout
	syscall 									# call write
	mov $60, %rax 						# syscall for exit
	mov $0, %rdi 							# exit with 0
	syscall 									# call exit
