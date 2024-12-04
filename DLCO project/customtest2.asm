label:  add wrong  ; Error: Unexpected operand
label:  ldc        ; Error: Missing operand
bogus   10         ; Error: Bogus mnemonic
:error  ldc 5      ; Error: Invalid label
        br nolab   ; Error: Undefined label