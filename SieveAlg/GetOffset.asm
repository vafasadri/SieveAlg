
.code                           ;Code Section begins
GetOffset    proc                
      ; returns p * max(p,ceil(bufferStart/p)) - bufferStart  
      ; bufferStart = rcx
      ; p = rdx
        push rbx
        push rcx
        mov rbx,rdx
        call DivCeil             
        cmp rax,rbx
        cmovl rax,rbx
        mul rbx
        pop rcx
        sub rax,rcx
        pop rbx       
        ret
GetOffset    endp

DivCeil proc
    ; ceil(a/b)    
    mov rax,rcx
    mov rcx,rdx
    cqo
    idiv rcx
    ; backup rax
    mov rcx,rax
    add rax,1
    test rdx,rdx
    cmovz rax,rcx
    ret
    DivCeil endp

innerLoop proc
    ; rcx = count
    ; rdx = p
    ; r8 = ptr
    test rcx,rcx
    jz fend
    add rdx,rdx
loopStart:
    mov byte ptr[r8],0
    add r8,rdx
    loop loopStart
 fend:
     ret
innerLoop endp

ToUpper proc

    mov rax,rcx
    xor rcx,rcx
    ; rax = char
    ; rcx = 0
    cmp rax,97
    setge cl
    cmp rcx,122
    setle ch
    ; now ah contains rcx <= 122
    ; and al contains rcx >= 97   
    mov rdx,rax   
    sub rax,32
    test al,ah
    cmovz rax,rdx   
    ret
    ToUpper endp
    end

