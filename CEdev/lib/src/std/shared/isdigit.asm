	.def	_isdigit
	.ref	___maptab
	.assume adl=1

;int isdigit(int c) {
;	if( (unsigned char)c & (unsigned char)0x80 )
;		return (0);
;	return(__maptab[c] & (unsigned char)(DIG));
;}

_isdigit:
	pop	hl
	pop	de
	push	de
	push	hl
	or	a,a
	sbc	hl,hl
	bit	7,e
	ret	nz
	ex	de,hl
	ld	de,___maptab
	add	hl,de
	ld	a,(hl)
	and	a,1
	sbc	hl,hl
	ld	l,a
	ret
