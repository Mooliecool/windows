### R code from vignette source 'ch-text-mining.rnw'

###################################################
### code chunk number 1: ch-text-mining.rnw:6-9
###################################################
# free memory
rm(list = ls())
gc()


###################################################
### code chunk number 2: ch-text-mining.rnw:27-32 (eval = FALSE)
###################################################
## library(twitteR)
## # retrieve the first 200 tweets (or all tweets if fewer than 200) from the 
## # user timeline of @rdatammining
## rdmTweets <- userTimeline("rdatamining", n=200)
## (nDocs <- length(rdmTweets))


###################################################
### code chunk number 3: ch-text-mining.rnw:36-39
###################################################
library(twitteR)
load(file = "data/rdmTweets.RData")
(nDocs <- length(rdmTweets))


###################################################
### code chunk number 4: ch-text-mining.rnw:49-50 (eval = FALSE)
###################################################
## rdmTweets[11:15]


###################################################
### code chunk number 5: ch-text-mining.rnw:54-58
###################################################
for (i in 11:15) {
  cat(paste("[[", i, "]] ", sep=""))
  writeLines(strwrap(rdmTweets[[i]]$getText(), width=73))
}  


###################################################
### code chunk number 6: ch-text-mining.rnw:71-78
###################################################
# convert tweets to a data frame
df <- do.call("rbind", lapply(rdmTweets, as.data.frame))
dim(df)

library(tm)  
# build a corpus, and specify the source to be character vectors
myCorpus <- Corpus(VectorSource(df$text))


###################################################
### code chunk number 7: ch-text-mining.rnw:82-97
###################################################
# convert to lower case  
myCorpus <- tm_map(myCorpus, tolower)  
# remove punctuation  
myCorpus <- tm_map(myCorpus, removePunctuation) 
# remove numbers  
myCorpus <- tm_map(myCorpus, removeNumbers)
# remove URLs
removeURL <- function(x) gsub("http[[:alnum:]]*", "", x)
myCorpus <- tm_map(myCorpus, removeURL)
# add two extra stop words: "available" and "via"
myStopwords <- c(stopwords('english'), "available", "via")
# remove "r" and "big" from stopwords
myStopwords <- setdiff(myStopwords, c("r", "big"))
# remove stopwords from corpus
myCorpus <- tm_map(myCorpus, removeWords, myStopwords)  


###################################################
### code chunk number 8: ch-text-mining.rnw:110-121
###################################################
# keep a copy of corpus to use later as a dictionary for stem completion
myCorpusCopy <- myCorpus
# stem words
myCorpus <- tm_map(myCorpus, stemDocument)
# inspect documents (tweets) numbered 11 to 15
# inspect(myCorpus[11:15])
# The code below is used for to make text fit for paper width
for (i in 11:15) {
  cat(paste("[[", i, "]] ", sep=""))
  writeLines(strwrap(myCorpus[[i]], width=73))
}


###################################################
### code chunk number 9: ch-text-mining.rnw:125-127
###################################################
# stem completion
myCorpus <- tm_map(myCorpus, stemCompletion, dictionary=myCorpusCopy)


###################################################
### code chunk number 10: ch-text-mining.rnw:132-133 (eval = FALSE)
###################################################
## inspect(myCorpus[11:15])


###################################################
### code chunk number 11: ch-text-mining.rnw:137-141
###################################################
for (i in 11:15) {
  cat(paste("[[", i, "]] ", sep=""))
  writeLines(strwrap(myCorpus[[i]], width=73))
}  


###################################################
### code chunk number 12: ch-text-mining.rnw:156-164
###################################################
# count frequency of "mining"
miningCases <- tm_map(myCorpusCopy, grep, pattern="\\<mining")
sum(unlist(miningCases))
# count frequency of "miners"
minerCases <- tm_map(myCorpusCopy, grep, pattern="\\<miners")
sum(unlist(minerCases))
# replace "miners" with "mining"
myCorpus <- tm_map(myCorpus, gsub, pattern="miners", replacement="mining")


###################################################
### code chunk number 13: ch-text-mining.rnw:176-178
###################################################
myTdm <- TermDocumentMatrix(myCorpus, control=list(wordLengths=c(1,Inf)))
myTdm


###################################################
### code chunk number 14: ch-text-mining.rnw:182-184
###################################################
idx <- which(dimnames(myTdm)$Terms == "r")
inspect(myTdm[idx+(0:5),101:110])


###################################################
### code chunk number 15: ch-text-mining.rnw:188-189 (eval = FALSE)
###################################################
## myTdm <- TermDocumentMatrix(myCorpus, control=list(minWordLength=1))


###################################################
### code chunk number 16: ch-text-mining.rnw:201-203
###################################################
# inspect frequent words
findFreqTerms(myTdm, lowfreq=10)


###################################################
### code chunk number 17: ch-text-mining.rnw:213-217
###################################################
termFrequency <- rowSums(as.matrix(myTdm))
termFrequency <- subset(termFrequency, termFrequency>=10)
library(ggplot2)
qplot(names(termFrequency), termFrequency, geom="bar", xlab="Terms")  + coord_flip()


###################################################
### code chunk number 18: ch-text-mining.rnw:225-226 (eval = FALSE)
###################################################
## barplot(termFrequency, las=2)


###################################################
### code chunk number 19: ch-text-mining.rnw:231-235
###################################################
# which words are associated with "r"? 
findAssocs(myTdm, 'r', 0.25)  
# which words are associated with "mining"?
findAssocs(myTdm, 'mining', 0.25)


###################################################
### code chunk number 20: wordcloud
###################################################
library(wordcloud)
m <- as.matrix(myTdm)
# calculate the frequency of words and sort it descendingly by frequency
wordFreq <- sort(rowSums(m), decreasing=TRUE)
# word cloud
set.seed(375) # to make it reproducible
grayLevels <- gray( (wordFreq+10) / (max(wordFreq)+10) )
wordcloud(words=names(wordFreq), freq=wordFreq, min.freq=3, random.order=F,
          colors=grayLevels)


###################################################
### code chunk number 21: ch-text-mining.rnw:273-279
###################################################
# remove sparse terms
myTdm2 <- removeSparseTerms(myTdm, sparse=0.95)
m2 <- as.matrix(myTdm2)
# cluster terms
distMatrix <- dist(scale(m2))
fit <- hclust(distMatrix, method="ward")


###################################################
### code chunk number 22: ch-text-mining.rnw:283-286
###################################################
# save m2 for social network analysis later
termDocMatrix <- m2
save(termDocMatrix, file="data/termDocMatrix.rdata")


###################################################
### code chunk number 23: ch-text-mining.rnw:292-296
###################################################
plot(fit)
# cut tree into 10 clusters  
rect.hclust(fit, k=10)
(groups <- cutree(fit, k=10))


###################################################
### code chunk number 24: ch-text-mining.rnw:313-322
###################################################
# transpose the matrix to cluster documents (tweets)
m3 <- t(m2)
# set a fixed random seed
set.seed(122)
# k-means clustering of tweets
k <- 8
kmeansResult <- kmeans(m3, k)
# cluster centers
round(kmeansResult$centers, digits=3)


###################################################
### code chunk number 25: ch-text-mining.rnw:326-333
###################################################
for (i in 1:k) {
  cat(paste("cluster ", i, ":  ", sep=""))
  s <- sort(kmeansResult$centers[i,], decreasing=T)
  cat(names(s)[1:3], "\n")
  # print the tweets of every cluster
  # print(rdmTweets[which(kmeansResult$cluster==i)])
}


###################################################
### code chunk number 26: ch-text-mining.rnw:343-356
###################################################
library(fpc)
# partitioning around medoids with estimation of number of clusters
pamResult <- pamk(m3, metric="manhattan")
# number of clusters identified
(k <- pamResult$nc)
pamResult <- pamResult$pamobject
# print cluster medoids
for (i in 1:k) {
  cat(paste("cluster", i, ":  "))
  cat(colnames(pamResult$medoids)[which(pamResult$medoids[i,]==1)], "\n")
  # print tweets in cluster i
  # print(rdmTweets[pamResult$clustering==i])
}


###################################################
### code chunk number 27: ch-text-mining.rnw:363-368
###################################################
# plot clustering result
layout(matrix(c(1,2),2,1)) # set to two graphs per page 
plot(pamResult, color=F, labels=4, lines=0, cex=.8, col.clus=1,
     col.p=pamResult$clustering)
layout(matrix(1)) # change back to one graph per page 


###################################################
### code chunk number 28: ch-text-mining.rnw:380-381
###################################################
pamResult2 <- pamk(m3, krange=2:8, metric="manhattan")


