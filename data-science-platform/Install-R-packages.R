## already included in R base
## rpart, parallel, data.table

## install R packages from CRAN
packages <- c("arules", "arulesSequences", "arulesViz", "caret", "cluster", "data.table", "dplyr", "e1071",
			  "ff", "fpc", "foreign", "ggplot2", "glmnet", 
              "Hmisc", "igraph", "knitr", "lattice", "lda",
              "magrittr", "MASS", "party", 
              "randomForest", "RColorBrewer", "rgl", "rJava", "rmarkdown", "ROAuth", "ROCR", "RODBC", 
              "scatterplot3d", "sna", "SnowballC", "snowfall", "stringi", "stringr",
              "TH.data", "topicmodels", "tm", "twitteR", 
              "visNetwork", "wordcloud", "xlsx")
install.packages(packages)


## install R packages from Bioconductor
source("http://bioconductor.org/biocLite.R")
biocLite("graph")
biocLite("Rgraphviz")


## install R packages from GitHub
install.packages("devtools")
library(devtools)
install_github("okugami79/sentiment140")