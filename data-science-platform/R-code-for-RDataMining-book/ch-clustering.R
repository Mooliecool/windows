### R code from vignette source 'ch-clustering.rnw'

###################################################
### code chunk number 1: ch-clustering.rnw:6-9
###################################################
# free memory
rm(list = ls())
gc()


###################################################
### code chunk number 2: ch-clustering.rnw:18-19
###################################################
set.seed(8953)


###################################################
### code chunk number 3: ch-clustering.rnw:22-25
###################################################
iris2 <- iris
iris2$Species <- NULL
(kmeans.result <- kmeans(iris2, 3)) 


###################################################
### code chunk number 4: ch-clustering.rnw:30-31
###################################################
table(iris$Species, kmeans.result$cluster)


###################################################
### code chunk number 5: ch-clustering.rnw:38-42
###################################################
plot(iris2[c("Sepal.Length", "Sepal.Width")], col = kmeans.result$cluster)
# plot cluster centers
points(kmeans.result$centers[,c("Sepal.Length", "Sepal.Width")], col = 1:3, 
       pch = 8, cex=2)


###################################################
### code chunk number 6: ch-clustering.rnw:62-68
###################################################
library(fpc)
pamk.result <- pamk(iris2)
# number of clusters
pamk.result$nc
# check clustering against actual species
table(pamk.result$pamobject$clustering, iris$Species)


###################################################
### code chunk number 7: ch-clustering.rnw:80-83
###################################################
layout(matrix(c(1,2),1,2)) # 2 graphs per page 
plot(pamk.result$pamobject)
layout(matrix(1)) # change back to one graph per page 


###################################################
### code chunk number 8: ch-clustering.rnw:95-97
###################################################
pam.result <- pam(iris2, 3)
table(pam.result$clustering, iris$Species)


###################################################
### code chunk number 9: ch-clustering.rnw:108-111
###################################################
layout(matrix(c(1,2),1,2)) # 2 graphs per page 
plot(pam.result)
layout(matrix(1)) # change back to one graph per page 


###################################################
### code chunk number 10: ch-clustering.rnw:131-132
###################################################
set.seed(2835)


###################################################
### code chunk number 11: ch-clustering.rnw:134-138
###################################################
idx <- sample(1:dim(iris)[1], 40)
irisSample <- iris[idx,]
irisSample$Species <- NULL
hc <- hclust(dist(irisSample), method="ave")


###################################################
### code chunk number 12: ch-clustering.rnw:144-148
###################################################
plot(hc, hang = -1, labels=iris$Species[idx])
# cut tree into 3 clusters
rect.hclust(hc, k=3)
groups <- cutree(hc, k=3)


###################################################
### code chunk number 13: ch-clustering.rnw:171-176
###################################################
library(fpc)
iris2 <- iris[-5] # remove class tags
ds <- dbscan(iris2, eps=0.42, MinPts=5)
# compare clusters with original class labels
table(ds$cluster, iris$Species)


###################################################
### code chunk number 14: ch-clustering.rnw:184-185
###################################################
plot(ds, iris2)


###################################################
### code chunk number 15: ch-clustering.rnw:196-197
###################################################
plot(ds, iris2[c(1,4)])


###################################################
### code chunk number 16: ch-clustering.rnw:206-207
###################################################
plotcluster(iris2, ds$cluster)


###################################################
### code chunk number 17: ch-clustering.rnw:217-229
###################################################
# create a new dataset for labeling
set.seed(435) 
idx <- sample(1:nrow(iris), 10)
newData <- iris[idx,-5]
newData <- newData + matrix(runif(10*4, min=0, max=0.2), nrow=10, ncol=4)
# label new data
myPred <- predict(ds, iris2, newData)
# plot result
plot(iris2[c(1,4)], col=1+ds$cluster)
points(newData[c(1,4)], pch="*", col=1+myPred, cex=3)
# check cluster labels
table(myPred, iris$Species[idx])


