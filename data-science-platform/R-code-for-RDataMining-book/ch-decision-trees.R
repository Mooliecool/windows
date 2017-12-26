### R code from vignette source 'ch-decision-trees.rnw'

###################################################
### code chunk number 1: ch-decision-trees.rnw:6-9
###################################################
# free memory
rm(list = ls())
gc()


###################################################
### code chunk number 2: ch-decision-trees.rnw:23-28
###################################################
str(iris)
set.seed(1234) 
ind <- sample(2, nrow(iris), replace=TRUE, prob=c(0.7, 0.3))
trainData <- iris[ind==1,]
testData <- iris[ind==2,]


###################################################
### code chunk number 3: ch-decision-trees.rnw:32-37
###################################################
library(party)
myFormula <- Species ~ Sepal.Length + Sepal.Width + Petal.Length + Petal.Width
iris_ctree <- ctree(myFormula, data=trainData)
# check the prediction
table(predict(iris_ctree), trainData$Species)


###################################################
### code chunk number 4: ch-decision-trees.rnw:42-43
###################################################
print(iris_ctree)


###################################################
### code chunk number 5: ch-decision-trees.rnw:49-50
###################################################
plot(iris_ctree)


###################################################
### code chunk number 6: ch-decision-trees.rnw:59-60
###################################################
plot(iris_ctree, type="simple")


###################################################
### code chunk number 7: ch-decision-trees.rnw:71-74
###################################################
# predict on test data
testPred <- predict(iris_ctree, newdata = testData)
table(testPred, testData$Species)


###################################################
### code chunk number 8: ch-decision-trees.rnw:88-92
###################################################
data("bodyfat", package = "mboost")
dim(bodyfat)
attributes(bodyfat)
bodyfat[1:5,]


###################################################
### code chunk number 9: ch-decision-trees.rnw:96-108
###################################################
set.seed(1234) 
ind <- sample(2, nrow(bodyfat), replace=TRUE, prob=c(0.7, 0.3))
bodyfat.train <- bodyfat[ind==1,]
bodyfat.test <- bodyfat[ind==2,]
# train a decision tree
library(rpart)
myFormula <- DEXfat ~ age + waistcirc + hipcirc + elbowbreadth + kneebreadth
bodyfat_rpart <- rpart(myFormula, data = bodyfat.train, 
                       control = rpart.control(minsplit = 10))
attributes(bodyfat_rpart)
print(bodyfat_rpart$cptable)
print(bodyfat_rpart)


###################################################
### code chunk number 10: ch-decision-trees.rnw:114-116
###################################################
plot(bodyfat_rpart)
text(bodyfat_rpart, use.n=T)


###################################################
### code chunk number 11: ch-decision-trees.rnw:127-133
###################################################
opt <- which.min(bodyfat_rpart$cptable[,"xerror"])
cp <- bodyfat_rpart$cptable[opt, "CP"]
bodyfat_prune <- prune(bodyfat_rpart, cp = cp)
print(bodyfat_prune)
plot(bodyfat_prune)
text(bodyfat_prune, use.n=T)


###################################################
### code chunk number 12: ch-decision-trees.rnw:143-148
###################################################
DEXfat_pred <- predict(bodyfat_prune, newdata=bodyfat.test)
xlim <- range(bodyfat$DEXfat)
plot(DEXfat_pred ~ DEXfat, data=bodyfat.test, xlab="Observed", 
     ylab="Predicted", ylim=xlim, xlim=xlim)
abline(a=0, b=1)


###################################################
### code chunk number 13: ch-decision-trees.rnw:162-165
###################################################
ind <- sample(2, nrow(iris), replace=TRUE, prob=c(0.7, 0.3))
trainData <- iris[ind==1,]
testData <- iris[ind==2,]


###################################################
### code chunk number 14: ch-decision-trees.rnw:169-174
###################################################
library(randomForest)
rf <- randomForest(Species ~ ., data=trainData, ntree=100, proximity=TRUE)
table(predict(rf), trainData$Species)
print(rf)
attributes(rf)


###################################################
### code chunk number 15: ch-decision-trees.rnw:182-183
###################################################
plot(rf)


###################################################
### code chunk number 16: ch-decision-trees.rnw:193-195
###################################################
importance(rf)
varImpPlot(rf)


###################################################
### code chunk number 17: ch-decision-trees.rnw:205-208
###################################################
irisPred <- predict(rf, newdata=testData)
table(irisPred, testData$Species)
plot(margin(rf, testData$Species))


