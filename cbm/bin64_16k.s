;
; Startup code for cc65 (C64 version)
;

        .export         _exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup
        .import         initlib, donelib
        .import         zerobss, callmain
	.import	     	pushax
        .import         BSOUT
        .import         __INTERRUPTOR_COUNT__
        .import         __RAM_START__, __RAM_SIZE__	; Linker generated

	.import		_memcpy
	.import		__DATA_LOAD__, __DATA_RUN__, __DATA_SIZE__

        .include        "zeropage.inc"
        .include        "c64.inc"

; ------------------------------------------------------------------------
; Startup code

.segment        "STARTUP"

	.word	Start           ;Cold start
	.word	$FEBC		;Warm start, default calls NMI exit.
	.byt	$C3,$C2,$CD,$38,$30 ;magic to identify cartridge

Start:

	jsr	$FF84		;Init. I/O
	jsr	$FF87		;Init. RAM
	jsr	$FF8A		;Restore vectors
	jsr	$FF81		;Init. screen

; Set up the stack.
	lda    	#<(__RAM_START__ + __RAM_SIZE__)
	sta	sp
	lda	#>(__RAM_START__ + __RAM_SIZE__)
       	sta	sp+1   		; Set argument stack ptr

	lda	#<__DATA_RUN__
	ldx	#>__DATA_RUN__
	jsr	pushax
	lda	#<__DATA_LOAD__
	ldx	#>__DATA_LOAD__
	jsr	pushax
	lda	#<__DATA_SIZE__
	ldx	#>__DATA_SIZE__
	jsr	_memcpy

; Switch to second charset

	lda	#14
	jsr	BSOUT

; Call module constructors

        jsr     initlib

; Clear the BSS data.

        jsr     zerobss

; call main().

        jsr     callmain

; Back from main() [this is also the exit() entry]. Run the module destructors.

_exit:  jsr	donelib

	jmp	64738		;Kernal reset address as best I know it.
