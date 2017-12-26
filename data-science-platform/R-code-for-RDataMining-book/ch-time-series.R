### R code from vignette source 'ch-time-series.rnw'

###################################################
### code chunk number 1: ch-time-series.rnw:6-9
###################################################
# free memory
rm(list = ls())
gc()


###################################################
### code chunk number 2: ch-time-series.rnw:22-26
###################################################
a <- ts(1:30, frequency=12, start=c(2011,3))
print(a)
str(a)
attributes(a)


###################################################
### code chunk number 3: ch-time-series.rnw:44-45
###################################################
plot(AirPassengers)


###################################################
### code chunk number 4: ch-time-series.rnw:57-68
###################################################
# decompose time series
apts <- ts(AirPassengers, frequency=12)
f <- decompose(apts)
# seasonal figures
f$figure
plot(f$figure, type="b", xaxt="n", xlab="")
# get names of 12 months in English words
monthNames <- months(ISOdate(2011,1:12,1))
# label x-axis with month names 
# las is set to 2 for vertical label orientation
axis(1, at=1:12, labels=monthNames, las=2) 


###################################################
### code chunk number 5: ch-time-series.rnw:77-78
###################################################
plot(f)


###################################################
### code chunk number 6: ch-time-series.rnw:100-108
###################################################
fit <- arima(AirPassengers, order=c(1,0,0), list(order=c(2,1,0), period=12))
fore <- predict(fit, n.ahead=24)
# error bounds at 95% confidence level
U <- fore$pred + 2*fore$se
L <- fore$pred - 2*fore$se
ts.plot(AirPassengers, fore$pred, U, L, col=c(1,2,4,4), lty = c(1,1,2,2))
legend("topleft", c("Actual", "Forecast", "Error Bounds (95% Confidence)"),
       col=c(1,2,4), lty=c(1,1,2))


###################################################
### code chunk number 7: ch-time-series.rnw:131-137
###################################################
library(dtw)
idx <- seq(0, 2*pi, len=100)
a <- sin(idx) + runif(100)/10
b <- cos(idx)
align <- dtw(a, b, step=asymmetricP1, keep=T)
dtwPlotTwoWay(align)


###################################################
### code chunk number 8: ch-time-series.rnw:162-167
###################################################
sc <- read.table("./data/synthetic_control.data", header=F, sep="")
# show one sample from each class
idx <- c(1,101,201,301,401,501)
sample1 <- t(sc[idx,])
plot.ts(sample1, main="")


###################################################
### code chunk number 9: ch-time-series.rnw:182-183
###################################################
set.seed(6218)


###################################################
### code chunk number 10: ch-time-series.rnw:185-197
###################################################
n <- 10
s <- sample(1:100, n)
idx <- c(s, 100+s, 200+s, 300+s, 400+s, 500+s)
sample2 <- sc[idx,]
observedLabels <- rep(1:6, each=n)
# hierarchical clustering with Euclidean distance
hc <- hclust(dist(sample2), method="average")
plot(hc, labels=observedLabels, main="")
# cut tree to get 6 clusters
rect.hclust(hc, k=6)
memb <- cutree(hc, k=6)
table(observedLabels, memb)


###################################################
### code chunk number 11: ch-time-series.rnw:216-224
###################################################
library(dtw)
distMatrix <- dist(sample2, method="DTW")
hc <- hclust(distMatrix, method="average")
plot(hc, labels=observedLabels, main="")
# cut tree to get 6 clusters
rect.hclust(hc, k=6)
memb <- cutree(hc, k=6)
table(observedLabels, memb)


###################################################
### code chunk number 12: ch-time-series.rnw:248-258
###################################################
classId <- rep(as.character(1:6), each=100)
newSc <- data.frame(cbind(classId, sc))
library(party)
ct <- ctree(classId ~ ., data=newSc, 
            controls = ctree_control(minsplit=30, minbucket=10, maxdepth=5))
pClassId <- predict(ct)
table(classId, pClassId)
# accuracy
(sum(classId==pClassId)) / nrow(sc)
plot(ct, ip_args=list(pval=FALSE), ep_args=list(digits=0))


###################################################
### code chunk number 13: ch-time-series.rnw:273-282
###################################################
library(wavelets)
wtData <- NULL
for (i in 1:nrow(sc)) {
  a <- t(sc[i,])
  wt <- dwt(a, filter="haar", boundary="periodic")
  wtData <- rbind(wtData, unlist(c(wt@W, wt@V[[wt@level]])))
}
wtData <- as.data.frame(wtData)
wtSc <- data.frame(cbind(classId, wtData))


###################################################
### code chunk number 14: ch-time-series.rnw:290-297
###################################################
# build a decision tree with DWT coefficients
ct <- ctree(classId ~ ., data=wtSc, 
            controls = ctree_control(minsplit=30, minbucket=10, maxdepth=5))
pClassId <- predict(ct)
table(classId, pClassId)
(sum(classId==pClassId)) / nrow(wtSc)
plot(ct, ip_args=list(pval=FALSE), ep_args=list(digits=0))


###################################################
### code chunk number 15: ch-time-series.rnw:308-310
###################################################
# fix seed to get a fixed result in the chunk below
set.seed(100)


###################################################
### code chunk number 16: ch-time-series.rnw:313-320
###################################################
k <- 20
# create a new time series by adding noise to time series 501
newTS <- sc[501,] + runif(100)*15
distances <- dist(newTS, sc, method="DTW")
s <- sort(as.vector(distances), index.return=TRUE)
# class IDs of k nearest neighbors
table(classId[s$ix[1:k]])


