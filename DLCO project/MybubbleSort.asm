; Name : Ritu Kumari Singh
; Roll No. 2301CS75
;Bubble Sort


; Array size is configurable via 'length' variable
; Array elements are stored starting at 'array' label

        br main            ; Branch to main program, skipping data section

; Data section
length: data 8            ; Length of array
array:  data 5           ; Sample array with 8 elements
        data 12           ; These values will be sorted
        data 22           ; in ascending order
        data 37
        data 46
        data 60
        data 65
        data 80

main:   ldc 0             ; Initialize outer loop counter (i = 0)
        stl 0             ; Store i in local variable 0

outer:  ldl 0             ; Load i
        ldc length        ; Load address of length
        ldnl 0            ; Get value of length
        sub               ; length - i
        ldc 1
        sub               ; (length - i - 1)
        brlz done         ; If negative, array is sorted, jump to done

        ldc 0             ; Initialize inner loop counter (j = 0)
        stl 1             ; Store j in local variable 1

inner:  ldl 1             ; Load j
        ldl 0             ; Load i
        ldc length
        ldnl 0            ; Get length
        sub               ; length - i
        ldc 1
        sub               ; (length - i - 1)
        sub               ; j - (length - i - 1)
        brlz compare      ; If j < length-i-1, continue with comparison

        ; Inner loop complete, increment outer loop counter
        ldl 0             ; Load i
        ldc 1
        add               ; i = i + 1
        stl 0             ; Store new i
        br outer          ; Go to next iteration of outer loop

compare:
        ; Compare array[j] and array[j+1]
        ldc array         ; Load base address of array
        ldl 1             ; Load j
        add               ; Calculate address of array[j]
        ldnl 0            ; Get value of array[j]
        stl 2             ; Store array[j] in temp variable

        ldc array         ; Load base address again
        ldl 1             ; Load j
        ldc 1
        add               ; j + 1
        add               ; Calculate address of array[j+1]
        ldnl 0            ; Get value of array[j+1]

        ldl 2             ; Load array[j]
        sub               ; array[j+1] - array[j]
        brlz swap         ; If array[j+1] < array[j], swap them

        ; No swap needed, move to next pair
next:   ldl 1             ; Load j
        ldc 1
        add               ; j = j + 1
        stl 1             ; Store new j
        br inner          ; Continue inner loop

swap:   ; Swap array[j] and array[j+1]
        ldc array         ; Load array base address
        ldl 1             ; Load j
        add               ; Address of array[j]
        ldnl 0            ; Get array[j]
        stl 3             ; Store array[j] temporarily

        ldc array         ; Load array base address
        ldl 1             ; Load j
        add               ; Address of array[j]
        ldl 1             ; Load j
        ldc 1
        add               ; j + 1
        ldc array
        add               ; Address of array[j+1]
        ldnl 0            ; Get array[j+1]
        stnl 0            ; Store in array[j]

        ldc array         ; Load array base address
        ldl 1             ; Load j
        ldc 1
        add               ; j + 1
        add               ; Address of array[j+1]
        ldl 3             ; Load saved array[j]
        stnl 0            ; Store in array[j+1]
        br next           ; Continue to next pair

done:   HALT              ; End program

; Local variable usage:
; 0: i (outer loop counter)
; 1: j (inner loop counter)
; 2: Temporary storage for array[j]
; 3: Temporary storage for swap operation