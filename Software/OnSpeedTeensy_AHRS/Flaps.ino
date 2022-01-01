int getFlapsIndex()
{
int pos=analogRead(FLAP_PIN);
// set it to flap zero if there are no multiple positions available
if (flapPotPositions.Count<=1)  return 0;

int i=0; 
int betweenZone=0;
bool reverseOrder=false;

if (flapPotPositions.Items[0]>flapPotPositions.Items[flapPotPositions.Count-1]) reverseOrder=true;

for (i = 1; i < flapPotPositions.Count; i++)
    {  
    betweenZone=(flapPotPositions.Items[i]+flapPotPositions.Items[i-1])/2;
    if (!reverseOrder) 
        {                     
        if (pos<=betweenZone)
                            {                            
                            return (i-1);        
                            }
        } else 
          {
          // reverse order
          if (pos>=betweenZone)
                              {
                              return (i-1);
                              }
          }
    }
    return (i-1);
} 
