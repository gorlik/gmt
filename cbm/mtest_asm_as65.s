;******************************************************************************
;*  GMT - GGLABS MEMORY TEST                                                  *
;*  A modern retro computer memory test optimized for coverage and speed      *
;*  Copyright 2022 Gabriele Gorla                                             *
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
.module gmt

.area XDATA
bankbank:
.db 0x00, 0x00

.globl _DPat
.globl _NErr
.globl _err_list

.globl _AAD_Write
.globl _AAD_Read
.globl _Bank_Write
.globl _Bank_Read
.globl _Bank_Set


.area CSEG (CODE)


; ****************** Bank Set *********************
_Bank_Set:
	sta bankbank
	sta bankbank+1          ; reference data bank
        ldx #0x00                ; complete the return value
	rts


; ****************** Bank Write *********************
_Bank_Write:
	lda bankbank
	sta 0xDFFF
	ldx #0x00
	ldy _DPat
bw_loop:
	stx 0xDFFE

idx=0			
.rept 256
	sty 0xDE00+idx
	idx = idx + 1
.endm	

	inx
	txa
	cmp #0x40
	beq bw_done
	jmp bw_loop
bw_done:
        lda #0x00        ; return value
	tax
	rts

.macro check1 idx label
	tya
	eor 0xde00+idx
	beq label
	ldy #idx
	jmp breakerror
label:
.endm
	
.macro check idx 
	check1 idx*8+0 l0'idx
	check1 idx*8+1 l1'idx
	check1 idx*8+2 l2'idx
	check1 idx*8+3 l3'idx
	check1 idx*8+4 l4'idx
	check1 idx*8+5 l5'idx
	check1 idx*8+6 l6'idx
	check1 idx*8+7 l7'idx
.endm
	
; ****************** Bank Read *********************
_Bank_Read:
	lda bankbank
	sta 0xDFFF
	ldx #0x00          	; high address in x
	ldy _DPat               ; data pattern in y
br_fastloop:
	stx 0xDFFE

	check 0
	check 1
	check 2
	check 3
	check 4
	check 5
	check 6
	check 7
	check 8
	check 9
	check 10
	check 11
	check 12
	check 13
	check 14
	check 15
	check 16
	check 17
	check 18
	check 19
	check 20
	check 21
	check 22
	check 23
	check 24
	check 25
	check 26
	check 27
	check 28
	check 29
	check 30
	check 31

	inx                     ; increase high address
	txa                     ;
	cmp #0x40                ; compare with 16k
	beq br_done
	jmp br_fastloop

br_slowloop:
        lda 0xDE00,y
        cmp _DPat
	bne br_error
br_resume:
	iny
	bne br_slowloop
	inx
	txa
	cmp #0x40
	stx 0xDFFE
	bne br_slowloop
br_done:
	rts

breakerror:
        eor _DPat  ; recover bad read value

; fall through into error reader, and slow loop for the rest of the bank

br_error:
	jsr log_error
	jmp br_resume

; ****************** Addr as Data Write *********************
_AAD_Write:
	lda bankbank
	sta 0xDFFF
	ldx #0x00
	ldy #0x11
aw_loop:
	stx 0xDFFE

idx=0		
.rept 256
	sty 0xDE00+idx
	iny
	idx = idx + 1
.endm
	inx
	txa
	cmp #0x40
	beq aw_done
	jmp aw_loop
aw_done:
	rts


; ****************** Addr as Data Read *********************
_AAD_Read:
	lda bankbank
	sta 0xDFFF
	ldx #0x00          	; window address
	ldy #0x00                ; low 8-bit addr

ar_loopb:
	stx 0xDFFE
ar_loop:
	tya
        clc
	adc #0x11
	eor 0xDE00,y
	bne aad_error
aad_resume:
	iny
	bne ar_loop
	inx
	cpx #0x40
	bne ar_loopb
	rts

aad_error:
	sta _DPat
	tya
	adc #0x11
	eor _DPat
	jsr log_error
        jmp aad_resume

log_error:
	pha	   ; push bad value on stack
	txa	   ;
	pha	   ; push high address

	lda _NErr		; load current number of errors
	and #0x0f		; limit to the size fo the array
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

	lda 0xDE00,y
	sta _err_list+4,x	; store re-read into error entry
	lda 0xDE00,y
	sta _err_list+5,x
	lda 0xDE00,y
	sta _err_list+6,x

	inc _NErr
	bne skip_inc
	inc _NErr+1
skip_inc:
	lda _err_list+1,x	; put address back in x
	tax
	rts
