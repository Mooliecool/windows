USE Favorites 
CREATE TABLE Category
(
	FavoriteID		int	PRIMARY KEY,
	CategoryName		varchar(30),
	CatID		int	FOREIGN KEY REFERENCES Category(FavoriteID)
	
)
GO
