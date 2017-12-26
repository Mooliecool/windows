### R code from vignette source 'ch-regression.rnw'

###################################################
### code chunk number 1: ch-regression.rnw:6-9
###################################################
# free memory
rm(list = ls())
gc()


###################################################
### code chunk number 2: ch-regression.rnw:35-43
###################################################
year <- rep(2008:2010, each=4)
quarter <- rep(1:4, 3)
cpi <- c(162.2, 164.6, 166.5, 166.0, 
         166.2, 167.0, 168.6, 169.5, 
         171.0, 172.1, 173.3, 174.0)
plot(cpi, xaxt="n", ylab="CPI", xlab="")
# draw x-axis
axis(1, labels=paste(year,quarter,sep="Q"), at=1:12, las=3)


###################################################
### code chunk number 3: ch-regression.rnw:50-52
###################################################
cor(year,cpi)
cor(quarter,cpi)


###################################################
### code chunk number 4: ch-regression.rnw:56-58
###################################################
fit <- lm(cpi ~ year + quarter)
fit


###################################################
### code chunk number 5: ch-regression.rnw:65-67
###################################################
(cpi2011 <- fit$coefficients[[1]] + fit$coefficients[[2]]*2011 +
            fit$coefficients[[3]]*(1:4))


###################################################
### code chunk number 6: ch-regression.rnw:72-74
###################################################
attributes(fit)
fit$coefficients


###################################################
### code chunk number 7: ch-regression.rnw:78-81
###################################################
# differences between observed values and fitted values
residuals(fit)
summary(fit)


###################################################
### code chunk number 8: ch-regression.rnw:87-88 (eval = FALSE)
###################################################
## plot(fit)


###################################################
### code chunk number 9: ch-regression.rnw:94-98
###################################################
## The chunk above simply output code to document, and the results are produced by the chunk below.
layout(matrix(c(1,2,3,4),2,2)) # 4 graphs per page 
plot(fit)
layout(matrix(1)) # change back to one graph per page 


###################################################
### code chunk number 10: ch-regression.rnw:110-113
###################################################
library(scatterplot3d)
s3d <- scatterplot3d(year, quarter, cpi, highlight.3d=T, type="h", lab=c(2,3))
s3d$plane3d(fit)


###################################################
### code chunk number 11: ch-regression.rnw:124-130
###################################################
data2011 <- data.frame(year=2011, quarter=1:4)
cpi2011 <- predict(fit, newdata=data2011)
style <- c(rep(1,12), rep(2,4))
plot(c(cpi, cpi2011), xaxt="n", ylab="CPI", xlab="", pch=style, col=style)
axis(1, at=1:16, las=3,
     labels=c(paste(year,quarter,sep="Q"), "2011Q1", "2011Q2", "2011Q3", "2011Q4"))


###################################################
### code chunk number 12: ch-regression.rnw:164-169
###################################################
data("bodyfat", package="mboost")
myFormula <- DEXfat ~ age + waistcirc + hipcirc + elbowbreadth + kneebreadth
bodyfat.glm <- glm(myFormula, family = gaussian("log"), data = bodyfat)
summary(bodyfat.glm)
pred <- predict(bodyfat.glm, type="response")


###################################################
### code chunk number 13: ch-regression.rnw:176-178
###################################################
plot(bodyfat$DEXfat, pred, xlab="Observed Values", ylab="Predicted Values")
abline(a=0, b=1)


