
.code 
myAdd PROC
	add rdx,rcx
	mov rax,rdx
	ret
myAdd ENDP
;rcx rdx r8 r9
;rcx:object address  rdx:my routine
hookFunc PROC
	push rbx
	push rax
	push rsi
	push rdi
	;关闭写保护
	cli
	mov rax,cr0
	and rax,not 10000h
	mov cr0,rax
	;1.保存该函数的前n字节指令
	mov rax,r8
	mov rbx,rcx

	mov rsi,rcx
	mov rcx, rax
	mov rdi,offset label2
	rep movsb
	;2.修改目标函数前n字节指令，使其跳转到label
	sub rsi,rax
	mov byte ptr [rsi],0e9h
	add rsi,1
	;label - 指令长度 -  address
	mov rdi,offset label1
	sub rdi,rax
	sub rdi,rbx
	push rdi
	mov rdi,rbx
	pop rsi
	mov rcx,rax
	sub rcx,1
	rep movsb



	mov rax,cr0
	or rax,10000h
	mov cr0,rax
	sti
	pop rdi
	pop rsi
	pop rax
	pop rbx
	ret
label1:
	push rcx
	push rdx
	push r8
	push r9
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	call myRoutine
	pop r9
	pop r8
	pop rdx
	pop rcx
label2:	
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

hookFunc ENDP

closeWriteProtect PROC
	;关闭写保护
	push rax
	cli
	mov rax,cr0
	and rax,not 10000h
	mov cr0,rax
	pop rax
	ret
closeWriteProtect ENDP


openWriteProtect PROC
	;开启写保护
	push rax
	mov rax,cr0
	or rax,10000h
	mov cr0,rax
	sti
	pop rax
	ret
openWriteProtect ENDP
testProc PROC
	mov rax,1234567812345678h
testProc ENDP

myRoutine PROC
	call closeWriteProtect
	;挂钩
	;1.复制原函数代码(到L)
	push rsi
	push rdi


	push rcx
	mov rsi,rcx
	mov rdi,offset L
	mov rcx,12
	rep movsb
	;2.修改原函数代码
	pop rdi
	push rdi
	;mov rax,0x0000000000000000	:48 b8 xxxxxxx
	;push rax: 50
	mov byte ptr [rdi],48h
	add rdi,1
	mov byte ptr [rdi],0b8h
	add rdi,1
	mov rax, offset L
	mov qword ptr [rdi],rax
	add rdi,8
	mov byte ptr [rdi],50h
	add rdi,1
	mov byte ptr [rdi],0c3h
	add rdi,1

	;3.修改挂钩函数返回代码(L2)
	pop rsi
	add rsi,12
	mov rdi,offset L2
	mov byte ptr [rdi],48h
	add rdi,1
	mov byte ptr [rdi],0b8h
	add rdi,1
	mov qword ptr [rdi],rsi
	add rdi,8
	mov byte ptr [rdi],50h
	add rdi,1
	mov byte ptr [rdi],0c3h
	add rdi,1



	pop rdi
	pop rsi
	call openWriteProtect
	ret
L:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
L2:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop



myRoutine ENDP

end