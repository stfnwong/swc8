; Test program to draw a block of pixels 

; height of block
LD V1, 0x0F         ; max height to draw
LD V2, 0x00         ; max width to draw

; Start position of block
LD Va, 0x00
LD Vb, 0x00

; Draw horizontal component of block
DRAW_BLOCK_X:
    ADD Va, 0x1
    LD I, Block     ; load block sprite
    DRW Va, Vb, 0x4
    SE Va, V1       ; skip once we've done <V1> lines
    JP DRAW_BLOCK_X


; Draw vertical component of block
DRAW_BLOCK_Y:
    ADD Vb, 0x1
    LD I, Block     ; load block sprite
    DRW Va, Vb, 0x4
    SE Vb, V1       ; skip once we've done <V1> lines
    JP DRAW_BLOCK_Y

RET

Block:
    DW 0x0000       ; data to draw...
    ;DW 0x0001       ; data to draw...
