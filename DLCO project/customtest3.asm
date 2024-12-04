; Binary search implementation
; Array is stored starting at memory location 50
; Array elements: [10, 20, 30, 40, 50, 60, 70, 80, 90, 100]

        ldc 50      ; load array start address
        stl 0       ; store it at local var 0
        ldc 10      ; load array length
        stl 1       ; store it at local var 1
        ldc 15      ; value to search for
        stl 2       ; store it at local var 2
        
        ; Initialize variables for binary search
        ldc 0       ; left = 0
        stl 3       ; store left at local var 3
        ldl 1       ; load array length
        ldc 1
        sub         ; length - 1 for right index
        stl 4       ; store right at local var 4

loop:   ldl 3       ; load left
        ldl 4       ; load right
        sub         ; right - left
        brlz found  ; if left > right, element not found
        
        ; Calculate mid = (left + right) / 2
        ldl 3       ; load left
        ldl 4       ; load right
        add         ; left + right
        ldc 2       ; prepare to divide by 2
        shr         ; (left + right) / 2
        stl 5       ; store mid at local var 5
        
        ; Load array[mid]
        ldl 0       ; load array base address
        ldl 5       ; load mid
        add         ; calculate array[mid] address
        ldnl 0      ; load array[mid] value
        
        ; Compare array[mid] with target
        ldl 2       ; load target value
        sub         ; array[mid] - target
        brz success ; if zero, element found
        brlz right  ; if negative, search right half
        
        ; Search left half
        ldl 5       ; load mid
        ldc 1
        sub         ; mid - 1
        stl 4       ; right = mid - 1
        br loop
        
right:  ldl 5       ; load mid
        ldc 1
        add         ; mid + 1
        stl 3       ; left = mid + 1
        br loop
        
success:ldl 5       ; load mid index (found position)
        stl 6       ; store result
        ldc 1       ; success flag
        stl 7
        br end
        
found:  ldc -1      ; not found
        stl 6       ; store result
        ldc 0       ; failure flag
        stl 7

end:    HALT

; Data section - sorted array
        data 10  ; array[0]
        data 20  ; array[1]
        data 30  ; array[2]
        data 40  ; array[3]
        data 50  ; array[4]
        data 60  ; array[5]
        data 70  ; array[6]
        data 80  ; array[7]
        data 90  ; array[8]
        data 100 ; array[9]