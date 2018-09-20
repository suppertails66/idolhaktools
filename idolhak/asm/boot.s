
.memorymap
   defaultslot     1
   
   ;===============================================
   ; RAM area
   ;===============================================
   
   slotsize        $800
   slot            0       $0000
   slotsize        $2000
   slot            1       $6000
   
   ;===============================================
   ; ROM area
   ;===============================================
   
   slotsize        $4000
   slot            2       $8000
   slot            3       $C000
.endme

.rombankmap
  bankstotal 16
  banksize $4000
  banks 16
.endro

.emptyfill $FF

.background "idolhak.nes"

; unbackground expanded ROM space
.unbackground $1C100 $1FF00
.unbackground $20000 $3BFFF
; unbackground now-unused space for dialogue in each bank
.unbackground $1AEF $22B7
.unbackground $51B1 $7430
.unbackground $90A4 $AFE9
.unbackground $CF9A $F35F
.unbackground $1104C $13411
.unbackground $15449 $173B3

.define PPUCTRL $2000
.define PPUMASK $2001
.define PPUSTATUS $2002
.define OAMADDR $2003
.define OAMDATA $2004
.define PPUSCROLL $2005
.define PPUADDR $2006
.define PPUDATA $2007
.define OAMDMA $4014

; n.b.:
;
; * byte 0 of every bank is reserved for the MMC1 reset byte
; * bytes 0x3FD0+ of every bank are reserved for the reset vector code

;===============================================
; Constants
;===============================================

.define currentStringNum $0067
.define ppuQueueSize $0096
.define currentBank $00AD
.define ppuQueue $0590

.define writeMapperReg $DD55

.define newScriptBankOffset $08

.define scratch1   $01E1
  .define scratch1Lo $01E1
  .define scratch1Hi $01E2
.define scratch2   $01E3
  .define scratch2Lo $01E3
  .define scratch2Hi $01E4

;===============================================
; single-space dialogue
;===============================================

/*.define dlgPpuBase $2243
.define numNewLines $08

.bank $0F slot 3
.org $14DB
.section "single-space dialogue 1" overwrite
  ; multiply linenum by 0x20 instead of 0x40
  ldx #$20
.ends

.bank $0F slot 3
.org $14E0
.section "single-space dialogue 2" overwrite
  ; add base PPU pos to result of previous multiplication
  clc
  lda $003F
  adc #<dlgPpuBase
  sta $0065
  lda $0040
  adc #>dlgPpuBase
  sta $0066
  
.ends

.bank $0F slot 3
.org $158D
.section "single-space dialogue 3" overwrite
  ; set line count limit
  cmp #numNewLines
.ends

.bank $0F slot 3
.org $14A2
.section "single-space dialogue 4" overwrite
  ; prevent old diacritical row from being cleared
  ; (otherwise, the top border of the dialogue box will be erased)
  ;
  ; I think this causes both of the PPU clear writes to get directed
  ; to the same row but should probably check more closely
  jmp $D4AF
.ends

.bank $0F slot 3
.org $1484
.section "single-space dialogue 5" overwrite
  ; clear all lines
  cmp #numNewLines
.ends */

; alt version that just moves things up a line to match the
; altered menus

.define dlgPpuBase $2243

.bank $0F slot 3
.org $14E0
.section "single-space dialogue 2" overwrite
  ; add base PPU pos to result of previous multiplication
  clc
  lda $003F
  adc #<dlgPpuBase
  sta $0065
  lda $0040
  adc #>dlgPpuBase
  sta $0066
  
.ends

.bank $0F slot 3
.org $14A2
.section "single-space dialogue 4" overwrite
  ; prevent old diacritical row from being cleared
  ; (otherwise, the top border of the dialogue box will be erased)
  ;
  ; I think this causes both of the PPU clear writes to get directed
  ; to the same row but should probably check more closely
  jmp $D4AF
.ends

;===============================================
; text speedup
;===============================================

.define textDelayReloadValue $01

.bank $0F slot 3
.org $1488
.section "text speedup 1" overwrite
  ; intial delay counter when ending step 3
  lda #textDelayReloadValue
.ends

.bank $0F slot 3
.org $1527
.section "text speedup 2" overwrite
  ; delay counter reload value
  lda #textDelayReloadValue
.ends

;===============================================
; text compression
;===============================================

.define textCmpRangeLow $C8
.define textCmpRangeHigh $F8
.define textCmpRangeLimit $F9

.define textCompressionFlag $01E0

.bank $0F slot 3
.org $1524
.section "trigger text decompression" overwrite
  jmp checkForTextDecompression
.ends

.bank $0F slot 3
.section "text decompression" free
  checkForTextDecompression:
    ; compressed character range = C8-F8
    cmp #textCmpRangeLow
    bcc @standardPrint
    cmp #textCmpRangeLimit
    bcs @standardPrint
    
      ; save character index
      sta $0026
      
      ; reset printing delay counter (so we can simplify later jumps
      ; back to standard logic)
      lda #textDelayReloadValue
      sta $0063
      
      ; need to do second half of a previous character?
      lda textCompressionFlag
      bne decmpSecondCharHalf
      beq decmpFirstCharHalf
    
    @standardPrint:
    ; do regular printing logic
    jsr $D62C
    ; jump back to normal logic
    jmp $D527
  
  decmpFirstCharHalf:
    ; get character index
    lda $0026
  
    ; flag transfer as active
    sta textCompressionFlag
    
    ; look up index from table
    sec
    sbc #textCmpRangeLow
    asl
    tax
    lda compressionTable.w,X
    
    ; print character
    jsr $D62C
    
    ; skip srcaddr increment
    jmp $D532
  
  decmpSecondCharHalf:
    
    ; look up index from table
    sec
    sbc #textCmpRangeLow
    asl
    tax
    inx ; target second character
    lda compressionTable.w,X
    
    ; print character
    jsr $D62C
    
    ; clear active decmp flag
    lda #$00
    sta textCompressionFlag
    
    ; increment srcaddr and do normal logic
    jmp $D52B
    
  compressionTable:
    .incbin "out/cmptbl/cmptbl.bin"
    
.ends

;===============================================
; no diacritics
;===============================================

; regular dialogue
.bank $0F slot 3
.org $162E
.section "no diacritic check dialogue" overwrite
  jmp $D632
.ends

; menus
.bank $0F slot 3
.org $1836
.section "no diacritic check menus" overwrite
  jmp $D83A
.ends

; ending karaoke
;.bank $00 slot 2
;.org $3BF7
;.section "no diacritic check ending karaoke" overwrite
;  jmp $BC11
;.ends

; unbackground diacritic resources we no longer need

; diacritic printing prep
.unbackground $3D649 $3D668
; conversion table and routine
;.unbackground $3D345 $3D399
.unbackground $3D345 $3D394
; menu PPU prep
.unbackground $3D854 $3D89D

; unbackground old menus
.unbackground $3D8BF $3DD1E

;===============================================
; new script
;===============================================

; n.b.: WLA-DX does not do macros that would actually be useful
/*.macro defineScriptBank
  .bank (newScriptBankOffset+\1) slot 2
  .org $0004
  .section "new script bank\1 pointer" overwrite
    .dw newScriptBank\1
  .ends

  .bank (newScriptBankOffset+\1) slot 2
  .org $0010
  .section "new script bank\1" overwrite
    newScriptBank\1:
      .incbin "out/script/script_\1.bin"
  .ends
.endm */

.bank $08 slot 2
.org $0004
.section "new script bank0 pointer" overwrite
  .dw newScriptBank0
.ends

.bank $08 slot 2
.org $0010
.section "new script bank0" overwrite
  newScriptBank0:
    .incbin "out/script/script_0.bin"
.ends

.bank $09 slot 2
.org $0004
.section "new script bank1 pointer" overwrite
  .dw newScriptBank1
.ends

.bank $09 slot 2
.org $0010
.section "new script bank1" overwrite
  newScriptBank1:
    .incbin "out/script/script_1.bin"
.ends

.bank $0A slot 2
.org $0004
.section "new script bank2 pointer" overwrite
  .dw newScriptBank2
.ends

.bank $0A slot 2
.org $0010
.section "new script bank2" overwrite
  newScriptBank2:
    .incbin "out/script/script_2.bin"
.ends

.bank $0B slot 2
.org $0004
.section "new script bank3 pointer" overwrite
  .dw newScriptBank3
.ends

.bank $0B slot 2
.org $0010
.section "new script bank3" overwrite
  newScriptBank3:
    .incbin "out/script/script_3.bin"
.ends

.bank $0C slot 2
.org $0004
.section "new script bank4 pointer" overwrite
  .dw newScriptBank4
.ends

.bank $0C slot 2
.org $0010
.section "new script bank4" overwrite
  newScriptBank4:
    .incbin "out/script/script_4.bin"
.ends

.bank $0D slot 2
.org $0004
.section "new script bank5 pointer" overwrite
  .dw newScriptBank5
.ends

.bank $0D slot 2
.org $0010
.section "new script bank5" overwrite
  newScriptBank5:
    .incbin "out/script/script_5.bin"
.ends

; menus + stuff

.bank $0E slot 2
.org $0001
.section "new menus" overwrite
  newMenus:
    .incbin "out/script/menus.bin"
  
  doMenuCopy:
    ; copy row 1
    ldy #$00
    jsr menuCopyLoop
    
    ; set up second transfer at (ppuaddr + 0x20)
    lda $0017
    clc
    adc #$20
    sta $0591,X
    sta $0017
    lda $0018
    adc #$00
    sta $0590,X
    sta $0018
    inx
    inx
    
    ; copy row 2
    jmp menuCopyLoop
    
  menuCopyLoop:
    lda #$07
    sta $001A
    -:
      lda ($0030),Y
      sta $0590,X
      ; increment get/putpos
      inx 
      iny 
      ; done when 7 characters copied
      dec $001A
      bne -
    
    ; write terminator
    lda #$FF
    sta $0590,X
    inx
    
    rts
.ends

;===============================================
; use new banks for dialogue
;===============================================

; update 0038 jump table entries to point to new routine
.bank $0F slot 3
.org $0E9D
.section "new banks for dialogue trigger" overwrite
  .dw newDlgLookup,newDlgLookup,newDlgLookup,newDlgLookup,newDlgLookup,newDlgLookup
.ends

.bank $0F slot 3
.section "new banks for dialogue" free
  newDlgLookup:
    ; save current bank num
    lda currentBank
    pha
      
      ; check if current printing step requires new dialogue bank
      
      lda $0038
      sta scratch1Lo
      cmp #$01
      beq @useNewBank
      cmp #$04
      bne @useOldBank
      
        ; step 04 requires several initial calls (add sprite obejcts?) to be
        ; performed with the old bank
        
        ; if $00B5 nonzero, update logic will be skipped and we don't do these
        ; calls
        lda $00B5
        bne @useNewBank
          jsr $DD7F
          jsr $E9EC
          jsr $EA30
          jsr $EA7A
          
      
      @useNewBank:
      
        ; load bank containing dialogue for bank
        lda currentBank
        clc
        adc #newScriptBankOffset
        sta scratch1Lo
        
        @changeBank:
        
        ; write to MMC1 banking reg
        ldx #03
        jsr writeMapperReg
      
      @useOldBank:
      
      ; call original printing routine
      jsr $D39C
    
    ; retrieve orig banknum
    pla
    
    ; under CIRCUMSTANCES (only game overs in act 4?), the program may
    ; change the bank during the previous call and expect the result to stick.
    ; so, we check if the number in 00AD after the call matches the one we
    ; set it to; if not; we do not restore the old bank.
    ldx currentBank
    cpx scratch1Lo
    bne @done
      
      ; write bank num to MMC1 banking reg
      ldx #03
      jmp writeMapperReg
    
    @done:
    rts
  
.ends

; print step 4: don't do moved sprite update calls with new bank
.bank $0F slot 3
.org $1507
.section "print step 4: don't use old sprite updates" overwrite
  jmp $D513
.ends

; restore original bank for op C7
.bank $0F slot 3
.org $14FB
.section "orig bank for op c7 1" overwrite
  jmp opC7Handler
.ends

; restore original bank for op C7
.bank $0F slot 3
.section "orig bank for op c7 2" free
  opC7Handler:
    ; restore "normal" bank for whatever it is we're doing
    lda currentBank
    sec
    sbc #newScriptBankOffset
    ; write to MMC1 banking reg
    ldx #03
    jsr writeMapperReg
    
    ; make up work
    lda #$01
    ldx #$5F
    jmp $D4FF
    
.ends

;===============================================
; use new menu labels
;===============================================

.define menuPpuBaseAddr $2256

.bank $0F slot 3
.org $17EB
.section "new menu labels 1" overwrite
  ; set base PPU address
  ; (write to 0590 transfer buffer and to 0017-0018)
  clc
  lda $003F
  adc #<menuPpuBaseAddr
  sta $0591,X
  sta $0017
  lda $0040
  adc #>menuPpuBaseAddr
  sta $0590,X
  sta $0018
  inx
  inx
  
  ; get pointer to content
  lda #<newMenus
  sta $0030
  lda #>newMenus
  sta $0031
  
  ;===============
  ; add (menuindex * 14) to base pointer
  ;===============
  
  ; multiply menuindex by 2
  asl $0020
  rol $0021
  
  ; subtract (index * 2) from base position
  sec
  lda $0030
  sbc $0020
  sta $0030
  lda $0031
  sbc $0021
  sta $0031
  
  ; multiply menuindex by 8 (so it's now the original value * 16)
  asl $0020
  rol $0021
  asl $0020
  rol $0021
  asl $0020
  rol $0021
  
  ; add to base position
  clc
  lda $0030
  adc $0020
  sta $0030
  lda $0031
  adc $0021
  sta $0031
  
  jmp copyMenuChars
  
  
  
.ends

.bank $0F slot 3
.section "new menu labels 2" free
  copyMenuChars:
    ;===============
    ; switch bank and copy in characters
    ;===============
    
    lda currentBank
    pha
      ; load menu bank
      txa
      pha
        ; this scratch mem holds X == menu index for the duration of
        ; the call, but is overwritten by writeMapperReg
        lda $0022
        pha
          ldx #$03
          lda #:newMenus
          jsr writeMapperReg
        pla
        sta $0022
      pla
      tax
      
      jsr doMenuCopy
      
    pla
    
    tay
    txa
    pha
      lda $0022
      pha
        tya
        ldx #$03
        jsr writeMapperReg
      pla
      sta $0022
    pla
    tax
    
    ; set up size, etc.
    jmp $D84D
    
.ends

; cursor pos
.bank $0F slot 3
.org $18AA
.section "new menu labels 3" overwrite
  adc #$90
.ends

;===============================================
; use new credits
;===============================================

.bank $00 slot 2
.section "new credits" free
  newCredits:
    .include "out/script/credits/credits.inc"
.ends

.bank $00 slot 2
.org $3C42
.section "new credits pointer" overwrite
  .dw newCredits
.ends

.bank $00 slot 2
.org $3B07
.section "force CHR page for credits" overwrite
  ; force CHR page 01 for all sections of credits
  lda #$01
  nop
.ends

;===============================================
; use new ending song lyrics
;===============================================

/*.bank $00 slot 2
.org $3BBB
.section "karaoke ppu initial low byte" overwrite
  lda #$62
.ends

.bank $00 slot 2
.section "karaoke en" free
  karaokeEn:
    .include "out/script/credits/karaoke_en.inc"
.ends

.bank $00 slot 2
.org $3B81
.section "use karaoke en" overwrite
  lda #<(karaokeEn - $16A)
  sta $0022
  lda #>(karaokeEn - $16A)
  sta $0023
  nop
  nop
.ends */

; new karaoke has 3 lines:
; line 1: english 1 -- 28 chars
; line 2: english 2 -- 12 chars
; line 3: romaji    -- 21 chars

.define karaokeLine1Len 28
.define karaokeLine2Len 12
.define karaokeLine3Len 21

.define karaokeLine1PpuAddr $2322
;.define karaokeLine2PpuAddr $2342
.define karaokeLine2PpuAddr $2344
;.define karaokeLine3PpuAddr $2362
.define karaokeLine3PpuAddr $2369

.bank $07 slot 2
.section "karaoke en" free
  karaokeEn:
    .include "out/script/credits/karaoke_en.inc"
.ends

.bank $0F slot 3
.org $154C
.section "karaoke en fix 1" overwrite
  ; default delay for "unscheduled" boxes (orig 60)
  lda #$50
.ends

.bank $00 slot 3
.org $3994
.section "karaoke en fix 2" overwrite
  ; delay after reika/shizuka's lines (orig A0)
  lda #$58
.ends

.bank $00 slot 3
.org $3999
.section "karaoke en fix 3" overwrite
  ; delay after erika's lines (orig A0)
  lda #$58
.ends

.bank $00 slot 2
.org $398B
.section "karaoke en fix 4" overwrite
  ; delay after each "friend" finishes dialogue (orig E0)
  lda #$8E
.ends

.bank $00 slot 2
.org $3B7F
.section "use karaoke en" overwrite
  jmp preKaraokeCopy
  
  ;================================
  ; Set up upper row transfer
  ;================================

;  ldx ppuQueueSize
;  lda $0590,X
;  sta $05A5,X
;  inx 
;  lda $0590,X
;  sec 
;  sbc #$20
;  sta $05A5,X
;  inx 
;  ldy #$02
  
.ends

.bank $0F slot 3
.section "karaoke call" free
  preKaraokeCopy:
    ; switch to target bank
  ;  lda $0022
  ;  pha
    lda currentBank
    pha
    
      lda $0020
      pha
      lda $0021
      pha
        lda #:karaokeEn
        ldx #$03
        jsr writeMapperReg
      pla
      sta $0021
      pla
      sta $0020
      
      jsr doKaraokeCopy
    
    ; retrieve old bank num
    pla
    ldx #$03
    jsr writeMapperReg
  ;  pla
  ;  sta $0022
    
    rts
.ends

.bank $07 slot 2
.section "karaoke linecopy" free

  doKaraokeCopy:
    ;================================
    ; Fetch table pointer
    ;================================
    
    ; these strings are intended to be read from a larger tilemap table
    ; in which the karaoke entries start at index 0xB5, hence the offset here
    lda #<(karaokeEn - $16A)
    sta $0022
    lda #>(karaokeEn - $16A)
    sta $0023
    
    ;================================
    ; Look up string pointer
    ;================================
      
    ; add (index * 2)
    clc 
    lda $0022
    adc $0020
    sta $0022
    lda $0023
    adc $0021
    sta $0023
    
    ; load string pointer to 0020
    ldy #$00
    lda ($0022),Y
    sta $0020
    iny 
    lda ($0022),Y
    sta $0021
    
    ;================================
    ; Determine target PPU address
    ;================================
    
    ; set ppuaddr (2348 or 2748)
    ; high byte of PPU addr
  ;  lda #>karaokeStartPpuAddr
  ;  sta ppuQueue,X
  ;  ; if low bit of $0777 unset, target alt nametable (we want whichever
  ;  ; one is currently off-screen)
  ;  lda $0777
  ;  and #$01
  ;  bne +
  ;    lda ppuQueue,X
  ;    clc 
  ;    adc #$04
  ;    sta ppuQueue,X
  ;  +:
  ;  ; low byte of PPU addr
  ;  inx 
  ;  lda #<karaokeStartPpuAddr
  ;  sta ppuQueue,X
  ;  inx 
    
    ; getpos
    ldy #$00
    ; current ppu transfer queue size/pos
    ldx ppuQueueSize
    
    ;================================
    ; Transfer line 1
    ;================================
    
    ; set high byte of PPU addr
    lda #>karaokeLine1PpuAddr
    sta scratch2Hi
    ; set low byte of PPU addr
    lda #<karaokeLine1PpuAddr
    sta scratch2Lo
    
    ; pad width
    lda #karaokeLine1Len
    sta scratch1Lo
    
    ; copy line 1
    jsr karaokeLineCopy
    
    ;================================
    ; Transfer line 2
    ;================================
    
    lda #>karaokeLine2PpuAddr
    sta scratch2Hi
    lda #<karaokeLine2PpuAddr
    sta scratch2Lo
    lda #karaokeLine2Len
    sta scratch1Lo
    jsr karaokeLineCopy
    
    ;================================
    ; Transfer line 3
    ;================================
    
    lda #>karaokeLine3PpuAddr
    sta scratch2Hi
    lda #<karaokeLine3PpuAddr
    sta scratch2Lo
    lda #karaokeLine3Len
    sta scratch1Lo
    jmp karaokeLineCopy

  ;===========================================
  ; X = ppu transfer queue size
  ; 0020 = src
  ; scratch1Lo = number of characters to pad to
  ; scratch2 = ppudst (lo-hi)
  ;
  ; returns Y as index of next character in
  ; string past terminator
  ;===========================================
  
  karaokeLineCopy:
    
    ;================================
    ; Adjust PPU addr to account for
    ; current target nametable
    ;================================
  
    ; if low bit of $0777 unset, target alt nametable (we want whichever
    ; one is currently off-screen)
    lda $0777
    and #$01
    bne +
      lda scratch2Hi
      clc 
      adc #$04
      sta scratch2Hi
    +:
    
    ;================================
    ; Write PPU addr to queue
    ;================================
  
    lda scratch2Hi
    sta ppuQueue,X
    inx 
    lda scratch2Lo
    sta ppuQueue,X
    inx 
    
    ;================================
    ; Copy string to PPU queue
    ;================================
    
    -:
      ; copy byte from src to dst
      lda ($0020),Y
      iny 
      sta ppuQueue,X
      inx 
      ; continue until FF
      cmp #$FF
      bne -
    
    ;================================
    ; Pad with spaces
    ;================================
    
    ; save index of terminator
    tya
    pha
    
      ; go back a byte in dst
      dex 
;      dey
      ldy #$00
      lda #$00
      ; pad with zeroes until length is 0x12 bytes
      -:
        sta ppuQueue,X
        inx 
        iny
        
        ; check if pad size reached
        cpy scratch1Lo
        bne -
        
      ; write terminator
      lda #$FF
      sta ppuQueue,X
      inx 
      
      ; save updated ppu queue size
      stx ppuQueueSize
    
    ; Y is index of next character in string
    pla
    tay
      
    rts

.ends

;===============================================
; use new tilemaps for intro, intermissions, etc.
;===============================================

  ;===============================================
  ; title screen
  ;===============================================
  
  .bank $00 slot 2
  .section "new title screen tilemaps" free
    titleLogoTilemap0:
      .incbin "out/maps_conv/title0.bin"
    titleLogoTilemap1:
      .incbin "out/maps_conv/title1.bin"
    titleLogoTilemap2:
      .incbin "out/maps_conv/title2.bin"
    titleLogoTilemapBlank0:
;      .incbin "out/maps_conv/title_blank0.bin"
;    titleLogoTilemapBlank1:
;      .incbin "out/maps_conv/title_blank1.bin"
;    titleLogoTilemapBlank2:
;      .incbin "out/maps_conv/title_blank2.bin"
  .ends
  
  .bank $00 slot 2
  .org $006C
  .section "use new title screen tilemaps 0" overwrite
    .dw titleLogoTilemap0,titleLogoTilemap1,titleLogoTilemap2
  .ends
  
  .bank $00 slot 2
  .org $0080
  .section "use new title screen tilemaps 1" overwrite
    .dw titleLogoTilemap0,titleLogoTilemap1,titleLogoTilemap2
    .rept $7
      .dw titleLogoTilemap2
    .endr
  .ends
  
  .bank $00 slot 2
  .org $0098
  .section "use new title screen tilemaps 2" overwrite
    .dw titleLogoTilemap0,titleLogoTilemap1,titleLogoTilemap2
  .ends
  
  .bank $00 slot 2
  .org $00B0
  .section "use new title screen tilemaps 3" overwrite
    .dw titleLogoTilemap0,titleLogoTilemap1,titleLogoTilemap2
  .ends
  
/*  .bank $00 slot 2
  .org $00CA
  .section "use new title screen tilemaps blank 0" overwrite
    .dw titleLogoTilemapBlank0,titleLogoTilemapBlank1,titleLogoTilemapBlank2
  .ends
  
  .bank $00 slot 2
  .org $0156
  .section "use new title screen tilemaps blank 1" overwrite
    .dw titleLogoTilemapBlank0,titleLogoTilemapBlank1,titleLogoTilemapBlank2
  .ends */

  ;===============================================
  ; intro
  ;===============================================
  
  .bank $00 slot 2
  .section "new intro tilemaps" free
    .include "out/script/maps/intro.inc"
  .ends
  
  .bank $00 slot 2
  .org $0028
  .section "use new intro tilemaps" overwrite
    .dw introMap0
  .ends

  ;===============================================
  ; passwords
  ;===============================================
  
  .bank $00 slot 2
  .section "new password tilemaps" free
    .include "out/script/maps/password.inc"
  .ends
  
  .bank $00 slot 2
  .org $0060
  .section "use new password tilemaps 1" overwrite
    .dw passwordMap0
    .dw passwordMap1
    .dw passwordMap2
    .dw passwordMap3
  .ends

  ;===============================================
  ; chapter 1
  ;===============================================
  
  .bank $01 slot 2
  .section "new chapter 1 tilemaps" free
    .include "out/script/maps/chapter1.inc"
  .ends
  
  .bank $01 slot 2
  .org $00C4
  .section "use new chapter 1 tilemaps 0" overwrite
    .dw chapter1Map0
  .ends
  
  .bank $01 slot 2
  .org $00BE
  .section "use new chapter 1 tilemaps 1" overwrite
    .dw chapter1Map1
  .ends
  
  .bank $01 slot 2
  .org $00C0
  .section "use new chapter 1 tilemaps 2" overwrite
    .dw chapter1Map2
  .ends
  
  .bank $01 slot 2
  .org $00C2
  .section "use new chapter 1 tilemaps 3" overwrite
    .dw chapter1Map3
  .ends

  ;===============================================
  ; chapter 2
  ;===============================================
  
  .bank $02 slot 2
  .section "new chapter 2 tilemaps" free
    .include "out/script/maps/chapter2.inc"
  .ends
  
  .bank $02 slot 2
  .org $00B8
  .section "use new chapter 2 tilemaps 0" overwrite
    .dw chapter2Map0,chapter2Map0,chapter2Map0
  .ends
  
  .bank $02 slot 2
  .org $00E8
  .section "use new chapter 2 tilemaps 1" overwrite
    .dw chapter2Map0
  .ends
  
  .bank $02 slot 2
  .org $00D0
  .section "use new chapter 2 tilemaps 2" overwrite
    .dw chapter2Map1,chapter2Map2,chapter2Map3
  .ends

  ;===============================================
  ; chapter 3
  ;===============================================
  
  .bank $03 slot 2
  .section "new chapter 3 tilemaps" free
    .include "out/script/maps/chapter3.inc"
  .ends
  
  .bank $03 slot 2
  .org $00D8
  .section "use new chapter 3 tilemaps 0" overwrite
    .dw chapter3Map1,chapter3Map2,chapter3Map3
  .ends
  
  .bank $03 slot 2
  .org $00EC
  .section "use new chapter 3 tilemaps 1" overwrite
    .dw chapter3Map0
  .ends

  ;===============================================
  ; chapter 4
  ;===============================================
  
  .bank $04 slot 2
  .section "new chapter 4 tilemaps" free
    .include "out/script/maps/chapter4.inc"
  .ends
  
  .bank $04 slot 2
  .org $00B8
  .section "use new chapter 4 tilemaps 0" overwrite
    .dw chapter4Map1,chapter4Map2,chapter4Map3
  .ends
  
  .bank $04 slot 2
  .org $00C0
  .section "use new chapter 4 tilemaps 1" overwrite
    .dw chapter4Map4,chapter4Map5,chapter4Map6
  .ends
  
  .bank $04 slot 2
  .org $00D6
  .section "use new chapter 4 tilemaps 2" overwrite
    .dw chapter4Map0
  .ends

  ;===============================================
  ; chapter 5
  ;===============================================
  
  .bank $05 slot 2
  .section "new chapter 5 tilemaps" free
    .include "out/script/maps/chapter5.inc"
  .ends
  
;  .bank $05 slot 2
;  .org $00D0
;;  .org ((newAct5GrpTbl # $4000) + $A2)
;  .section "use new chapter 5 tilemaps 0" overwrite
;    .dw chapter5Map0
;  .ends
  
    ; MOVE MASTER MAP TABLE DUE TO STUPID PROGRAMMERS
    .bank $05 slot 2
    .section "new chapter 5 tilemap pointer table" free
      newAct5GrpTbl:
        .dw $817f
        .dw $8353
        .dw $8382
        .dw $83b1
        .dw $819c
        .dw $83ef
        .dw $841e
        .dw $844d
        .dw $81b9
        .dw $848b
        .dw $84ba
        .dw $84e9
        .dw $81d6
        .dw $8527
        .dw $8556
        .dw $8585
        .dw $81f3
        .dw $85c3
        .dw $85f2
        .dw $8621
        .dw $8210
        .dw $865f
        .dw $868e
        .dw $86bd
        .dw $822d
        .dw $865f
        .dw $86fb
        .dw $872a
        .dw $824a
        .dw $8768
        .dw $8797
        .dw $87c6
        .dw $8267
        .dw $8804
        .dw $8842
        .dw $8862
        .dw $8284
        .dw $8804
        .dw $88a0
        .dw $88c0
        .dw $82a1
        .dw $88fe
        .dw $892d
        .dw $895c
        .dw $82be
        .dw $899a
        .dw $89c9
        .dw $89f8
        .dw $82db
        .dw $8a36
        .dw $8a65
        .dw $8a94
        .dw $82f8
        .dw $8ad2
        .dw $8b01
        .dw $8b30
        .dw $8315
        .dw $8b6e
        .dw $8b9d
        .dw $8bcc
        .dw $8332
        .dw $8c0a
        .dw $8c39
        .dw $8c68
        .dw $8267
        .dw $8ca6
        .dw $8cd5
        .dw $8d04
        .dw $81b9
        .dw $8d42
        .dw $8d71
        .dw $8da0
        .dw $80d4
        .dw $80e7
        .dw $80fa
        .dw $810d
        .dw $8120
        .dw $8133
        .dw $8146
        .dw $8159
        .dw $816c
        ; NEW ACT INTRO MAP
        .dw chapter5Map0
        .dw $80d4
        ; NEW POSE ATTRIBUTE MAP (THE REASON FOR THIS ENTIRE MESS)
        .dw newAttrPoseMap
    .ends
    
    .bank $05 slot 2
    .org $0002
    .section "new chapter 5 tilemap pointer table pointer" overwrite
      .dw newAct5GrpTbl
    .ends

;===============================================
; fix attribute glitches on act 5 "pose" scene
;
; the scene incorrectly uses the attribute
; map from the previous scene.
; the programmer apparently forgot to include
; the correct table entirely, which makes
; things more difficult.
;===============================================

.define act5NewMapSentinel $53

.bank $05 slot 2
.org $3A55
.section "use new pose attr map value" overwrite
  .db act5NewMapSentinel
.ends

.bank $05 slot 2
.section "new pos attr map" free
  newAttrPoseMap:
    .incbin "out/script/pose_attr_right.bin"
.ends

.bank $0F slot 3
.org $0F32
.section "fix stupid shit 1" overwrite
  jmp act5FixCheck
.ends

.bank $0F slot 3
.section "fix stupid shit 2" free
  act5FixCheck:
    lda currentBank
    cmp #$05
    bne @done
    
    lda $0069
    cmp #(act5NewMapSentinel + 1)
    bne @done
    
      lda #$09
      sta $0069
    
    @done:
      jmp $D0D8
.ends

;.bank $0F slot 3
;.org $12B5
;.section "use new attr index lookup logic" overwrite
;  jsr newAttrLookupLogic
;.ends
;
;.bank $0F slot 3
;.section "new attr index lookup logic" free
;  newAttrLookupLogic:
;    jsr $D2E7
;    cmp #$FE
;    
;    rts
;.ends

;===============================================
; add translation credits to staff roll
;===============================================

.define newCreditsSize $2C

.bank $00 slot 2
.org $3AF1
.section "translation credits 1" overwrite
  cmp #newCreditsSize
.ends

.bank $00 slot 2
.org $3AF5
.section "translation credits 2" overwrite
  lda #newCreditsSize
.ends



