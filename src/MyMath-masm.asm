PUBLIC mysincosf

.code

; args: xmm0, rdx, r8
; https://docs.microsoft.com/en-us/cpp/build/x64-calling-convention?view=msvc-170
mysincosf PROC
        movss   dword ptr [rsp+8], xmm0
        fld     dword ptr [rsp+8]
        fsincos
        fnstsw  ax
        test    eax, 0400h
        jz      a
        fldpi
        fadd    st, st(0)
        fxch    st(1)
b:      fprem1
        fnstsw  ax
        test    eax, 0400h
        jnz     b
        fstp    st(1)
        fsincos
a:      fxch    st(1)
        fstp    dword ptr [rdx]
        fstp    dword ptr [r8]
        ret
mysincosf ENDP

END
