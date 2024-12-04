; Calculate GCD of 48 and 36
        data 078    ; Store 48 at memory location 0
        data 36    ; Store 36 at memory location 1
        data 0     ; Reserve space for result at location 2

start:  ldc 0      ; Load address 0
        ldnl 0     ; Load first number (48)
        stl 0      ; Store in stack location 0
        ldc 1      ; Load address 1
        ldnl 0     ; Load second number (36)
        stl 1      ; Store in stack location 1

loop:   ldl 1      ; Load B
        brz done   ; If B == 0, we're done
        ldl 0      ; Load A
        ldl 1      ; Load B again
        sub        ; A = A - B
        brlz swap  ; If A < B, swap them
        stl 0      ; Store new A
        br loop    ; Continue loop

swap:   ldl 1      ; Load B
        stl 2      ; Store B temporarily
        ldl 0      ; Load A
        stl 1      ; Store A as new B
        ldl 2      ; Load old B
        stl 0      ; Store as new A
        br loop    ; Continue loop

done:   ldl 0      ; Load result
        stl 2      ; Store result in memory location 2
        HALT       ; Stop program