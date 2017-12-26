module Favorites
{
    type Model
    {
        Id : Integer32 => AutoNumber();
        
        Field : Integer32;
        
    }
    
    Modelsamples : {Model*} where identity Id;
    
}
