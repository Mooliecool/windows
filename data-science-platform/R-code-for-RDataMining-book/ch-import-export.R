### R code from vignette source 'ch-import-export.rnw'

###################################################
### code chunk number 1: ch-import-export.rnw:6-9
###################################################
# free memory
rm(list = ls())
gc()


###################################################
### code chunk number 2: ch-import-export.rnw:19-24
###################################################
a <- 1:10
save(a, file="./data/dumData.Rdata")
rm(a)
load("./data/dumData.Rdata")
print(a)


###################################################
### code chunk number 3: ch-import-export.rnw:32-40
###################################################
var1 <- 1:5
var2 <- (1:5) / 10
var3 <- c("R", "and", "Data Mining", "Examples", "Case Studies")
df1 <- data.frame(var1, var2, var3)
names(df1) <- c("VariableInt", "VariableReal", "VariableChar")
write.csv(df1, "./data/dummmyData.csv", row.names = FALSE)
df2 <- read.csv("./data/dummmyData.csv")
print(df2)


###################################################
### code chunk number 4: ch-import-export.rnw:71-80
###################################################
library(foreign) # for importing SAS data
# the path of SAS on your computer
sashome <- "C:/Program Files/SAS/SASFoundation/9.2" 
filepath <- "./data"
# filename should be no more than 8 characters, without extension
fileName <- "dumData" 
# read data from a SAS dataset
a <- read.ssd(file.path(filepath), fileName, sascmd=file.path(sashome, "sas.exe"))
print(a)


###################################################
### code chunk number 5: ch-import-export.rnw:85-90
###################################################
# read variable names from a .CSV file
variableFileName <- "dumVariables.csv"
myNames <- read.csv(paste(filepath, variableFileName, sep="/"))
names(a) <- names(myNames)
print(a)


###################################################
### code chunk number 6: ch-import-export.rnw:107-114 (eval = FALSE)
###################################################
## library(RODBC)
## connection <- odbcConnect(dsn="servername",uid="userid",pwd="******")
## query <- "SELECT * FROM lib.table WHERE ..."
## # or read query from file
## # query <- readChar("data/myQuery.sql", nchars=99999)
## myData <- sqlQuery(connection, query, errors=TRUE)
## odbcClose(connection)


###################################################
### code chunk number 7: ch-import-export.rnw:122-128 (eval = FALSE)
###################################################
## library(RODBC) 
## filename <- "data/dummmyData.xls"
## xlsFile <- odbcConnectExcel(filename, readOnly = FALSE)
## sqlSave(xlsFile, a, rownames = FALSE)
## b <- sqlFetch(xlsFile, "a")
## odbcClose(xlsFile) 


