; Startup code for cc65 (C128 version)

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
	.include        "c128.inc"
	
; ------------------------------------------------------------------------
; Startup code

.segment        "STARTUP"

	jmp Start           ;Cold start
	jmp Start		;Warm start, default calls NMI exit.
	.byt	$01,$43,$42,$4D ;magic to identify cartridge

Start:

        sei
        ldx #$ff
        txs
        cld
        lda #$e3
        sta $01
        lda #$37
        sta $00

        lda #%00001010     ; Bank in Kernal ROM
		           ; BIT 0   : $D000-$DFFF (0 = I/O Block)
                           ; BIT 1   : $4000-$7FFF (1 = RAM)
                           ; BIT 2/3 : $8000-$BFFF (10 = External ROM)
                           ; BIT 4/5 : $C000-$CFFF/$E000-$FFFF (00 = Kernal ROM)
                           ; BIT 6/7 : RAM used. (00 = RAM 0)
        sta $ff00          ; MMU Configuration Register

        jsr $ff8a          ; Restore Vectors
        jsr $ff84          ; Init I/O Devices, Ports & Timers
        jsr $ff81          ; Init Editor & Video Chips


; Switch to second charset

	lda	#14
	jsr	BSOUT

; Clear the BSS data.

        jsr     zerobss

; Set up the stack.
	lda    	#<(__RAM_START__ + __RAM_SIZE__)
	sta	sp
	lda	#>(__RAM_START__ + __RAM_SIZE__)
       	sta	sp+1   		; Set argument stack ptr

NoIRQ1:
	lda	#<__DATA_RUN__
	ldx	#>__DATA_RUN__
	jsr	pushax
	lda	#<__DATA_LOAD__
	ldx	#>__DATA_LOAD__
	jsr	pushax
	lda	#<__DATA_SIZE__
	ldx	#>__DATA_SIZE__
	jsr	_memcpy

; Call the module constructors.

        jsr     initlib

; Call main().

        jsr     callmain

; Back from main() [this is also the exit() entry]. Run the module destructors.

_exit:  jsr	donelib

loop:   jmp loop
