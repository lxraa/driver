
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
	;�ر�д����
	cli
	mov rax,cr0
	and rax,not 10000h
	mov cr0,rax
	;1.����ú�����ǰn�ֽ�ָ��
	mov rax,r8
	mov rbx,rcx

	mov rsi,rcx
	mov rcx, rax
	mov rdi,offset label2
	rep movsb
	;2.�޸�Ŀ�꺯��ǰn�ֽ�ָ�ʹ����ת��label
	sub rsi,rax
	mov byte ptr [rsi],0e9h
	add rsi,1
	;label - ָ��� -  address
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
	;�ر�д����
	push rax
	cli
	mov rax,cr0
	and rax,not 10000h
	mov cr0,rax
	pop rax
	ret
closeWriteProtect ENDP


openWriteProtect PROC
	;����д����
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
	;�ҹ�
	;1.����ԭ��������(��L)
	push rsi
	push rdi


	push rcx
	mov rsi,rcx
	mov rdi,offset L
	mov rcx,12
	rep movsb
	;2.�޸�ԭ��������
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

	;3.�޸Ĺҹ��������ش���(L2)
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