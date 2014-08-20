
#Level generator
#By Kim Bratzel

#Defines the location of the pills and or walls for 3D pacman game.

import sys

rows = range(1,16)
cols = range(1,16)
level = 0


if (len(sys.argv) > 1):
    level = (int)(sys.argv[1])
    #print "Generating Level: " + (sys.argv[1])
else:
    print "Error: Not enough args."
    print "Usage: python level_gen.py (levelnum) > ./levels/levelN.txt"
    
#############################################    
#most simple level, all squares contain a pill    
if (level == 1):
        for i in rows:
            for j in cols:
                print "D",
            print ""
#############################################    
#cool outer square   
elif (level == 2):
        for i in rows:
            for j in cols:
                if (i > 12 or i < 4 or j > 12 or j < 4):
                    print "D",
                else:
                    print "-",    
            print ""     
            #print a new line 
#############################################    
#rows of pills    
elif (level == 3):
        for i in rows:
            for j in cols:
                if (j % 2 == 0):
                    print "D",
                else:
                    print "-",    
            print ""     
            #print a new line   
#############################################    
#Checkerboard style   
elif (level == 4):
        for i in rows:
            for j in cols:
                if ((j % 2 == 0 and i % 2 == 1) or (i % 2 == 0 and j % 2 == 1)):
                    print "D",
                else:
                    print "-",    
            print ""
            #print a new line    
#############################################    
#nicely spaced
elif (level == 5):
        for i in rows:
            for j in cols:
                if ((j % 2 == 0 and i % 2 == 0)):
                    print "D",
                else:
                    print "-",
            print ""     
            #print a new line 
#############################################    
#triqngle
elif (level == 6):
        for i in rows:
            for j in cols:
                if (j < i):
                    print "D",
                else:
                    print "-",
            print ""
            #print a new line 
#############################################    
#triqngle
elif (level == 7):
        for i in rows:
            for j in cols:
                if (j & i > 5):
                    print "D",
                else:
                    print "-",
            print ""
            #print a new line                                                      
#############################################            
#Fake level, everything blank  
elif (level == 99):
        for i in rows:
            for j in cols:
                print "-",
            print ""                    
#############################################
#DEFAULT BEHAVIOUR
elif (level == 0):
    print "No Level Loaded - Does not exist" 
else:
    print "No Level Loaded - Does not exist"     