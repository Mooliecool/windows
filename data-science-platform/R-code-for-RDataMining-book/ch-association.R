### R code from vignette source 'ch-association.rnw'

###################################################
### code chunk number 1: ch-association.rnw:7-10
###################################################
# free memory
rm(list = ls())
gc()


###################################################
### code chunk number 2: ch-association.rnw:51-64
###################################################
str(Titanic)
df <- as.data.frame(Titanic)
head(df)
titanic.raw <- NULL
for(i in 1:4) {
   titanic.raw <- cbind(titanic.raw, rep(as.character(df[,i]), df$Freq))
}
titanic.raw <- as.data.frame(titanic.raw)
names(titanic.raw) <- names(df)[1:4]
dim(titanic.raw)
str(titanic.raw)
head(titanic.raw)
summary(titanic.raw)


###################################################
### code chunk number 3: ch-association.rnw:70-75
###################################################
# have a look at the 1st 5 lines
readLines("./data/Dataset.data", n=5)
# read it into R
titanic <- read.table("./data/Dataset.data", header=F)
names(titanic) <- c("Class", "Sex", "Age", "Survived")


###################################################
### code chunk number 4: ch-association.rnw:86-91
###################################################
library(arules)
# find association rules with default settings
rules.all <- apriori(titanic.raw)
rules.all
inspect(rules.all)


###################################################
### code chunk number 5: ch-association.rnw:96-103
###################################################
# rules with rhs containing "Survived" only
rules <- apriori(titanic.raw, control = list(verbose=F),
                 parameter = list(minlen=2, supp=0.005, conf=0.8),
                 appearance = list(rhs=c("Survived=No", "Survived=Yes"),
                                   default="lhs"))
quality(rules) <- round(quality(rules), digits=3)
rules.sorted <- sort(rules, by="lift")


###################################################
### code chunk number 6: ch-association.rnw:106-107
###################################################
inspect(rules.sorted)


###################################################
### code chunk number 7: ch-association.rnw:124-132
###################################################
# find redundant rules
subset.matrix <- is.subset(rules.sorted, rules.sorted)
subset.matrix[lower.tri(subset.matrix, diag=T)] <- NA
redundant <- colSums(subset.matrix, na.rm=T) >= 1
which(redundant)
# remove redundant rules
rules.pruned <- rules.sorted[!redundant]
inspect(rules.pruned)


###################################################
### code chunk number 8: ch-association.rnw:141-150
###################################################
rules <- apriori(titanic.raw, 
                 parameter = list(minlen=3, supp=0.002, conf=0.2),
                 appearance = list(rhs=c("Survived=Yes"),
                                   lhs=c("Class=1st", "Class=2nd", "Class=3rd",
                                         "Age=Child", "Age=Adult"),
                                   default="none"), 
                 control = list(verbose=F))
rules.sorted <- sort(rules, by="confidence")
inspect(rules.sorted)


###################################################
### code chunk number 9: ch-association.rnw:165-167
###################################################
library(arulesViz)
plot(rules.all)


###################################################
### code chunk number 10: ch-association.rnw:177-178
###################################################
plot(rules.all, method="grouped")


###################################################
### code chunk number 11: ch-association.rnw:188-189
###################################################
plot(rules.all, method="graph")


###################################################
### code chunk number 12: ch-association.rnw:199-200
###################################################
plot(rules.all, method="graph", control=list(type="items"))


###################################################
### code chunk number 13: ch-association.rnw:210-211
###################################################
plot(rules.all, method="paracoord", control=list(reorder=TRUE))


