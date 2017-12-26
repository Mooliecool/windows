### R code from vignette source 'ch-outlier.rnw'

###################################################
### code chunk number 1: ch-outlier.rnw:6-9
###################################################
# free memory
rm(list = ls())
gc()


###################################################
### code chunk number 2: ch-outlier.rnw:24-30
###################################################
set.seed(3147)
x <- rnorm(100)
summary(x)
# outliers
boxplot.stats(x)$out
boxplot(x)


###################################################
### code chunk number 3: ch-outlier.rnw:38-48
###################################################
y <- rnorm(100)
df <- data.frame(x, y)
rm(x, y)
head(df)
attach(df)
# find the index of outliers from x
(a <- which(x %in% boxplot.stats(x)$out))
# find the index of outliers from y
(b <- which(y %in% boxplot.stats(y)$out))
detach(df)


###################################################
### code chunk number 4: ch-outlier.rnw:54-58
###################################################
# outliers in both x and y
(outlier.list1 <- intersect(a,b))
plot(df)
points(df[outlier.list1,], col="red", pch="+", cex=2.5)


###################################################
### code chunk number 5: ch-outlier.rnw:69-73
###################################################
# outliers in either x or y
(outlier.list2 <- union(a,b))
plot(df)
points(df[outlier.list2,], col="blue", pch="x", cex=2)


###################################################
### code chunk number 6: ch-outlier.rnw:94-99
###################################################
library(DMwR)
# remove "Species", which is a categorical column
iris2 <- iris[,1:4]
outlier.scores <- lofactor(iris2, k=5)
plot(density(outlier.scores))


###################################################
### code chunk number 7: ch-outlier.rnw:105-110
###################################################
# pick top 5 as outliers
outliers <- order(outlier.scores, decreasing=T)[1:5]
# who are outliers
print(outliers)
print(iris2[outliers,])


###################################################
### code chunk number 8: ch-outlier.rnw:118-122
###################################################
n <- nrow(iris2)
labels <- 1:n
labels[-outliers] <- "."
biplot(prcomp(iris2), cex=.8, xlabs=labels)


###################################################
### code chunk number 9: ch-outlier.rnw:135-140
###################################################
pch <- rep(".", n)
pch[outliers] <- "+"
col <- rep("black", n)
col[outliers] <- "red"
pairs(iris2, pch=pch, col=col)


###################################################
### code chunk number 10: ch-outlier.rnw:148-152 (eval = FALSE)
###################################################
## library(Rlof)
## outlier.scores <- lof(iris2, k=5)
## # try with different number of neighbors (k = 5,6,7,8,9 and 10)
## outlier.scores <- lof(iris2, k=c(5:10))


###################################################
### code chunk number 11: ch-outlier.rnw:164-179
###################################################
# remove species from the data to cluster
iris2 <- iris[,1:4]
kmeans.result <- kmeans(iris2, centers=3)
# cluster centers
kmeans.result$centers
# cluster IDs
kmeans.result$cluster
# calculate distances between objects and cluster centers
centers <- kmeans.result$centers[kmeans.result$cluster, ]
distances <- sqrt(rowSums((iris2 - centers)^2))
# pick top 5 largest distances
outliers <- order(distances, decreasing=T)[1:5]
# who are outliers
print(outliers)
print(iris2[outliers,])


###################################################
### code chunk number 12: ch-outlier.rnw:185-193
###################################################
# plot clusters
plot(iris2[,c("Sepal.Length", "Sepal.Width")], pch="o", 
     col=kmeans.result$cluster, cex=0.3)
# plot cluster centers
points(kmeans.result$centers[,c("Sepal.Length", "Sepal.Width")], col=1:3, 
       pch=8, cex=1.5)
# plot outliers
points(iris2[outliers, c("Sepal.Length", "Sepal.Width")], pch="+", col=4, cex=1.5)


###################################################
### code chunk number 13: ch-outlier.rnw:209-219
###################################################
# use robust fitting
f <- stl(AirPassengers, "periodic", robust=TRUE)
(outliers <- which(f$weights<1e-8))
# set layout
op <- par(mar=c(0, 4, 0, 3), oma=c(5, 0, 4, 0), mfcol=c(4, 1))
plot(f, set.pars=NULL)
sts <- f$time.series
# plot outliers
points(time(sts)[outliers], 0.8*sts[,"remainder"][outliers], pch="x", col="red")
par(op) # reset layout


