### R code from vignette source 'cs-HPI-forecast.rnw'

###################################################
### code chunk number 1: cs-HPI-forecast.rnw:7-10
###################################################
# free memory
rm(list = ls())
gc()


###################################################
### code chunk number 2: cs-HPI-forecast.rnw:31-44
###################################################
# import data
filepath <- "./data/"
filename <- "House-index-canberra.csv"
houseIndex <- read.csv(paste(filepath, filename, sep=""), header=FALSE)
names(houseIndex) <- c("date", "index")
n <- nrow(houseIndex)
# check start date and end date
cat(paste("HPI from", houseIndex$date[1], "to", houseIndex$date[n], "\n"))
# extract year and month
dates <- strptime(houseIndex$date, format="%d-%b-%y")
houseIndex$year <- dates$year + 1900
houseIndex$month <- dates$mon + 1
fromYear <- houseIndex$year[1]


###################################################
### code chunk number 3: cs-HPI-forecast.rnw:48-51 (eval = FALSE)
###################################################
## dates <- as.Date(houseIndex$date, format="%d-%b-%y")
## houseIndex$year <- as.numeric(format(dates, "%Y"))
## houseIndex$month <- as.numeric(format(dates, "%m"))


###################################################
### code chunk number 4: cs-HPI-forecast.rnw:63-74
###################################################
plot(houseIndex$index, pty=1, type="l", lty="solid", xaxt="n", xlab="",
     ylab="Index", main=paste("HPI (Canberra) - Since ", fromYear, sep=""))
# draw tick-marks at 31 Jan of every year
nYear <- ceiling(n/12)
posEveryYear <- 12 * (1:nYear) - 11
axis(1, labels=houseIndex$date[posEveryYear], las=3, at=posEveryYear)
# add horizontal reference lines
abline(h=1:4, col="gray", lty="dotted")
# draw a vertical reference line every five years
posEvery5years <- 12 * (5* 1:ceiling(nYear/5) - 4) - 11
abline(v=posEvery5years, col="gray", lty="dotted")


###################################################
### code chunk number 5: cs-HPI-forecast.rnw:88-93
###################################################
houseIndex$delta <- houseIndex$index - c(1, houseIndex$index[-n])
plot(houseIndex$delta, main="Increase in HPI", xaxt="n", xlab="")
axis(1, labels=houseIndex$date[posEveryYear], las=3, at=posEveryYear)
# add a reference line
abline(h=0, lty="dotted")


###################################################
### code chunk number 6: cs-HPI-forecast.rnw:109-119
###################################################
# increase ratio in every month
houseIndex$rate <- houseIndex$index / c(1, houseIndex$index[-n]) - 1
# percentage of months having positive increases in HPI
100 * sum(houseIndex$rate>0)/n
# use ifelse() to set positive values to green and and negative ones to red
plot(houseIndex$rate, xaxt="n", xlab="", ylab="HPI Increase Rate",
     col=ifelse(houseIndex$rate>0,"green","red"),
     pch=ifelse(houseIndex$rate>0,"+","o"))
axis(1, labels=houseIndex$date[posEveryYear], las=3, at=posEveryYear)
abline(h=0, lty="dotted")


###################################################
### code chunk number 7: cs-HPI-forecast.rnw:134-141
###################################################
rateMatrix <- xtabs(rate ~ month + year, data=houseIndex)
# show the first four years, rounded to 4 decimal places
round(rateMatrix[,1:4], digits=4)
# plot a grouped barchart: 
barplot(rateMatrix, beside=TRUE, space=c(0,2),
        col=ifelse(rateMatrix>0,"lightgreen","lightpink"),
        ylab="HPI Increase Rate", cex.names=1.2)


###################################################
### code chunk number 8: cs-HPI-forecast.rnw:153-155
###################################################
numPositiveMonths <- colSums(rateMatrix>0)
barplot(numPositiveMonths, xlab="Year", ylab="Number of Months with Increased HPI")


###################################################
### code chunk number 9: cs-HPI-forecast.rnw:164-167
###################################################
yearlyMean <- colMeans(rateMatrix)
barplot(yearlyMean, main="Yearly Average Increase Rates of HPI", 
        col=ifelse(yearlyMean>0,"lightgreen","lightpink"), xlab="Year")


###################################################
### code chunk number 10: cs-HPI-forecast.rnw:178-181
###################################################
monthlyMean <- rowMeans(rateMatrix)
plot(names(monthlyMean), monthlyMean, type="b", xlab="Month",
     main="Monthly Average Increase Rates of HPI")


###################################################
### code chunk number 11: cs-HPI-forecast.rnw:193-195
###################################################
summary(houseIndex$rate)
boxplot(houseIndex$rate, ylab="HPI Increase Rate")


###################################################
### code chunk number 12: cs-HPI-forecast.rnw:205-206
###################################################
boxplot(rate ~ year, data=houseIndex, xlab="Year", ylab="HPI Increase Rate")


###################################################
### code chunk number 13: cs-HPI-forecast.rnw:215-216
###################################################
boxplot(rate ~ month, data=houseIndex, xlab="Month", ylab="HPI Increase Rate")


###################################################
### code chunk number 14: cs-HPI-forecast.rnw:234-237
###################################################
hpi <- ts(houseIndex$index, start=c(1990,1), frequency=12)
f <- stl(hpi, "per")
plot(f)


###################################################
### code chunk number 15: cs-HPI-forecast.rnw:247-250
###################################################
# plot seasonal components
plot(f$time.series[1:12,"seasonal"], type='b', xlab="Month", 
     ylab="Seasonal Components")


###################################################
### code chunk number 16: cs-HPI-forecast.rnw:258-263 (eval = FALSE)
###################################################
## # an alternative decomposition function
## f2 <- decompose(hpi)
## plot(f2)
## # plot seasonal components
## plot(f2$figure, type="b", xlab="Month", ylab="Seasonal Components")


###################################################
### code chunk number 17: cs-HPI-forecast.rnw:277-298
###################################################
startYear <- 1990
endYear <- 2010
# to forecast HPIs in the next four years
nYearAhead <- 4

fit <- arima(hpi, order=c(2,0,1), seasonal=list(order=c(2,1,0), period=12))
fore <- predict(fit, n.ahead=12*nYearAhead)
# error bounds at 95% confidence level
U <- fore$pred + 2 * fore$se
L <- fore$pred - 2 * fore$se
# plot original and predicted data, as well as error bounds
ts.plot(hpi, fore$pred, U, L, col=c("black", "blue","green","red"), 
        lty=c(1,5,2,2), gpars=list(xaxt="n",xlab=""),
        ylab="Index", main="House Price Trading Index Forecast (Canberra)")
# add labels, reference grid and legend        
years <- startYear:(endYear+nYearAhead+1)
axis(1, labels=paste("Jan ", years, sep=""), las=3, at=years)
grid()
legend("topleft", col=c("black", "blue","green","red"), lty=c(1,5,2,2),
       c("Actual Index", "Forecast", "Upper Bound (95% Confidence)", 
         "Lower Bound (95% Confidence)"))


###################################################
### code chunk number 18: cs-HPI-forecast.rnw:308-317
###################################################
ts.plot(fore$pred, U, L, col=c("blue","green","red"), 
        lty=c(5,2,2), gpars=list(xaxt="n",xlab=""),
        ylab="Index", main="House Price Trading Index Forecast (Canberra)")
years <- endYear + (1 : (nYearAhead+1))
axis(1, labels=paste("Jan ", years, sep=""), las=3, at=years)
grid(col = "gray", lty = "dotted")
legend("topleft", col=c("blue","green","red"), lty=c(5,2,2),
       c("Forecast", "Upper Bound (95% Confidence)", 
         "Lower Bound (95% Confidence)"))


###################################################
### code chunk number 19: cs-HPI-forecast.rnw:327-335
###################################################
newHpi <- ts(c(hpi, fore$pred), start=c(1990,1), frequency=12)
(startDate <- start(newHpi))
startYear <- startDate[1]
m <- 9 + (2009-startYear)*12
n <- 9 + (2011-startYear)*12
# percentage of increase
100 * (newHpi[n] / newHpi[m] - 1)
round(535000 * newHpi[n] / newHpi[m])


