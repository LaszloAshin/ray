.model flat, C
.code

mysincosf PROC public
	fld     DWORD PTR [esp+4]
	fsincos
	fnstsw  ax
	test    ax, 0400h
	jz      a
	fldpi
	fadd    st, st(0)
	fxch    st(1)
b:	fprem1
	fnstsw  ax
	test    ax, 0400h
	jnz     b
	fstp    st(1)
	fsincos
a:	mov     eax, [esp+12]
	fstp    DWORD PTR [eax]
	mov     eax, [esp+8]
	fstp    DWORD PTR [eax]
	ret
mysincosf ENDP

myatan2f PROC public
	fld    DWORD PTR [esp+4]
	fld    DWORD PTR [esp+8]
	fpatan
	ret
myatan2f ENDP

END
