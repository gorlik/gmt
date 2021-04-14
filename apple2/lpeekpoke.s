;******************************************************************************
;*  GMT - GGLABS MEMORY TEST                                                  *
;*  A modern retro computer memory test optimized for coverage and speed      *
;*  Copyright 2017, 2018 Gabriele Gorla                                       *
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

.export 	_LPEEK
.export 	_LPOKE
;.export 	_LPOKEW

.segment	"CODE"

;; **************************** LONG PEEK *****************************
.proc _LPEEK: near
.I8
.A8
	jsr pushax
 	ldy #$02		; load bank into A
	lda (sp),y
	sta ppatch		; patch the code
	dey 			; load address in A,B
	lda (sp),y
	xba
	lda (sp)

	clc			; enable native mode
	xce			;
	rep #$10 		; force index to 16-b
;	sep #$20
	.I16

	tax			; transfer address from A,B to X
	lda $100000,x		; perform long peek
ppatch = *-1
	sec			; back to 6502 emulation
	xce
.I8
.A8
	jsr incsp3 		; restore stack pointer
	rts
.endproc

;; **************************** LONG POKE *****************************
.proc	_LPOKE: near
.I8
.A8
	jsr pusha	
 	ldy #$03 		; load bank into A
	lda (sp),y
	sta ppatch  		; patch the code
	dey			; load address in A,B
	lda (sp),y
	xba
	dey
	lda (sp),y

	clc			; enable native mode
	xce			;
	rep #$10 		; force index to 16-b
;	sep #$20
.I16
	tax			; transfer address from A,B to X
	lda (sp)                ; load data to poke
	sta $100000,x		; perform long poke
ppatch = *-1
	sec			; back to 6502 emulation
	xce			;
.I8
.A8
	jsr incsp4		; restore stack pointer
	rts
.endproc

;; **************************** LONG WORD POKE ************************
;.proc	_LPOKEW: near
;.I8
;.A8
;	jsr pusha
; 	ldy #$04 		; load bank into A
;	lda (sp),y
;	sta ppatch  		; patch the code
;	dey			; load address in A,B
;	dey
;	clc			; enable native mode
;	xce			;
;	rep #$30 		; force index and acc to 16-b
;.I16
;.A16
;	lda (sp),y
;	tax			; transfer address from A,B to X
;	lda (sp)                ; load data to poke
;	sta $100000,x		; perform long poke
;ppatch = *-1
;	sec			; back to 6502 emulation
;	xce			;
;.I8
;.A8
;	jsr incsp5		; restore stack pointer
;	rts
;.endproc
