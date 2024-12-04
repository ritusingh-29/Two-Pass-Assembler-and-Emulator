; Addition of 25 and 15 and 10
a:       data 025    
b:       data 15    
c:       data 10  
d:       data 0  

start:  ldc 0      
        ldnl 0     
        stl 0      
        ldc 1      
        ldnl 0     
        stl 1
        ldc 2
        ldnl 0
        stl 2     

add:    ldl 0      
        ldl 1
        add
        ldl 2     
        add        
        stl 3      

done:   HALT       
