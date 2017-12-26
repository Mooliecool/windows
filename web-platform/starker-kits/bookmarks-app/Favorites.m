module Favorites
{

    type Category
    {
        CategoryID : Integer32 => AutoNumber();
        CategoryName : Text;
                
    }
     where identity CategoryID;      
    
    type Site
    {
        SiteID : Integer32 => AutoNumber();
        Name : Text; 
        URL :Text;
        City : Text;
        State : Text;
        Country : Text;        
        Category : Category where value in Categories; 
       
    }
   
    where identity SiteID;
           
        Categories : {Category*};
        Sites : {Site*};
        
        Categories
        {
            Software
            {
                CategoryName=>"Software"                
            }
        }       
        
        Sites
        {
            Microsoft
            {            
                 Name=>"Steve", URL=>"36", City=>"Boston",State=>"WA",Country=>"USA",Category=>Categories.Software                
            },
             
            Apple
            {
                 Name=>"Steve", URL=>"36", City=>"Boston",State=>"VA",Country=>"USA",Category=>Categories.Software
            },
        }
} 


    
   
