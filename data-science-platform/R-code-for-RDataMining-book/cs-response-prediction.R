### R code from vignette source 'cs-response-prediction.rnw'
### Encoding: ISO8859-1

###################################################
### code chunk number 1: cs-response-prediction.rnw:9-12
###################################################
# free memory
rm(list = ls())
gc()


###################################################
### code chunk number 2: cs-response-prediction.rnw:45-46 (eval = FALSE)
###################################################
## cup98 <- read.csv("./data/KDDCup1998/cup98LRN.txt")


###################################################
### code chunk number 3: cs-response-prediction.rnw:51-52
###################################################
load("./data/cup98.rdata")


###################################################
### code chunk number 4: cs-response-prediction.rnw:55-61
###################################################
dim(cup98)
# have a look at the first 30 variables
str(cup98[,1:30])
head(cup98[,1:30])
# a summary of the first 10 variables
summary(cup98[,1:10])


###################################################
### code chunk number 5: cs-response-prediction.rnw:66-80 (eval = FALSE)
###################################################
## library(Hmisc)
## describe(cup98[,1:28]) # demographics
## describe(cup98[,29:42]) # number of times responded to other types of mail order offers
## describe(cup98[,43:55]) # overlay data
## describe(cup98[,56:74]) # donor interests
## describe(cup98[,75]) # PEP star RFA status
## describe(cup98[,76:361]) # characteristics of the donors neighborhood
## describe(cup98[,362:407]) # promotion history
## describe(cup98[,408:412]) # summary variables of promotion history
## describe(cup98[,413:456]) # giving history
## describe(cup98[,457:469]) # summary variables of giving history
## describe(cup98[,470:473]) # ID & targets
## describe(cup98[,474:479]) # RFA (Recency/Frequency/Donation Amount)
## describe(cup98[,480:481]) # CLUSTER & GEOCODE


###################################################
### code chunk number 6: cs-response-prediction.rnw:87-91
###################################################
(response.percentage <- round(100 * prop.table(table(cup98$TARGET_B)), digits=1))
mylabels <- paste("TARGET_B=", names(response.percentage), "\n", 
                  response.percentage, "%", sep="")
pie(response.percentage, labels=mylabels)


###################################################
### code chunk number 7: cs-response-prediction.rnw:101-106
###################################################
# data with positive donations
cup98pos <- cup98[cup98$TARGET_D>0, ]
targetPos <- cup98pos$TARGET_D
summary(targetPos)
boxplot(targetPos)


###################################################
### code chunk number 8: cs-response-prediction.rnw:123-129
###################################################
# number of positive donations
length(targetPos)
# number of positive donations not in whole dollars
sum(!(targetPos %in% 1:200))
targetPos <- round(targetPos)
barplot(table(targetPos), las=2)


###################################################
### code chunk number 9: cs-response-prediction.rnw:138-143
###################################################
cup98$TARGET_D2 <- cut(cup98$TARGET_D, right=F,
        breaks=c(0, 0.1, 10, 15, 20, 25, 30, 50, max(cup98$TARGET_D)))
table(cup98$TARGET_D2)
cup98pos$TARGET_D2 <- cut(cup98pos$TARGET_D, right=F,
        breaks=c(0, 0.1, 10, 15, 20, 25, 30, 50, max(cup98pos$TARGET_D)))


###################################################
### code chunk number 10: cs-response-prediction.rnw:151-153
###################################################
table(cup98$RFA_2R)
round(100 * prop.table(table(cup98$NOEXCH)), digits=3)


###################################################
### code chunk number 11: cs-response-prediction.rnw:210-233
###################################################
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
   "CONTROLN", "TARGET_B", "TARGET_D", "TARGET_D2", "HPHONE_D",
   # RFA (Recency/Frequency/Donation Amount)
   "RFA_2F", "RFA_2A", "MDMAUD_R", "MDMAUD_F", "MDMAUD_A",
   #others
   "CLUSTER2", "GEOCODE2")
cup98 <- cup98[, varSet]


###################################################
### code chunk number 12: cs-response-prediction.rnw:245-247
###################################################
# select numeric variables
idx.num <- which(sapply(cup98, is.numeric))


###################################################
### code chunk number 13: cs-response-prediction.rnw:254-263
###################################################
layout(matrix(c(1,2), 1, 2)) # 2 graphs per page 
# histograms of numeric variables
myHist <- function(x) {
   hist(cup98[,x], main=NULL, xlab=x)
}
sapply(names(idx.num[4:5]), myHist)
layout(matrix(1)) # change back to one graph per page
# run code below to generate histograms for all numeric variables
# sapply(names(idx.num), myHist)


###################################################
### code chunk number 14: cs-response-prediction.rnw:274-279
###################################################
layout(matrix(c(1,2),1,2)) # 2 graphs per page
boxplot(cup98$HIT)
cup98$HIT[cup98$HIT>200]
boxplot(cup98$HIT[cup98$HIT<200])
layout(matrix(1)) # change back to one graph per page


###################################################
### code chunk number 15: cs-response-prediction.rnw:290-292
###################################################
AGE2 <- cut(cup98pos$AGE, right=F, breaks=seq(0, 100, by=5))
boxplot(cup98pos$TARGET_D ~ AGE2, ylim=c(0,40), las=3)


###################################################
### code chunk number 16: cs-response-prediction.rnw:303-313
###################################################
attach(cup98pos)
layout(matrix(c(1,2),1,2)) # 2 graphs per page
boxplot(TARGET_D ~ GENDER, ylim=c(0,80))
# density plot
plot(density(TARGET_D[GENDER=="F"]), xlim=c(0,60), col=1, lty=1)
lines(density(TARGET_D[GENDER=="M"]), col=2, lty=2)
lines(density(TARGET_D[GENDER=="J"]), col=3, lty=3)
legend("topright", c("Female", "Male", "Joint account"), col=1:3, lty=1:3)
layout(matrix(1)) # change back to one graph per page
detach(cup98pos)


###################################################
### code chunk number 17: cs-response-prediction.rnw:322-327
###################################################
correlation <- cor(cup98$TARGET_D, cup98[,idx.num], use="pairwise.complete.obs")
correlation <- abs(correlation)
(correlation <- correlation[,order(correlation, decreasing=T)])
# save to a CSV file, with important variables at the top
write.csv(correlation, "absolute_correlation.csv")


###################################################
### code chunk number 18: cs-response-prediction.rnw:331-333 (eval = FALSE)
###################################################
## cor(cup98[,idx.num])
## pairs(cup98)


###################################################
### code chunk number 19: cs-response-prediction.rnw:339-345
###################################################
color <- ifelse(cup98$TARGET_D>0, "blue", "black")
pch <- ifelse(cup98$TARGET_D>0, "+", ".")
plot(jitter(cup98$AGE), jitter(cup98$HIT), pch=pch, col=color, cex=0.7, 
     ylim=c(0,70), xlab="AGE", ylab="HIT")
legend("topleft", c("TARGET_D>0", "TARGET_D=0"), col=c("blue", "black"), 
       pch=c("+", "."))


###################################################
### code chunk number 20: cs-response-prediction.rnw:359-369
###################################################
myChisqTest <- function(x) {
   t1 <- table(cup98pos[,x], cup98pos$TARGET_D2)
   plot(t1, main=x, las=1)
   print(x)
   print(chisq.test(t1))
}
myChisqTest("GENDER")
# run the code below to do chi-square test for all categorical variables
# idx.cat <- which(sapply(cup98pos, is.factor))
# sapply(names(idx.cat), myChisqTest)


###################################################
### code chunk number 21: cs-response-prediction.rnw:384-403
###################################################
nRec <- dim(cup98)[1]
trainSize <- round(nRec * 0.7)
testSize <- nRec - trainSize
# ctree parameters   
MinSplit <- 1000
MinBucket <- 400
MaxSurrogate <- 4
MaxDepth <- 10  
(strParameters <- paste(MinSplit, MinBucket, MaxSurrogate, MaxDepth, sep="-"))
LoopNum <- 9
# The cost for each contact is $0.68.
cost <- 0.68
varSet2 <- c("AGE", "AVGGIFT", "CARDGIFT", "CARDPM12", "CARDPROM", "CLUSTER2",
   "DOMAIN", "GENDER", "GEOCODE2", "HIT", "HOMEOWNR", "HPHONE_D", "INCOME",
   "LASTGIFT", "MAXRAMNT", "MDMAUD_F", "MDMAUD_R", "MINRAMNT", "NGIFTALL",
   "NUMPRM12", "PCOWNERS", "PEPSTRFL", "PETS", "RAMNTALL", "RECINHSE", 
   "RFA_2A", "RFA_2F", "STATE", "TIMELAG")
cup98 <- cup98[, c("TARGET_D", varSet2)]
library(party) # for ctree


###################################################
### code chunk number 22: cs-response-prediction.rnw:408-471 (eval = FALSE)
###################################################
## pdf(paste("evaluation-tree-", strParameters, ".pdf", sep=""), 
##     width=12, height=9, paper="a4r", pointsize=6)
## cat(date(), "\n")
## cat(" trainSize=", trainSize, ", testSize=", testSize, "\n")
## cat(" MinSplit=", MinSplit, ", MinBucket=", MinBucket, 
##     ", MaxSurrogate=", MaxSurrogate, ", MaxDepth=", MaxDepth, "\n\n")
## 
## # run for multiple times and get the average result   
## allTotalDonation <- matrix(0, nrow=testSize, ncol=LoopNum)
## allAvgDonation <- matrix(0, nrow=testSize, ncol=LoopNum)
## allDonationPercentile <- matrix(0, nrow=testSize, ncol=LoopNum)
## for (loopCnt in 1:LoopNum) {
##    cat(date(), ":  iteration = ", loopCnt, "\n")
##    
##    # split into training data and testing data
##    trainIdx <- sample(1:nRec, trainSize)
##    trainData <- cup98[trainIdx,]
##    testData <- cup98[-trainIdx,]
##    
##    # train a decision tree
##    myCtree <- ctree(TARGET_D ~ ., data=trainData,
##          controls=ctree_control(minsplit=MinSplit, minbucket=MinBucket,
##                                 maxsurrogate=MaxSurrogate, maxdepth=MaxDepth))
##    # size of ctree
##    print(object.size(myCtree), units="auto")
##    save(myCtree, file=paste("cup98-ctree-", strParameters, "-run-", 
##                             loopCnt, ".rdata", sep=""))
##       
##    figTitle <- paste("Tree", loopCnt)
##    plot(myCtree, main=figTitle, type="simple", ip_args=list(pval=FALSE), 
##         ep_args=list(digits=0,abbreviate=TRUE), tp_args=list(digits=2))
##    #print(myCtree)
##   
##    # test
##    pred <- predict(myCtree, newdata=testData)
##    plot(pred, testData$TARGET_D)
##    print(sum(testData$TARGET_D[pred > cost] - cost))
##    # quick sort is "unstable" for tie values, so it is used here to introduce 
##    # a bit random for tie values
##    s1 <- sort(pred, decreasing=TRUE, method = "quick", index.return=TRUE)
##    totalDonation <- cumsum(testData$TARGET_D[s1$ix]) # cumulative sum
##    avgDonation <- totalDonation / (1:testSize)
##    donationPercentile <- 100 * totalDonation / sum(testData$TARGET_D)
##    allTotalDonation[,loopCnt] <- totalDonation
##    allAvgDonation[,loopCnt] <- avgDonation
##    allDonationPercentile[,loopCnt] <- donationPercentile
##    plot(totalDonation, type="l")
##    grid()
## }
## graphics.off()
## cat(date(), ":  Loop completed.\n\n\n")
## 
## fnlTotalDonation <- rowMeans(allTotalDonation)
## fnlAvgDonation <- rowMeans(allAvgDonation)
## fnlDonationPercentile <- rowMeans(allDonationPercentile)
## 
## rm(trainData, testData, pred)
## 
## # save results into a CSV file
## results <- data.frame(cbind(allTotalDonation,fnlTotalDonation))
## names(results) <- c(paste("run",1:LoopNum), "Average")
## write.csv(results, paste("evaluation-TotalDonation-", strParameters, ".csv",
##                          sep=""))    


###################################################
### code chunk number 23: cs-response-prediction.rnw:498-510
###################################################
result <- read.csv("evaluation-TotalDonation-1000-400-4-10.csv")
head(result)
result[,2:12] <- result[,2:12] - cost * (1:testSize)
# to reduce size of the file to save this chart
idx.pos <- c(seq(1, nrow(result), by=10), nrow(result))
plot(result[idx.pos,12], type="l", lty=1, col=1, ylim=c(0,4500), 
     xlab="Number of Mails", ylab="Amount of Donations ($)")
for (fCnt in 1:LoopNum) {
   lines(result[idx.pos,fCnt+1], pty=".", type="l", lty=1+fCnt, col=1+fCnt)
}
legend("bottomright", col=1:(LoopNum+1), lty=1:(LoopNum+1), 
       legend=c("Average", paste("Run",1:LoopNum)))


###################################################
### code chunk number 24: cs-response-prediction.rnw:520-533
###################################################
donationPercentile <- sapply(2:12, function(i) 
      100 *  result[,i] / result[testSize,i])
percentile <- 100 * (1:testSize)/testSize
plot(percentile[idx.pos], donationPercentile[idx.pos,11], pty=".", type="l",
     lty=1, col=1, ylim=c(0,170), xlab="Contact Percentile (%)",
     ylab="Donation Percentile (%)")
grid(col = "gray", lty = "dotted")
for (fCnt in 1:LoopNum) {
   lines(percentile[idx.pos], donationPercentile[idx.pos,fCnt], pty=".",
         type="l", lty=1+fCnt, col=1+fCnt)
}
legend("bottomright", col=1:(LoopNum+1), lty=1:(LoopNum+1), 
       legend=c("Average", paste("Run",1:LoopNum)))


###################################################
### code chunk number 25: cs-response-prediction.rnw:567-582
###################################################
avgDonation <- sapply(2:12, function(i) result[,i] / (1:testSize))
yTitle = c("Total Donation Amount Percentile (%)",
           "Average Donation Amount per Contact ($)")
par(mar=c(5,4,4,5)+.1)
plot(percentile[idx.pos], donationPercentile[idx.pos,7], pty=".", type="l",
     lty="solid", col="red", ylab=yTitle[1], xlab="Contact Percentile (%)")
grid(col = "gray", lty = "dotted")
par(new=TRUE)
plot(percentile[idx.pos], avgDonation[idx.pos,7], type="l", lty="dashed",
     col="blue", xaxt="n", yaxt="n", xlab="", ylab="",
     ylim=c(0,max(avgDonation[,7])))
axis(4)     
mtext(yTitle[2], side=4, line=2)
legend("right", col=c("red","blue"), lty=c("solid","dashed"),
       legend=yTitle)


###################################################
### code chunk number 26: cs-response-prediction.rnw:604-625 (eval = FALSE)
###################################################
## # compare results got with different parameters
## parameters <- c("1000-400-4-5", "1000-400-4-6", "1000-400-4-8", "1000-400-4-10")
## #parameters <- c("1000-400-4-10", "700-200-4-10", "200-50-4-10")
## paraNum <- length(parameters)
## percentile <- 100 * (1:testSize)/testSize
## # 1st result
## results <- read.csv(paste("evaluation-TotalDonation-", parameters[1], ".csv",
##                           sep=""))
## avgResult <- results$Average - cost * (1:testSize)
## plot(percentile, avgResult, pty=1, type="l", lty=1, col=1, #ylim=c(0,4000),
##      ylab="Amount of Donation", xlab="Contact Percentile (%)", 
##      main="Parameters: MinSplit, MinBucket, MaxSurrogate, MaxDepth")
## grid(col = "gray", lty = "dotted")     
## # other results
## for (i in 2:paraNum) {
##    results <- read.csv(paste("evaluation-TotalDonation-", parameters[i], 
##                              ".csv", sep=""))
##    avgResult <- results$Average - cost * (1:testSize)
##    lines(percentile, avgResult, type="l", lty=i, col=i)
## }
## legend("bottomright", col=1:paraNum, lty=1:paraNum, legend=parameters)


###################################################
### code chunk number 27: cs-response-prediction.rnw:659-660 (eval = FALSE)
###################################################
## cup98val <- read.csv("./data/KDDCup1998/cup98VAL.txt")


###################################################
### code chunk number 28: cs-response-prediction.rnw:665-666
###################################################
load("./data/cup98val.rdata")


###################################################
### code chunk number 29: cs-response-prediction.rnw:669-692
###################################################
cup98val <- cup98val[, c("CONTROLN", varSet2)]
trainNames <- names(cup98)
scoreNames <- names(cup98val)
# check if any variables not in scoring data
idx <- which(trainNames %in% scoreNames)
print(trainNames[-idx])

# check and set levels in factors in scoring data
scoreData <- cup98val
vars <- intersect(trainNames, scoreNames)
for (i in 1:length(vars)) {
   varname <- vars[i]
   trainLevels <- levels(cup98[,varname])
   scoreLevels <- levels(scoreData[,varname])
   if (is.factor(cup98[,varname]) & setequal(trainLevels, scoreLevels)==F) {
      cat("Warning: new values found in score data, and they will be changed to NA!\n")
      cat(varname, "\n")
      #cat("train: ", length(trainLevels), ", ", trainLevels, "\n")
      #cat("score: ", length(scoreLevels), ", ", scoreLevels, "\n\n")
      scoreData[,varname] <- factor(scoreData[,varname], levels=trainLevels)
   }
}
rm(cup98val)


###################################################
### code chunk number 30: cs-response-prediction.rnw:697-705 (eval = FALSE)
###################################################
## # loading the selected model
## load("cup98-ctree-1000-400-4-10-run-7.Rdata")
## # predicting
## pred <- predict(myCtree, newdata = scoreData)
## pred <- round(pred, digits=3)
## #table(pred, useNA="ifany")
## result <- data.frame(scoreData$CONTROLN, pred)
## names(result) <- c("CONTROLN", "pred")


###################################################
### code chunk number 31: cs-response-prediction.rnw:710-711
###################################################
load("response-result.rdata")


###################################################
### code chunk number 32: cs-response-prediction.rnw:725-732
###################################################
valTarget <- read.csv("./data/KDDCup1998/valtargt.txt")
merged <- merge(result, valTarget, by="CONTROLN")
# donation profit if mail all people
sum(valTarget$TARGET_D - cost)
# donation profit if mail those predicted to donate more than mail cost
idx <- (merged$pred > cost)
sum(merged$TARGET_D[idx] - cost)


###################################################
### code chunk number 33: cs-response-prediction.rnw:739-748
###################################################
# ranking customers
merged <- merged[order(merged$pred, decreasing=T),]
x <- 100 * (1:nrow(merged)) / nrow(merged)
y <- cumsum(merged$TARGET_D) - cost*(1:nrow(valTarget))
# to reduce size of the file to save this chart
idx.pos <- c(seq(1, length(x), by=10), length(x))
plot(x[idx.pos], y[idx.pos], type="l", xlab="Contact Percentile (%)",
     ylab="Amount of Donation")
grid()


