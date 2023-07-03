; DRAW_DEBUG 
; Draw some specific patterns 

SPRITE:
    DW 0x0000

DRAW_ONE_PIXEL:
    LD Va, 0x1
    LD Vb, 0x1
    LD I, SPRITE
    DRW Va, Vb, 0x1
    JP DRAW_ONE_PIXEL

;
;DRAW_BLOCK_X:
;    ADD Va, 0x1
;    LD I, SPRITE     ; TODO : I think this should be u8 rather than u16....
;    DRW Va, Vb, 0x2
;    SE Va, V1       ; skip once we've done <V1> lines
;    JP DRAW_BLOCK_X
