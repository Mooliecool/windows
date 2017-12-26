### R code from vignette source 'cs-limited-memory.rnw'

###################################################
### code chunk number 1: cs-limited-memory.rnw:6-9
###################################################
# free memory
rm(list = ls())
gc()


###################################################
### code chunk number 2: cs-limited-memory.rnw:48-49 (eval = FALSE)
###################################################
## cup98 <- read.csv("./data/KDDCup1998/cup98LRN.txt")


###################################################
### code chunk number 3: cs-limited-memory.rnw:52-53
###################################################
load("./data/cup98.rdata")


###################################################
### code chunk number 4: cs-limited-memory.rnw:56-85
###################################################
dim(cup98)
n.missing <- rowSums(is.na(cup98))
sum(n.missing>0)

varSet <- c(
   # demographics
   "ODATEDW", "OSOURCE", "STATE", "ZIP", "PVASTATE", "DOB", "RECINHSE", 
   "MDMAUD", "DOMAIN", "CLUSTER", "AGE", "HOMEOWNR", "CHILD03", "CHILD07", 
   "CHILD12", "CHILD18", "NUMCHLD", "INCOME", "GENDER", "WEALTH1", "HIT", 
   # donor interests
   "COLLECT1", "VETERANS", "BIBLE", "CATLG", "HOMEE", "PETS", "CDPLAY", 
   "STEREO", "PCOWNERS", "PHOTO", "CRAFTS", "FISHER", "GARDENIN", "BOATS", 
   "WALKER", "KIDSTUFF", "CARDS", "PLATES",
   # PEP star RFA status
   "PEPSTRFL",
   # summary variables of promotion history
   "CARDPROM", "MAXADATE", "NUMPROM", "CARDPM12", "NUMPRM12",
   # summary variables of giving history
   "RAMNTALL", "NGIFTALL", "CARDGIFT", "MINRAMNT", "MAXRAMNT", "LASTGIFT", 
   "LASTDATE", "FISTDATE", "TIMELAG", "AVGGIFT",
   # ID & targets
   "CONTROLN", "TARGET_B", "TARGET_D", "HPHONE_D",
   # RFA (Recency/Frequency/Donation Amount)
   "RFA_2F", "RFA_2A", "MDMAUD_R", "MDMAUD_F", "MDMAUD_A",
   #others
   "CLUSTER2", "GEOCODE2")
# remove ID & TARGET_D
vars <- setdiff(varSet, c("CONTROLN", "TARGET_D"))
cup98 <- cup98[,vars]


###################################################
### code chunk number 5: cs-limited-memory.rnw:97-99 (eval = FALSE)
###################################################
## library(randomForest)
## rf <- randomForest(TARGET_B ~ ., data=cup98)


###################################################
### code chunk number 6: cs-limited-memory.rnw:104-113
###################################################
# check missing values
n.missing <- rowSums(is.na(cup98))
(tab.missing <- table(n.missing))
# percentage of records without missing values
round(tab.missing["0"] / nrow(cup98), digits=2)
# check levels of categorical variables
idx.cat <- which(sapply(cup98, is.factor))
all.levels <- sapply(names(idx.cat), function(x) nlevels(cup98[,x]))
all.levels[all.levels > 10]


###################################################
### code chunk number 7: cs-limited-memory.rnw:119-125
###################################################
trainPercentage <- 80
testPercentage <- 20
ind <- sample(2, nrow(cup98), replace=TRUE, 
              prob=c(trainPercentage, testPercentage))
trainData <- cup98[ind==1,]
testData <- cup98[ind==2,]


###################################################
### code chunk number 8: cs-limited-memory.rnw:130-141 (eval = FALSE)
###################################################
## # cforest
## library(party)
## (time1 <- Sys.time())
## cf <- cforest(TARGET_B ~ ., data=trainData,
##               control = cforest_unbiased(mtry=2, ntree=50))
## (time2 <- Sys.time())
## time2 - time1 
## print(object.size(cf), units = "Mb")
## myPrediction <- predict(cf, newdata=testData)
## (time3 <- Sys.time())
## time3 - time2


###################################################
### code chunk number 9: cs-limited-memory.rnw:150-152
###################################################
memory.limit(4095)
library(party)


###################################################
### code chunk number 10: cs-limited-memory.rnw:155-156 (eval = FALSE)
###################################################
## ct <- ctree(TARGET_B ~ ., data=trainData)


###################################################
### code chunk number 11: cs-limited-memory.rnw:179-190
###################################################
library(party) # for ctree
trainPercentage <- 30
testPercentage <- 20
restPrecentage <- 100 - trainPercentage - testPercentage
fileName <- paste("cup98-ctree", trainPercentage, testPercentage, sep="-")
vars <- setdiff(varSet, c("TARGET_D", "CONTROLN", "ZIP", "OSOURCE"))
# partition the data into training and test datasets
ind <- sample(3, nrow(cup98), replace=T, 
              prob=c(trainPercentage, testPercentage, restPrecentage))
trainData <- cup98[ind==1, vars]
testData <- cup98[ind==2, vars]


###################################################
### code chunk number 12: cs-limited-memory.rnw:194-203
###################################################
# check the percentage of classes
round(prop.table(table(cup98$TARGET_B)), digits=3)
round(prop.table(table(trainData$TARGET_B)), digits=3)
round(prop.table(table(testData$TARGET_B)), digits=3)

# remove raw data to save memory
rm(cup98, ind)
gc()
memory.size()


###################################################
### code chunk number 13: cs-limited-memory.rnw:208-223
###################################################
# build ctree
myCtree <- NULL 
startTime <- Sys.time()
myCtree <- ctree(TARGET_B ~ ., data = trainData)
Sys.time() - startTime
print(object.size(myCtree), units = "Mb")
#print(myCtree)
memory.size()

# plot the tree and save it in a .PDF file
pdf(paste(fileName, ".pdf", sep=""), width=12, height=9,
    paper="a4r", pointsize=6)
plot(myCtree, type="simple", ip_args=list(pval=F), ep_args=list(digits=0), 
     main=fileName)
graphics.off()


###################################################
### code chunk number 14: cs-limited-memory.rnw:233-235
###################################################
load("./data/cup98.rdata")
set.seed(235)


###################################################
### code chunk number 15: cs-limited-memory.rnw:238-255
###################################################
vars.selected <- c("CARDS", "CARDGIFT", "CARDPM12", "CHILD12", "CLUSTER2", 
                   "DOMAIN", "GENDER", "GEOCODE2", "HIT", "HOMEOWNR", 
                   "INCOME", "LASTDATE", "MINRAMNT", "NGIFTALL", "PEPSTRFL", 
                   "RECINHSE", "RFA_2A", "RFA_2F", "STATE", "WALKER")
trainPercentage <- 80
testPercentage <- 20
fileName <- paste("cup98-ctree", trainPercentage, testPercentage, sep="-")
vars <- c("TARGET_B", vars.selected)
# partition the data into training and test subsets
ind <- sample(2, nrow(cup98), replace=T, prob=c(trainPercentage, testPercentage))
trainData <- cup98[ind==1, vars]
testData <- cup98[ind==2, vars]
# build a decision tree
myCtree <- ctree(TARGET_B ~ ., data = trainData)
print(object.size(myCtree), units = "Mb")
memory.size()
print(myCtree)


###################################################
### code chunk number 16: cs-limited-memory.rnw:260-268
###################################################
save(myCtree, file = paste(fileName, ".Rdata", sep=""))
pdf(paste(fileName, ".pdf", sep=""), width=12, height=9,
    paper="a4r", pointsize=6)
plot(myCtree, type="simple", ip_args=list(pval=F), ep_args=list(digits=0), 
     main=fileName)
plot(myCtree, terminal_panel=node_barplot(myCtree), ip_args=list(pval=F),
     ep_args=list(digits=0), main=fileName)
graphics.off()


###################################################
### code chunk number 17: cs-limited-memory.rnw:286-294
###################################################
rm(trainData)
myPrediction <- predict(myCtree, newdata=testData)
# check predicted results
testResult <- table(myPrediction, testData$TARGET_B)
percentageOfOne <- round(100 * testResult[,2] / (testResult[,1] + testResult[,2]), 
                         digits=1)
testResult <- cbind(testResult, percentageOfOne)
print(testResult)


###################################################
### code chunk number 18: cs-limited-memory.rnw:300-302
###################################################
boxplot(myPrediction ~ testData$TARGET_B, xlab="TARGET_B", ylab="Prediction",
        ylim=c(0,0.25))


###################################################
### code chunk number 19: cs-limited-memory.rnw:311-322
###################################################
s1 <- sort(myPrediction, decreasing=TRUE, method = "quick", index.return=TRUE)
testSize <- nrow(testData)
TotalNumOfTarget <- sum(testData$TARGET_B)
NumOfTarget <- rep(0, testSize)
NumOfTarget[1] <- (testData$TARGET_B)[s1$ix[1]]
for (i in 2:testSize) {  
  NumOfTarget[i] <- NumOfTarget[i-1] + testData$TARGET_B[s1$ix[i]]
}
plot(1:testSize, NumOfTarget, pty=".", type="l", lty="solid", col="red",
     ylab="Count Of Responses in Top k", xlab="Top k", main=fileName)
grid(col = "gray", lty = "dotted")


###################################################
### code chunk number 20: cs-limited-memory.rnw:331-337
###################################################
percentile <- 100 * (1:testSize) / testSize
percentileTarget <- 100 * NumOfTarget / TotalNumOfTarget
plot(percentile, percentileTarget, pty=".", type="l", lty="solid", col="red",
     ylab="Percentage of Predicted Donations (%)", xlab="Percentage of Pool", 
     main=fileName)
grid(col = "gray", lty = "dotted")        


###################################################
### code chunk number 21: cs-limited-memory.rnw:349-353 (eval = FALSE)
###################################################
## memory.limit(4095)
## # read scoring data and training data
## cup98val <- read.csv("./data/KDDCup1998/cup98VAL.txt")
## cup98 <- read.csv("./data/KDDCup1998/cup98LRN.txt")


###################################################
### code chunk number 22: cs-limited-memory.rnw:358-361
###################################################
memory.limit(4095)
load("./data/cup98val.rdata")
load("./data/cup98.rdata")


###################################################
### code chunk number 23: cs-limited-memory.rnw:364-367
###################################################
library(party) # for ctree
treeFileName <- "cup98-ctree-80-20"
splitNum <- 10


###################################################
### code chunk number 24: cs-limited-memory.rnw:373-396
###################################################
# check and set levels of categorical variables
trainData <- cup98[,vars]
vars2 <- setdiff(c(vars,"CONTROLN"), "TARGET_B")
scoreData <- cup98val[,vars2]
rm(cup98, cup98val)
trainNames <- names(trainData)
scoreNames <- names(scoreData)
#cat("\n checking and setting variable values \n")
newScoreData <- scoreData
variableList <- intersect(trainNames, scoreNames)
for (i in 1:length(variableList)) {
    varname <- variableList[i]
    trainLevels <- levels(trainData[,varname])
    scoreLevels <- levels(newScoreData[,varname])
    if (is.factor(trainData[,varname]) & setequal(trainLevels, scoreLevels)==F) {
        cat("Warning: new values found in score data, and they will be changed to NA!\n")
        cat(varname, "\n")
        cat("train: ", length(trainLevels), ", ", trainLevels, "\n")
        cat("score: ", length(scoreLevels), ", ", scoreLevels, "\n\n")
        newScoreData[,varname] <- factor(newScoreData[,varname],
                                         levels=trainLevels)
    } #endif
} 


###################################################
### code chunk number 25: cs-limited-memory.rnw:400-415
###################################################
# loading model
load(paste(treeFileName, ".Rdata", sep=""))

print(object.size(trainData), units = "Mb")
print(object.size(scoreData), units = "Mb")
print(object.size(newScoreData), units = "Mb")
print(object.size(myCtree), units = "Mb")
gc()
memory.size()
rm(trainNames, scoreNames)
rm(variableList)
rm(trainLevels, scoreLevels)
rm(trainData, scoreData)
gc()
memory.size()


###################################################
### code chunk number 26: cs-limited-memory.rnw:420-461
###################################################
nScore <- dim(newScoreData)[1]
(splitSize <- round(nScore/splitNum))
myPred <- NULL
for (i in 1:splitNum) {
    startPos <- 1 + (i-1)*splitSize
    if (i==splitNum) {
      endPos <- nScore
    }
    else {
      endPos <- i * splitSize
    }
    print(paste("Predicting:", startPos, "--", endPos))
    # make prediction
    tmpPred <- predict(myCtree, newdata = newScoreData[startPos:endPos, ])
    myPred <- c(myPred, tmpPred)
} 

# cumulative count and percentage
length(myPred)
rankedLevels <- table(round(myPred, digits=4))
# put highest rank first by reversing the vector
rankedLevels <- rankedLevels[length(rankedLevels):1]
levelNum <- length(rankedLevels)
cumCnt <- rep(0, levelNum)
cumCnt[1] <- rankedLevels[1]
for (i in 2:levelNum) {
    cumCnt[i] <- cumCnt[i-1] + rankedLevels[i]
} 
cumPercent <- 100 * cumCnt / nScore
cumPercent <- round(cumPercent, digits=1)
percent <- 100 * rankedLevels / nScore
percent <- round(percent, digits=1)
cumRanking <- data.frame(rankedLevels, cumCnt, percent, cumPercent)
names(cumRanking) <- c("Frequency", "CumFrequency", "Percentage",
                       "CumPercentage")
print(cumRanking)
write.csv(cumRanking, "cup98-cumulative-ranking.csv", row.names=T)
pdf(paste("cup98-score-distribution.pdf",sep=""))
plot(rankedLevels, x=names(rankedLevels), type="h", xlab="Score", 
     ylab="# of Customers")
graphics.off()


###################################################
### code chunk number 27: cs-limited-memory.rnw:480-487
###################################################
s1 <- sort(myPred, decreasing=TRUE, method = "quick", index.return=T)
varToOutput <- c("CONTROLN")
score <- round(myPred[s1$ix], digits=4)
table(score, useNA="ifany")
result <- data.frame(cbind(newScoreData[s1$ix, varToOutput]), score)
names(result) <- c(varToOutput, "score")
write.csv(result, "cup98-predicted-score.csv", row.names=F)


###################################################
### code chunk number 28: cs-limited-memory.rnw:492-497 (eval = FALSE)
###################################################
## # output as an EXCEL file
## library(RODBC)
## xlsFile <- odbcConnectExcel("cup98-predicted-score.xls", readOnly=F)
## sqlSave(xlsFile, result, rownames=F)
## odbcCloseAll() 


###################################################
### code chunk number 29: cs-limited-memory.rnw:510-564
###################################################
# functions for printing rules from ctree
# based on "Print.R" from package party
print.TerminalNode <- function(x, rule = NULL, ...) {
    n.rules <<- n.rules + 1
    node.ids <<- c(node.ids, x$nodeID)
    n.records <<- c(n.records, sum(x$weights))
    scores <<- c(scores, x$prediction)
    ruleset <<- c(ruleset, rule)
}

print.SplittingNode <- function(x, rule = NULL, ...) {
    if (!is.null(rule)) {
      rule <- paste(rule, "\n")
    }
    rule2 <- print(x$psplit, left = TRUE, rule=rule)
    print(x$left, rule=rule2)
    rule3 <- print(x$psplit, left = FALSE, rule=rule)
    print(x$right, rule=rule3)
}

print.orderedSplit <- function(x, left = TRUE, rule = NULL, ...) {
    if (!is.null(attr(x$splitpoint, "levels"))) {
        sp <- attr(x$splitpoint, "levels")[x$splitpoint]
    } else {
        sp <- x$splitpoint
    }
    n.pad <- 20 - nchar(x$variableName)
    pad <- paste(rep(" ", n.pad), collapse="")
    if (!is.null(x$toleft)) {
       left <- as.logical(x$toleft) == left
    }
    if (left) {
        rule2 <- paste(rule, x$variableName, pad, "<= ", sp, sep = "")
    } else {
        rule2 <- paste(rule, x$variableName, pad, "> ", sp, sep = "")
    }
    rule2
}

print.nominalSplit <- function(x, left = TRUE, rule = NULL, ...) {
    levels <- attr(x$splitpoint, "levels")
    ### is > 0 for levels available in this node
    tab <- x$table
    if (left) {
        lev <- levels[as.logical(x$splitpoint) & (tab > 0)]
    } else {
        lev <- levels[!as.logical(x$splitpoint) & (tab > 0)]
    }
    txt <- paste("'", paste(lev, collapse="', '"), "'", sep="")
    n.pad <- 20 - nchar(x$variableName)
    pad <- paste(rep(" ", n.pad), collapse="")
    rule2 <- paste(rule, x$variableName, pad, txt, sep = "")
    rule2
}


###################################################
### code chunk number 30: cs-limited-memory.rnw:577-588
###################################################
library(party) # for ctree
# loading model
load(paste(treeFileName, ".Rdata", sep=""))
# extract rules from tree
n.rules <- 0
node.ids <- NULL
n.records <- NULL
scores <- NULL
ruleset <- NULL
print(myCtree@tree)
n.rules


###################################################
### code chunk number 31: cs-limited-memory.rnw:592-597
###################################################
# sort by score descendingly
s1 <- sort(scores, decreasing=T, method = "quick", index.return=T)
percentage <- 100 * n.records[s1$ix] / sum(myCtree@weights)
cumPercentage <- round(cumsum(percentage), digits=1)
percentage <- round(percentage, digits=1)


###################################################
### code chunk number 32: cs-limited-memory.rnw:600-602
###################################################
# print first 5 rules only
n.rules <- 5


###################################################
### code chunk number 33: cs-limited-memory.rnw:605-614
###################################################
# print all rules
for (i in 1:n.rules) {
  cat("Rule", i, "\n")
  cat("Node:", node.ids[s1$ix[i]])
  cat(", score:", scores[s1$ix[i]])
  cat(", Percentage: ", percentage[i], "%", sep="")
  cat(", Cumulative Percentage: ",cumPercentage[i], "%\n", sep="")
  cat(ruleset[s1$ix[i]], "\n\n")
}


###################################################
### code chunk number 34: cs-limited-memory.rnw:622-668
###################################################
# functions for printing rules in SAS statement for scoring with a DATA step
# based on "Print.R" from package party
print.TerminalNode <- function(x, rule = NULL, ...) {
    rule <- sub(' +', '', rule) # remove leading spaces
    n.rules <<- n.rules + 1
    node.ids <<- c(node.ids, x$nodeID)
    n.records <<- c(n.records, sum(x$weights))
    scores <<- c(scores, x$prediction)
    ruleset <<- c(ruleset, rule)
}
print.SplittingNode <- function(x, rule = NULL, ...) {
    if (!is.null(rule)) {
      rule <- paste(rule, "\n    and")
    }#endif
    rule2 <- print(x$psplit, left = TRUE, rule=rule)
    print(x$left, rule=rule2)
    rule3 <- print(x$psplit, left = FALSE, rule=rule)
    print(x$right, rule=rule3)
}
print.orderedSplit <- function(x, left = TRUE, rule = NULL, ...) {
    if (!is.null(attr(x$splitpoint, "levels"))) {
        sp <- attr(x$splitpoint, "levels")[x$splitpoint]
    } else {
        sp <- x$splitpoint
    }
    if (!is.null(x$toleft)) left <- as.logical(x$toleft) == left
    if (left) {
        rule2 <- paste(rule, " ", x$variableName, "  <=  ", sp, sep = "")
    } else {
        rule2 <- paste(rule, " ", x$variableName, "  >  ", sp, sep = "")
    }
    rule2
}
print.nominalSplit <- function(x, left = TRUE, rule = NULL, ...) {
    levels <- attr(x$splitpoint, "levels")
    ### is > 0 for levels available in this node
    tab <- x$table
    if (left) {
        lev <- levels[as.logical(x$splitpoint) & (tab > 0)]
    } else {
        lev <- levels[!as.logical(x$splitpoint) & (tab > 0)]
    }
    txt <- paste("'", paste(lev, collapse="', '"), "'", sep="")
    rule2 <- paste(rule, " ", x$variableName, "  in  (", txt, ")", sep = "")
    rule2
}


###################################################
### code chunk number 35: cs-limited-memory.rnw:673-683
###################################################
library(party) # for ctree
# loading model
load(paste(treeFileName, ".Rdata", sep=""))
n.rules <- 0
node.ids <- NULL
n.records <- NULL
scores <- NULL
ruleset <- NULL
print(myCtree@tree)
n.rules


###################################################
### code chunk number 36: cs-limited-memory.rnw:688-693
###################################################
# sort by score descendingly
s1 <- sort(scores, decreasing=TRUE, method = "quick", index.return=TRUE)
percentage <- 100 * n.records[s1$ix] / sum(myCtree@weights)
cumPercentage <- round(cumsum(percentage), digits=1)
percentage <- round(percentage, digits=1)


###################################################
### code chunk number 37: cs-limited-memory.rnw:696-698
###################################################
# print first 5 rules only
n.rules <- 5


###################################################
### code chunk number 38: cs-limited-memory.rnw:701-716
###################################################
# print all rules
for (i in 1:n.rules) {
  cat("/* Rule", i, "\n")
  cat("   Node:", node.ids[s1$ix[i]])
  cat(", score:", scores[s1$ix[i]])
  cat(", Percentage: ", percentage[i], "%", sep="")
  cat(", Cumulative Percentage: ",cumPercentage[i], "% \n*/\n", sep="")
  if (i == 1) {
    cat("IF \n    ")
  } else {
      cat("ELSE IF \n    ")
  }
  cat(ruleset[s1$ix[i]], "\n")
  cat("THEN\n    score = ", scores[s1$ix[i]], ";\n\n", sep="") 
}


