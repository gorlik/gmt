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
	
.setcpu		"6502"
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

.segment "DATA"
bankbank:
.byte $00,$00

.segment	"CODE"
	
; ****************** Bank Set *********************
.proc _Bank_Set: near
;	jsr pusha
;	ldy #$00
;	lda (sp),y		; load bank in A
	sta bankbank
	sta bankbank+1          ; reference data bank
        ldx #$00                ; complete the return value
;	jsr incsp1		; restore stak pointer
	rts
.endproc


; ****************** Bank Write *********************
_Bank_Write:
	lda bankbank
	sta $DFFF
	ldx #$00
	ldy _DPat
@loop: 
	stx $DFFE
.repeat 256,idx
	sty $DE00+idx	; store with $1 bytes stride
.endrepeat
	inx
	txa
	cmp #$40
	beq @done
	jmp @loop
@done:
        lda #$00        ; return value
	tax
	rts

; ****************** Bank Read *********************
_Bank_Read:
	lda bankbank
	sta $DFFF
	ldx #$00          	; high address in x
	ldy _DPat               ; data pattern in y
@fastloop:
	stx $DFFE	
.repeat 256,idx
        tya
        eor $DE00+idx
	beq	:+
	ldy #idx                ; fail address low in y
        jmp @breakerror
:
.endrepeat
	inx                     ; increase high address
	txa                     ;
	cmp #$40                ; compare with 16k
	beq @done
	jmp @fastloop

@slowloop:
        lda $DE00,y
        cmp _DPat
	bne @error
@resume:
	iny
	bne @slowloop
	inx
	txa
	cmp #$40
	stx $DFFE		
	bne @slowloop
@done: 
	rts

@breakerror:
        eor _DPat  ; recover bad read value

; fall through into error reader, and slow loop for the rest of the bank

@error:
	pha	   ; push bad value on stack
	txa	   ;
	pha	   ; push high address

	lda _NErr		; load current number of errors
	and #$000f		; limit to the size fo the array
	asl a			; multiply by 8 (size of one entry)
	asl a
	asl a
	tax			; put the entry offset into x
	tya
	sta _err_list,x	        ; store error address into error entry
	pla			; pull error address high
	sta _err_list+1,x	; store error address into error entry
	
	lda _DPat	        ; load expected pattern
	sta _err_list+2,x	; put expected pattern into error entry
	pla			; pull bad read back to A
	sta _err_list+3,x	; store bad read into error entry


	lda $DE00,y
	sta _err_list+4,x	; store re-read into error entry
	lda $DE00,y
	sta _err_list+5,x
	lda $DE00,y
	sta _err_list+6,x

	clc
        lda _NErr
	adc #$01		        ; increment error number
        sta _NErr
        lda _NErr+1
	adc #$00
	sta _NErr+1
	clc
	bcc @resume		; back to read test

; ****************** Addr as Data Write *********************
_AAD_Write:
	lda bankbank
	sta $DFFF
	ldx #$00
	ldy #$11
@loop: 
	stx $DFFE
.repeat 256,idx
	sty $DE00+idx	; store with $1 bytes stride
	iny
.endrepeat
	inx
	txa
	cmp #$40
	beq @done
	jmp @loop
@done:
	rts

	
; ****************** Addr as Data Read *********************
_AAD_Read:
	lda bankbank
	sta $DFFF
	ldx #$00          	; window address
	ldy #$00                ; low 8-bit addr
	
@loopb:	
	stx $DFFE
@loop:
	tya
        clc
	adc #$11
	eor $DE00,y
	bne @error
@resume:
	iny
	bne @loop
	inx
	txa
	cmp #$40
	bne @loopb
	rts

@error:
	sta _DPat
	tya
	adc #$11
	eor _DPat
	
	pha	   ; push bad value on stack
	txa	   ;
	pha	   ; push high address

	lda _NErr		; load current number of errors
	and #$000f		; limit to the size fo the array
	asl a			; multiply by 8 (size of one entry)
	asl a
	asl a
	tax			; put the entry offset into x
	tya
	sta _err_list,x	        ; store error address into error entry
	pla			; pull error address high
	sta _err_list+1,x	; store error address into error entry
	
	lda _DPat	        ; load expected pattern
	sta _err_list+2,x	; put expected pattern into error entry
	pla			; pull bad read back to A
	sta _err_list+3,x	; store bad read into error entry


	lda $DE00,y
	sta _err_list+4,x	; store re-read into error entry
	lda $DE00,y
	sta _err_list+5,x
	lda $DE00,y
	sta _err_list+6,x

	clc
        lda _NErr
	adc #$01		        ; increment error number
        sta _NErr
        lda _NErr+1
	adc #$00
	sta _NErr+1
	clc
	bcc @resume		; back to read test
