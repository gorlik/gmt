;******************************************************************************
;*  GMT - GGLABS MEMORY TEST                                                  *
;*  A modern retro computer memory test optimized for coverage and speed      *
;*  Copyright 2017, 2018 Gabriele Gorla and Jason Andersen                    *
;*                                                                            *
;*  This program is free software: you can redistribute it and/or modify      *
;*  it under the terms of the GNU General Public License as published by      *
;*  the Free Software Foundation, either version 3 of the License, or         *
;*  (at your option) any later version.                                       *
;*                                                                            *
;*  GMT is distributed in the hope that it will be useful,                    *
;*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
;*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
;*  GNU General Public License for more details.                              *
;*                                                                            *
;*  You should have received a copy of the GNU General Public License         *
;*  along with GMT.  If not, see <http://www.gnu.org/licenses/>.              *
;*                                                                            *
;******************************************************************************
	
.setcpu		"65816"
.autoimport 	on
.importzp	sp
.importzp       ptr1
.importzp       ptr2

.import 	_err_list
.import 	_DPat
.import 	_NErr

.export 	_Bank_Set
.export 	_Bank_Write
.export 	_Bank_Read
.export 	_AAD_Write
.export 	_AAD_Read

.segment	"CODE"

bankbank:
.byte $00,$00

; ****************** Bank Set *********************
.proc _Bank_Set: near
.I8
.A8
;	lda (sp)		; load bank in A
	sta bankbank
	sta bankbank+1          ; reference data bank

;	jsr incsp1		; restore stak pointer
	rts
.endproc


; ****************** Bank Write *********************
_Bank_Write:
.I8
.A8
	php
	phb                     ; preserve current data bank
	lda bankbank
	pha
	clc			; enable native mode
	xce			;
	rep #$31 		; force m and x to 16-b, clc
.I16
.A16
	lda _DPat
	plb
	tay
	ldx #0000
	
@loop: 
.repeat 256,idx
	sta a:idx*$100,x	; store with $100 bytes stride
.endrepeat
	txa
	inc
	inc
	cmp #$0100
	beq @done
	tax
	tya
	jmp @loop

@done:
	xce			; back to 6502
.I8
.A8
	plb                     ; restore data bank
        plp
	rts

; ****************** Bank Read *********************
_Bank_Read:
.I8
.A8
        php        
	phb
	lda bankbank
	pha
	clc			; enable native mode
	xce			;
	rep #$31 		; force m and x to 16-b, clc
.I16
.A16
	ldx #$0000
	lda _DPat
        sta z:ptr1              ; _DPat copy in the zero page 
	tay
	plb

@fastloop:
.repeat 128,idx
        tya
        eor a:idx*2,x
	beq	:+
        pea idx*2
        jmp @breakerror
:
.endrepeat
	txa
	clc
	adc #$100
	bcs @done
	tax
	jmp @fastloop

@slowloop:
        lda a:$0000,x
        cmp z:ptr1 ;;f:_DPat
	bne @error
@resume:
        inx
	inx
	bne @slowloop
@done: 
	sec			; back to 6502 emulation
	xce			;
.I8
.A8
	plb
        plp
	rts

.I16
.A16

@breakerror:
        eor z:ptr1 ; recover bad read value
        tay        ; save bad read value in y
        stx z:ptr2 ; x into direct page
        pla        ; the value from the earlier pea
; c is supposed to already be clear
        adc z:ptr2
        tax   ; x now contains real bad address
        tya   ; a is the bad read value
                 
; fall through into error reader, and slow loop for the rest of the bank

@error:
        phx			; push the error address
	tay			; move bad read to y
	
	lda f:_NErr		; load current number of errors
	and #$000f		; limit to the size fo the array
	asl a			; multiply by 16 (size of one entry)
	asl a
	asl a
	asl a
	tax			; put the entry offset into x

	tya			; move bad read back to A
	sta f:_err_list+4,x	; store bad read into error entry

	lda ptr1 ;f:_DPat	; load expected pattern
	sta f:_err_list+2,x	; put expected pattern into error entry

	ply			; pull error address 
	tya			; move to A
	sta f:_err_list,x	; store error address into error entry

	jsr @reread		; re-read @ bad address
	sta f:_err_list+6,x	; store re-read into error entry
	jsr @reread
	sta f:_err_list+8,x
	jsr @reread
	sta f:_err_list+10,x

        lda f:_NErr
	inc		        ; increment error number
        sta f:_NErr

        tyx    
	bra @resume		; back to read test

@reread:
	lda a:$0000,y
	rts


; ****************** Addr as Data Write *********************
_AAD_Write:
.I8
.A8
	php
	phb                     ; preserve current data bank
	lda bankbank
	pha
	clc			; enable native mode
	xce			;
	rep #$31 		; force m and x to 16-b, clc
.I16
.A16
;	lda #$0011		;
	plb
	ldx #$0000 		; x offset 
	
@loop: 
.repeat 256,idx
	txa
	lsr
	adc #((idx*$80)+$11)   	; data is (addr/2)+$11
	sta a:idx*$100,x	; store with $100 bytes stride
.endrepeat
	txa
	inc
	inc
	cmp #$0100
	beq @done
	tax
	jmp @loop

@done:
	xce			; back to 6502
.I8
.A8
	plb                     ; restore data bank
        plp
	rts

; ****************** Addr as Data Read *********************
_AAD_Read:
.I8
.A8
        php        
	phb
	lda bankbank
	pha
	clc			; enable native mode
	xce			;
	rep #$31 		; force m and x to 16-b, clc
.I16
.A16
	ldx #$0000		; address
	lda #$0000
        sta z:ptr1              ; _DPat copy in the zero page 
	tay
	plb

@loop:
	txa
	lsr
	adc #$11
	eor a:$0000,x
	bne @error
@resume:
	inx
	inx
	bne @loop

@done: 
	sec			; back to 6502 emulation
	xce			;
.I8
.A8
	plb
        plp
	rts

.I16
.A16

@error:
	sta z:ptr1 		; save accumulator (read value xor expected)
	txa
	lsr
	adc #$11
	tay			; move expected value to y
        eor z:ptr1 		; recover read value
	sta z:ptr1              ; store read value
        phx			; push the error address
	
	lda f:_NErr		; load current number of errors
	and #$000f		; limit to the size fo the array
	asl a			; multiply by 16 (size of one entry)
	asl a
	asl a
	asl a
	tax			; put the entry offset into x

	lda z:ptr1
	sta f:_err_list+4,x	; store bad read into error entry

	tya			; move expected back to A
	sta f:_err_list+2,x	; put expected pattern into error entry
	
	ply			; pull error address 
	tya			; move to A
	sta f:_err_list,x	; store error address into error entry

	jsr @reread		; re-read @ bad address
	sta f:_err_list+6,x	; store re-read into error entry
	jsr @reread
	sta f:_err_list+8,x
	jsr @reread
	sta f:_err_list+10,x

        lda f:_NErr
	inc		        ; increment error number
        sta f:_NErr

        tyx    
	bra @resume		; back to read test

@reread:
	lda a:$0000,y
	rts
