
.code                           ;Code Section begins
GetOffset    proc                
      ; returns p * max(p,ceil(bufferStart/p)) - bufferStart  
      ; bufferStart = rcx
      ; p = rdx
        push rbx       
        mov rax,rcx       
        mov rbx,rdx
        xor rdx,rdx
        div rbx
        test rdx,rdx
        setnz dl
        and rdx,1
        add rax,rdx              
        cmp rax,rbx
        cmovl rax,rbx
        mul rbx
        sub rax,rcx
        pop rbx       
        ret
GetOffset    endp
        end 