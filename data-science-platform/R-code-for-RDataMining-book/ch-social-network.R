### R code from vignette source 'ch-social-network.rnw'

###################################################
### code chunk number 1: ch-social-network.rnw:6-9
###################################################
# free memory
rm(list = ls())
gc()


###################################################
### code chunk number 2: ch-social-network.rnw:21-31
###################################################
# load termDocMatrix
load("./data/termDocMatrix.rdata")
# inspect part of the matrix
termDocMatrix[5:10,1:20]
# change it to a Boolean matrix
termDocMatrix[termDocMatrix>=1] <- 1
# transform into a term-term adjacency matrix
termMatrix <- termDocMatrix %*% t(termDocMatrix)
# inspect terms numbered 5 to 10
termMatrix[5:10,5:10]


###################################################
### code chunk number 3: ch-social-network.rnw:35-43
###################################################
library(igraph)
# build a graph from the above matrix
g <- graph.adjacency(termMatrix, weighted=T, mode="undirected")
# remove loops
g <- simplify(g)
# set labels and degrees of vertices
V(g)$label <- V(g)$name
V(g)$degree <- degree(g)


###################################################
### code chunk number 4: ch-social-network.rnw:50-54
###################################################
# set seed to make the layout reproducible
set.seed(3952)
layout1 <- layout.fruchterman.reingold(g)
plot(g, layout=layout1)


###################################################
### code chunk number 5: ch-social-network.rnw:63-65 (eval = FALSE)
###################################################
## plot(g, layout=layout.kamada.kawai)
## tkplot(g, layout=layout.kamada.kawai)


###################################################
### code chunk number 6: ch-social-network.rnw:69-72 (eval = FALSE)
###################################################
## pdf("term-network.pdf")
## plot(g, layout=layout.fruchterman.reingold) 
## dev.off()


###################################################
### code chunk number 7: ch-social-network.rnw:79-87
###################################################
V(g)$label.cex <- 2.2 * V(g)$degree / max(V(g)$degree)+ .2
V(g)$label.color <- rgb(0, 0, .2, .8)
V(g)$frame.color <- NA
egam <- (log(E(g)$weight)+.4) / max(log(E(g)$weight)+.4)
E(g)$color <- rgb(.5, .5, 0, egam)
E(g)$width <- egam
# plot the graph in layout1
plot(g, layout=layout1)


###################################################
### code chunk number 8: ch-social-network.rnw:98-113
###################################################
# remove "r", "data" and "mining"
idx <- which(dimnames(termDocMatrix)$Terms %in% c("r", "data", "mining"))
M <- termDocMatrix[-idx,]
# build a tweet-tweet adjacency matrix
tweetMatrix <- t(M) %*% M
library(igraph)
g <- graph.adjacency(tweetMatrix, weighted=T, mode = "undirected")
V(g)$degree <- degree(g)
g <- simplify(g)
# set labels of vertices to tweet IDs
V(g)$label <- V(g)$name
V(g)$label.cex <- 1
V(g)$label.color <- rgb(.4, 0, 0, .7)
V(g)$size <- 2
V(g)$frame.color <- NA


###################################################
### code chunk number 9: ch-social-network.rnw:122-123
###################################################
barplot(table(V(g)$degree))


###################################################
### code chunk number 10: ch-social-network.rnw:133-145
###################################################
idx <- V(g)$degree == 0
V(g)$label.color[idx] <- rgb(0, 0, .3, .7)
# load twitter text
library(twitteR)
load(file = "data/rdmTweets.RData")
# convert tweets to a data frame
df <- do.call("rbind", lapply(rdmTweets, as.data.frame))
# set labels to the IDs and the first 20 characters of tweets
V(g)$label[idx] <- paste(V(g)$name[idx], substr(df$text[idx], 1, 20), sep=": ")
egam <- (log(E(g)$weight)+.2) / max(log(E(g)$weight)+.2)
E(g)$color <- rgb(.5, .5, 0, egam)
E(g)$width <- egam


###################################################
### code chunk number 11: ch-social-network.rnw:151-154
###################################################
set.seed(3152)
layout2 <- layout.fruchterman.reingold(g)
plot(g, layout=layout2) 


###################################################
### code chunk number 12: ch-social-network.rnw:162-163
###################################################
g2 <- delete.vertices(g, V(g)[degree(g)==0])


###################################################
### code chunk number 13: ch-social-network.rnw:169-170
###################################################
plot(g2, layout=layout.fruchterman.reingold) 


###################################################
### code chunk number 14: ch-social-network.rnw:179-181
###################################################
g3 <- delete.edges(g, E(g)[E(g)$weight <= 1])
g3 <- delete.vertices(g3, V(g3)[degree(g3) == 0])


###################################################
### code chunk number 15: ch-social-network.rnw:187-188
###################################################
plot(g3, layout=layout.fruchterman.reingold) 


###################################################
### code chunk number 16: ch-social-network.rnw:196-197 (eval = FALSE)
###################################################
## df$text[c(7,12,6,9,8,3,4)]


###################################################
### code chunk number 17: ch-social-network.rnw:200-205
###################################################
for (i in c(7,12,6,9,8,3,4)) {
  cat(paste("[", i, "] ", sep=""))
  writeLines(strwrap(df$text[i], 76))
  #cat("\n")
}


###################################################
### code chunk number 18: ch-social-network.rnw:232-253
###################################################
# create a graph
g <- graph.incidence(termDocMatrix, mode=c("all"))
# get index for term vertices and tweet vertices
nTerms <- nrow(M)
nDocs <- ncol(M)
idx.terms <- 1:nTerms
idx.docs <- (nTerms+1):(nTerms+nDocs)
# set colors and sizes for vertices
V(g)$degree <- degree(g)
V(g)$color[idx.terms] <- rgb(0, 1, 0, .5)
V(g)$size[idx.terms] <- 6
V(g)$color[idx.docs] <- rgb(1, 0, 0, .4)
V(g)$size[idx.docs] <- 4
V(g)$frame.color <- NA
# set vertex labels and their colors and sizes
V(g)$label <- V(g)$name
V(g)$label.color <- rgb(0, 0, 0, 0.5)
V(g)$label.cex <- 1.4*V(g)$degree/max(V(g)$degree) + 1
# set edge width and color
E(g)$width <- .3
E(g)$color <- rgb(.5, .5, 0, .3)


###################################################
### code chunk number 19: ch-social-network.rnw:260-262
###################################################
set.seed(958)
plot(g, layout=layout.fruchterman.reingold)


###################################################
### code chunk number 20: ch-social-network.rnw:270-271
###################################################
V(g)[nei("r")]


###################################################
### code chunk number 21: ch-social-network.rnw:275-276 (eval = FALSE)
###################################################
## V(g)[neighborhood(g, order=1, "r")[[1]]]


###################################################
### code chunk number 22: ch-social-network.rnw:280-281
###################################################
(rdmVertices <- V(g)[nei("r") & nei("data") & nei("mining")])


###################################################
### code chunk number 23: ch-social-network.rnw:283-284 (eval = FALSE)
###################################################
## df$text[as.numeric(rdmVertices$label)]


###################################################
### code chunk number 24: ch-social-network.rnw:287-292
###################################################
for (i in as.numeric(rdmVertices$label)[1:10]) {
  cat(paste("[", i, "] ", sep=""))
  writeLines(strwrap(df$text[i], 76))
  #cat("\n")
}


###################################################
### code chunk number 25: ch-social-network.rnw:302-307
###################################################
idx <- which(V(g)$name %in% c("r", "data", "mining"))
g2 <- delete.vertices(g, V(g)[idx-1])
g2 <- delete.vertices(g2, V(g2)[degree(g2)==0])
set.seed(209)
plot(g2, layout=layout.fruchterman.reingold)


