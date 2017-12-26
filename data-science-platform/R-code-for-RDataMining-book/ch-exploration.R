### R code from vignette source 'ch-exploration.rnw'

###################################################
### code chunk number 1: ch-exploration.rnw:6-9
###################################################
# free memory
rm(list = ls())
gc()


###################################################
### code chunk number 2: ch-exploration.rnw:20-24
###################################################
dim(iris)
names(iris)
str(iris)
attributes(iris)


###################################################
### code chunk number 3: ch-exploration.rnw:28-31
###################################################
iris[1:5,]
head(iris)
tail(iris)


###################################################
### code chunk number 4: ch-exploration.rnw:36-38
###################################################
iris[1:10, "Sepal.Length"]
iris$Sepal.Length[1:10]


###################################################
### code chunk number 5: ch-exploration.rnw:48-49
###################################################
summary(iris)


###################################################
### code chunk number 6: ch-exploration.rnw:53-55
###################################################
quantile(iris$Sepal.Length)
quantile(iris$Sepal.Length, c(.1, .3, .65))


###################################################
### code chunk number 7: ch-exploration.rnw:63-65
###################################################
var(iris$Sepal.Length)
hist(iris$Sepal.Length)


###################################################
### code chunk number 8: ch-exploration.rnw:74-75
###################################################
plot(density(iris$Sepal.Length))


###################################################
### code chunk number 9: ch-exploration.rnw:85-87
###################################################
table(iris$Species)
pie(table(iris$Species))


###################################################
### code chunk number 10: ch-exploration.rnw:95-96
###################################################
barplot(table(iris$Species))


###################################################
### code chunk number 11: ch-exploration.rnw:105-109
###################################################
cov(iris$Sepal.Length, iris$Petal.Length)
cov(iris[,1:4])
cor(iris$Sepal.Length, iris$Petal.Length)
cor(iris[,1:4])


###################################################
### code chunk number 12: ch-exploration.rnw:114-115
###################################################
aggregate(Sepal.Length ~ Species, summary, data=iris)


###################################################
### code chunk number 13: ch-exploration.rnw:123-124
###################################################
boxplot(Sepal.Length~Species, data=iris)


###################################################
### code chunk number 14: ch-exploration.rnw:134-135
###################################################
with(iris, plot(Sepal.Length, Sepal.Width, col=Species, pch=as.numeric(Species)))


###################################################
### code chunk number 15: ch-exploration.rnw:146-147
###################################################
plot(jitter(iris$Sepal.Length), jitter(iris$Sepal.Width))


###################################################
### code chunk number 16: ch-exploration.rnw:159-160
###################################################
pairs(iris)


###################################################
### code chunk number 17: ch-exploration.rnw:177-179
###################################################
library(scatterplot3d)
scatterplot3d(iris$Petal.Width, iris$Sepal.Length, iris$Sepal.Width)


###################################################
### code chunk number 18: ch-exploration.rnw:187-189 (eval = FALSE)
###################################################
## library(rgl) 
## plot3d(iris$Petal.Width, iris$Sepal.Length, iris$Sepal.Width)


###################################################
### code chunk number 19: ch-exploration.rnw:199-201
###################################################
distMatrix <- as.matrix(dist(iris[,1:4]))
heatmap(distMatrix)


###################################################
### code chunk number 20: ch-exploration.rnw:214-217
###################################################
library(lattice)
levelplot(Petal.Width~Sepal.Length*Sepal.Width, iris, cuts=9,
          col.regions=grey.colors(10)[10:1])


###################################################
### code chunk number 21: ch-exploration.rnw:229-231
###################################################
filled.contour(volcano, color=terrain.colors, asp=1, 
               plot.axes=contour(volcano, add=T))


###################################################
### code chunk number 22: ch-exploration.rnw:243-244
###################################################
persp(volcano, theta=25, phi=30, expand=0.5, col="lightblue")    


###################################################
### code chunk number 23: ch-exploration.rnw:257-259
###################################################
library(MASS)
parcoord(iris[1:4], col=iris$Species)


###################################################
### code chunk number 24: ch-exploration.rnw:269-271
###################################################
library(lattice)
parallelplot(~iris[1:4] | Species, data=iris)


###################################################
### code chunk number 25: ch-exploration.rnw:283-285
###################################################
library(ggplot2)
qplot(Sepal.Length, Sepal.Width, data=iris, facets=Species ~.)


###################################################
### code chunk number 26: ch-exploration.rnw:299-310 (eval = FALSE)
###################################################
## # save as a PDF file
## pdf("myPlot.pdf")
## x <- 1:50
## plot(x, log(x))
## graphics.off()
## #
## # Save as a postscript file
## postscript("myPlot2.ps")
## x <- -20:20
## plot(x, x^2)
## graphics.off()


