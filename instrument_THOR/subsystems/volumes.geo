////////////////VOLUMES////////////////
////////////////SENSITIVE_VOLUMES////////////////

Volume PIXCDTE								
PIXCDTE.Material CdTe								
PIXCDTE.Visibility 1								
PIXCDTE.Color 2									
PIXCDTE.Shape BOX 0.704 0.704 0.1 	

////////////////LADDER_VOLUMES////////////////

Volume FINGERCDTE
FINGERCDTE.Material Vacuum
FINGERCDTE.Color 1
FINGERCDTE.Visibility 1
FINGERCDTE.Shape BOX 0.7 2.25 0.1

////////////////MOTHER_VOLUMES////////////////

PIXCDTE.Position 0.0 0.0 0.0
PIXCDTE.Mother FINGERCDTE

////////////////DUMMY_VOLUMES////////////////

#Volume CIRC1								
#CIRC1.Material CircuitBoard						
#CIRC1.Visibility 1								
#CIRC1.Color 3									
#CIRC1.Shape BOX 0.8 1.1 0.1 	
#CIRC1.Position 0.0 0.8 0.1
#CIRC1.Mother FINGERCDTE
