
#Copyright Jonathan Moore 2017

main <- function() {
  ....
}

JournalSearch<-function(n)
{
  # List all available installed packages on your machine.
  installed.packages()
  search()
  # List all "attached" or loaded packages.
  # install rplos package
  install.packages("rplos")
  library('rplos')
  search()
  install.packages("fulltext")
  library('fulltext')
  search()

  searchplos("genetics", 'id,journal,title,publication_date', limit = 40)

}

foo <- eval(parse(text = paste("mean(x,", myoptions, ")")))











