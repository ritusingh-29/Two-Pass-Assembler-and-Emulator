ldc 5      ; Load constant 5
    stl 0      ; Store in local variable
    ldc 3      ; Load constant 3
    stl 1      ; Store in local variable
    ldl 0      ; Load first number
    ldl 1      ; Load second number
    add        ; Add numbers
    stl 2      ; Store result
    HALT       ; End program